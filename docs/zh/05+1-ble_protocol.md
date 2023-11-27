# 蓝牙参数

目前使用NRF52 SDK提供Nordic UART Service，提供一个BLE的虚拟串口来和主机通信。

设备名：Pixl.js 

服务列表：

1. Nordic UART Service

NUS_SERVICE_UUID: 6e400001-b5a3-f393-e0a9-e50e24dcca9e

提供了两个特性：

* NUS_CHAR_TX_UUID: 6e400002-b5a3-f393-e0a9-e50e24dcca9e => 提供数据的发送，支持写入和无返回的写入。
* NUS_CHAR_RX_UUID: 6e400003-b5a3-f393-e0a9-e50e24dcca9e => 提供数据的读取，仅支持通知。

（上述描述符正式版本可能会修改。）

# 协议格式

## 协议定义

<p>
考虑到效率，BLE的协议为二进制协议。
此协议为请求应答协议，所有的请求都必须由客户端发起。
为简化协议实现，每个请求只会有一个回复包。
</p>

### 包结构定义

为了简化协议，请求和回复都使用相同的包结构。

通用的包结构如下

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  命令类型  |
| status | uint8 | 1 | 状态码，见下面状态码定义，请求包这个固定为0 |
| chunk | uint16 | 2 | 分片序号 | 
| data   | byte | N | 数据，和具体的包定义有关 |
 

### chunk 传输模式

目前pixl.js设备定义的MTU为250，在一个ATT数据传输中，最多可以传输247个字节数据。

包最大字节：
固定的包头部为4个字节，因此每个包最多能传输243个字节。如果传输的数据超过243字节则需要分块传输。

<p>
当传输的请求或者返回结果超过MTU时，需分块传输，每次传输chunk值加1。
chunk最高位为1时，代表数据还有数据继续发送，为0时，代表数据读取完毕。
</p>

<p>
写入文件和读取文件则需要启用chunk传输。
chunk 字段为2个字节，chunk传输流最多能传输7.8M字节。
超过这个7.8M字节就需要开启新的chunk传输流。
</p>

比如写入文件流程：

```
c -> s: 0x12，mode: wc, 打开文件
s -> c: 0x12, status 0, file id 0xa1，文件已经创建好，可以写入
c -> s: 0x14, chunk 0 | 0x8000, file id 0xa1,  246字节数据
c -> s: 0x14, chunk 1 | 0x8000, file id 0xa1, 246字节数据
...
c -> s: 0x14, chunk N , file id 0xa1, 56字节数据
s -> c: 0x14， status 0, 写入完毕
c -> s: 0x13, file id 0xa1，关闭文件，
s -> c: 0x13, status 0, 
```

读取文件流程

```
c -> s: 0x12，mode: r, 打开文件
s -> c: 0x12, status 0, file id 0xa1，文件已经打开，可以读取
c -> s: 0x15, chunk 0 , file id 0xa1
s -> c: 0x15, status 0, chunk 1 | 0x8000, file id 0xa1, 246字节数据
...
s -> c: 0x15, status 0, chunk N , file id 0xa1, 56字节数据
c -> s: 0x13, file id 0xa1，关闭文件，
s -> c: 0x13, status 0, 
```
### 路径定义

<p>
文件操作对于文件需要传输完整的文件路径。标准的带盘符的Windows路径表达式。
比如 E:/amiibo/mifa.bin。
</p>

### 类型 

<p>
数值类型都是无符号整数，小端模式。 </br>
字符串编码为 2字节长度 + 字符串数组，字符串统一使用utf8编码。
</p>

### 文件元信息
文件元信息用于存储和文件相关的一些数据最大128个字节，基于type字段区分，数据不定长，根据类型解析数据。 <br />

基本格式如下：

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| meta size  | uint8 | 1  | 所有元信息的大小, 0xFF或者0无元数据  |
| meta N type  | uint8 | 1  | 元信息类型，定义见下面小节 |
| meta N data | byte | N | 长度根据类型的不同有区别 |

目前定义的类型可以取如下的值：

1: amiibo备注
| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| note length | uint8 | 1 | 备注长度 |
| note data | byte | N | 备注字符串，最大只能90个字节，utf8编码。 |

2: 文件属性
| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| flag | uint8 | N | 文件的属性， 按位定义 |

