# Amiibolink传输协议

## 蓝牙参数

目前协议仍然采用了Nordic NUS服务作为蓝牙串口协议。

广播设备名：amiibolink

服务列表：

1. Nordic UART Service

NUS_SERVICE_UUID: 6e400001-b5a3-f393-e0a9-e50e24dcca9e

提供了两个特性：

* NUS_CHAR_TX_UUID: 6e400002-b5a3-f393-e0a9-e50e24dcca9e => 提供数据的发送，支持写入和无返回的写入。
* NUS_CHAR_RX_UUID: 6e400003-b5a3-f393-e0a9-e50e24dcca9e => 提供数据的读取，仅支持通知。

## 传输amiibo协议

## 协议说明


| 功能      | 发送           | 响应    | 说明                        |
|---------|--------------|-------|---------------------------|
| 设置设备模式  | a1 b1 01 | b1 a1 | 01: 随机模式 02: 按序模式 03:读写模式 |
| 写入卡片准备  | a0 b0        | b0 a0 | 在发送数据前发送                  |
| ?? | ac ac 00 04 00 00 02 1c | ca ca | 发送540个字节数据                |
| ?? | ab ab 02 1c | ba ba | 不知道为啥发了相同的含义的包               |
| 发送数据 | dd aa 00 96 ... 00 01 | aa dd | 0x96数据长度，后面跟数据，最后两个字节为包序号   |
| ?? | bc bc | cb cb | 结束传输               |
| ?? | cc dd | dd cc | 也不知道发这个包是干啥的             |



## 交互流程

### 官方小程序交互流程

```
c->s: a0 b0
s->c: b0 a0
c->s: ac ac 00 04 00 00 02 1c //540
s->c: ca ca
c->s: ab ab 02 1c
s->c: ba ba
c->s:

0000   dd aa 00 96 (0x96字节数据) 00 00               

dd aa 00 
96 //150  
04 51 91 4c e2 ..//150 bytes 
00 00 //seq, start from 0


c->s: aadd
s->c:
0000   dd aa 00 96 (0x96字节数据) 00 01              
c->s: aadd
s->c:
0000   dd aa 00 96 (0x96字节数据) 00 02            
c->s: aadd
s->c:  dd aa 00 5a (0x5a字节数据) 00 03 
s->c: aadd
c->s: bcbc
s->c: cbcb
s->c: ccdd
c->s: ddcc
```

### iNFC交互流程 
流程基本和上面一样，不过多了一个命令
```
c->s: a1 b1 01
s->c: b1 a1
c->s: a0b0
s->c: b0a0
...
```

