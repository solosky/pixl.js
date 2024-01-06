# Bluetooth parameters

Currently, NRF52 SDK is used to provide Nordic UART Service, which provides a BLE virtual serial port to communicate with the host.

Device name: Pixl.js

Service list:

1. Nordic UART Service

NUS_SERVICE_UUID: 6e400001-b5a3-f393-e0a9-e50e24dcca9e

Two features are provided:

* NUS_CHAR_TX_UUID: 6e400002-b5a3-f393-e0a9-e50e24dcca9e => Provides data transmission, supports writing and writing without return.
* NUS_CHAR_RX_UUID: 6e400003-b5a3-f393-e0a9-e50e24dcca9e => Provides data reading and only supports notification.

(The official version of the above descriptors may be modified.)

# Protocol format

## Protocol definition


Considering efficiency, the BLE protocol is a binary protocol.
This protocol is a request response protocol, and all requests must be initiated by the client.
To simplify protocol implementation, there will only be one reply packet per request.


### Package structure definition

To simplify the protocol, both requests and replies use the same packet structure.

The general package structure is as follows

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | command type |
| status | uint8 | 1 | Status code, see the status code definition below, the request packet is fixed to 0 |
| chunk | uint16 | 2 | fragment sequence number |
| data | byte | N | Data, related to the specific package definition |


### chunk transfer mode

The current MTU defined by pixl.js device is 250. In an ATT data transmission, up to 247 bytes of data can be transmitted.

Maximum packet size:
The fixed packet header is 4 bytes, so each packet can transmit up to 243 bytes. If the transmitted data exceeds 243 bytes, it needs to be transmitted in blocks.


When the transmitted request or return result exceeds the MTU, it needs to be transmitted in chunks, and the chunk value is increased by 1 for each transmission.
When the highest bit of the chunk is 1, it means that data is still being sent. When it is 0, it means that the data has been read.


Writing and reading files requires enabling chunk transfer.
The chunk field is 2 bytes, and the chunk transport stream can transmit up to 7.8M bytes.
If it exceeds this 7.8M bytes, a new chunk transmission stream needs to be opened.


For example, writing to a file process:

```
c -> s: 0x12, mode: wc, open file
s -> c: 0x12, status 0, file id 0xa1, the file has been created and can be written
c -> s: 0x15, chunk 0 | 0x8000, file id 0xa1, 246 bytes of data
c -> s: 0x15, chunk 1 | 0x8000, file id 0xa1, 246 bytes of data
...
c -> s: 0x15, chunk N, file id 0xa1, 56 bytes of data
s -> c: 0x15, status 0, writing completed
c -> s: 0x13, file id 0xa1, close the file,
s -> c: 0x13, status 0,
```

Read file process

```
c -> s: 0x12, mode: r, open file
s -> c: 0x12, status 0, file id 0xa1, the file is open and can be read
c -> s: 0x14, chunk 0, file id 0xa1
s -> c: 0x14, status 0, chunk 1 | 0x8000, file id 0xa1, 246 bytes of data
...
s -> c: 0x14, status 0, chunk N, file id 0xa1, 56 bytes of data
c -> s: 0x13, file id 0xa1, close the file,
s -> c: 0x13, status 0,
```

### Path definition

File operations require the transfer of the complete file path for files. Standard Windows path expression with drive letter.
For example, E:/amiibo/mifa.bin.

### type 

Numeric types are all unsigned integers, little endian mode.
The string encoding is 2 bytes long + string array, and the strings are uniformly encoded using utf8.

### File meta information
File metainformation is used to store some data related to the file, with a maximum length of 128 bytes. It is distinguished based on the type field. The data is of variable length and the data is parsed according to the type.

The basic format is as follows:

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| meta size | uint8 | 1 | The size of all meta information, 0xFF or 0 without metadata |
| meta N type | uint8 | 1 | Meta information type, defined in the following section |
| meta N data | byte | N | The length varies depending on the type |

The currently defined types can take on the following values:

1: amiibo remarks
| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| note length | uint8 | 1 | note length |
| note data | byte | N | Note string, maximum 90 bytes, UTF8 encoding. |

2: File properties
| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| flag | uint8 | N | Attributes of the file, bitwise defined |

* 0b0000_0001: The file is set as a hidden attribute and will not be displayed on the amiibo page, but can be displayed on the web page. settings.bin will set this flag by default.
* 0bxxxx_xxxx: Other bits are temporarily undefined.