* 0b0000_0001: 文件设置为隐藏属性，在amiibo页面不予显示，但可以在网页端显示。settings.bin默认会设置这个标记。
* 0bxxxx_xxxx: 其他位，暂时未定义。

3: amiibo属性
| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| head | uint32 | N | amiibo id head |
| tail | uint32 | N | amiibo id tail |

注：
* 此属性目前仅在amiibo/data/*.bin中使用，用于快速识别是否是amiibo文件。



### 全局状态码定义

| 状态码 | 说明 |
| ---- | ---- |
| 0 | 请求已经成功完成 |
| 0xFF | 固件不支持当前报文 |

TODO 详细补充错误码。。



# 协议定义 

## 0x01: 获取版本信息

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x01  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  | 0x01  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 | 0 | 
| version length | uint16 | 2 |  版本字符串长度 |
| version  |  byte | N | 版本 | 


## 0x02: 进入DFU模式 

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x02  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  | 0x102  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 | 0 | 


## 0x10：获取磁盘列表

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x10  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x10  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 
| drive count| uint8 | 1 | 磁盘数量 |
| drive N status code | uint8 | 1 | 磁盘N状态码  0：磁盘可用，非0磁盘不可用| 
| drive N label |char | 1| 磁盘盘符 |
| drive N name length | uint16 | 2 | 磁盘名长度 |
| drive N name | byte | N | 磁盘名字符串 |
| drive N total size |uint32 | 4 | 磁盘总空间 |
| drive N used size | uint32  | 4 | 磁盘可用空间 |


## 0x11：格式化磁盘

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x11  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| drive label | char | 1 | 磁盘盘符：E/I |


## 0x12: 打开文件 

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x12  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | 路径长度 |
| path  | byte | N | 路径字符串 |
| mode | uint8 | 1 | 打开模式 |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  固定0x12  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 
| file id | uint8 | 1 | 文件ID，用于后续的读取写入请求 |

## 0x13: 关闭文件 

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x13  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| file id | uint8 | 1 | 文件ID |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x13  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 
| file id | uint8 | 1 | 文件ID，用于后续的读取写入请求 |



## 0x14: 读取文件

1. 客户端发送请求

目前仅支持顺序读取文件

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x14  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| file id | uint8 | 1| file id |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x14  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  启用chunk传输 | 
| data  | byte | N | 文件数据 |


## 0x15 写入文件

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x15  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 启用chunk传输 | 
| file id | uint8 | 1 | 需要写入的file id | 
| data | byte | N | 文件数据，最大能发送 MTU - 4 个字节 |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  固定0x15  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  和请求一致 | 


## 0x16：读取文件夹

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x16  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| path length | uint16 | 2 | 路径长度 |
| path  | byte | N | 路径字符串 |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x16  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  启用chunk传输 | 
| file N name length| uint16 | 2 | 文件名长度|
| file N name | byte | N| 文件名 |
| file N size | uint32 | 4 | 文件大小 |
| file N type | uint8 | 1 | 文件类型： 0 => 文件，1：文件夹 |
| file N meta length | uint8 | 1 | 文件元信息长度，最大只能有64个字节|
| file N meta | byte | N | 文件元信息数据|

## 0x17 创建文件夹

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x17  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | 路径长度 |
| path  | byte | N | 路径字符串 |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x17  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 



## 0x18 删除文件或文件夹

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | 路径长度 |
| path  | byte | N | 路径字符串 |


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  固定0x18  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 


## 0x19 重命名文件或者文件夹

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 启用chunk传输 | 
| old path length | uint16 | 2 | 路径长度 |
| old path  | byte | N | 路径字符串 |
| new path length | uint16 | 2 | 路径长度 |
| new path  | byte | N | 路径字符串 |

包体可能会超MTU。。。注意要分块传输。


2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  固定0x18  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 


## 0x1A 更新文件元信息

1. 客户端发送请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x1a  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | 路径长度 |
| path  | byte | N | 路径字符串 |
| meta data | byte | N | 元信息数据，参考"文件元信息"小节定义 | 

2. 服务端响应请求

| 字段名 | 类型 | 长度(字节) | 说明 |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  固定0x1a  |
| status | uint8 | 1 | 状态码，参见状态码说明 |
| chunk | uint16 | 2 |  0 | 