3: amiibo attributes
| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| head | uint32 | N | amiibo id head |
| tail | uint32 | N | amiibo id tail |

Note:
* This attribute is currently only used in amiibo/data/*.bin and is used to quickly identify whether it is an amiibo file.



### Global status code definition

| Status code | Description |
| ---- | ---- |
| 0 | The request has been completed successfully |
| 0xFF | The firmware does not support the current message |

TODO Supplement the error code in detail. .



#Protocol definition

## 0x01: Get version information

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x01 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x01 |
| status | uint8 | 1 | Status code, see status code description |
| chunk | uint16 | 2 | 0 |
| version length | uint16 | 2 | version string length |
| version | byte | N | version |


## 0x02: Enter DFU mode

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x02 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x102 |
| status | uint8 | 1 | Status code, see status code description |
| chunk | uint16 | 2 | 0 |


## 0x10: Get disk list

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x10 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x10 |
| status | uint8 | 1 | Status code, see status code description |
| chunk | uint16 | 2 | 0 |
| drive count| uint8 | 1 | Number of disks |
| drive N status code | uint8 | 1 | Disk N status code 0: The disk is available, non-0 disk is unavailable |
| drive N label |char | 1| disk drive letter |
| drive N name length | uint16 | 2 | disk name length |
| drive N name | byte | N | disk name string |
| drive N total size |uint32 | 4 | Total disk space |
| drive N used size | uint32 | 4 | disk free space |


## 0x11: Format disk

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x11 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |
| drive label | char | 1 | Disk drive letter: E/I |


## 0x12: Open file

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x12 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 |
| path length | uint16 | 2 | path length |
| path | byte | N | path string |
| mode | uint8 | 1 | turn on mode |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | fixed 0x12 |
| status | uint8 | 1 | Status code, see status code description |
| chunk | uint16 | 2 | 0 |
| file id | uint8 | 1 | file id, used for subsequent read and write requests |

## 0x13: Close file

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd | uint8 | 1 | 0x13 |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| file id | uint8 | 1 | file id |


2. Service response

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x13  |
| status | uint8 | 1 | Status code, see status code description |
| chunk | uint16 | 2 |  0 | 
| file id | uint8 | 1 | file id, used for subsequent read and write requests |


## 0x14: Read file

1. The client sends a request

Only support sequential reading.

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x14  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| file id | uint8 | 1| file id |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x14  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  Enable chunk transfer | 
| data  | byte | N | File data |


## 0x15 Write to file

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x15  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | Enable chunk transfer | 
| file id | uint8 | 1 | file id | 
| data | byte | N | File data, max length = MTU - 4 |



2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x15  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  Same as Request | 


## 0x16：Read folder

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x16  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 |  0 | 
| path length | uint16 | 2 | Path length under bytes |
| path  | byte | N | Path string bytes |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x16  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  Enable chunk transfer | 
| file N name length| uint16 | 2 | File name bytes length|
| file N name | byte | N| File name bytes |
| file N size | uint32 | 4 |File size |
| file N type | uint8 | 1 | File Type： 0 => File, 1 => Folder |
| file N meta length | uint8 | 1 | File meta data length, 64 max|
| file N meta | byte | N | File meta data |

## 0x17 Create folder

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x17  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | Path length under bytes |
| path  | byte | N | Path string bytes |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x17  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  0 | 



## 0x18 Delete file or folder

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | Path length under bytes |
| path  | byte | N | Path string bytes |


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  0 | 


## 0x19 Rename file or folder

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | Enable chunk transfer | 
| old path length | uint16 | 2 | Path length under bytes |
| old path  | byte | N | Path string bytes |
| new path length | uint16 | 2 | Path length under bytes |
| new path  | byte | N | Path string bytes |

If data length is larger than MTU, use chunk transfer.


2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x18  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  0 | 


## 0x1A Update file meta

1. The client sends a request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x1a  |
| status | uint8 | 1 | 0 |
| chunk | uint16 | 2 | 0 | 
| path length | uint16 | 2 | Path length under bytes |
| path  | byte | N | Path string bytes |
| meta data | byte | N | File Meta Data | 

2. The server responds to the request

| Field name | Type | Length (bytes) | Description |
| ---- | ----- |---- | ---- |
| cmd  | uint8 | 1  |  0x1a  |
| status | uint8 | 1 |Status code, see status code description |
| chunk | uint16 | 2 |  0 | 

