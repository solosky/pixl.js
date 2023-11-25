#Amiibolink transfer protocol

## Bluetooth parameters

The current protocol still uses the Nordic NUS service as the Bluetooth serial port protocol.

Broadcasting device name: amiibolink

Service list:

1. Nordic UART Service

NUS_SERVICE_UUID: 6e400001-b5a3-f393-e0a9-e50e24dcca9e

Two features are provided:

* NUS_CHAR_TX_UUID: 6e400002-b5a3-f393-e0a9-e50e24dcca9e => Provides data transmission, supports writing and writing without return.
* NUS_CHAR_RX_UUID: 6e400003-b5a3-f393-e0a9-e50e24dcca9e => Provides data reading and only supports notification.

##Transfer amiibo protocol

## Protocol Description


| Function | Send | Response | Description |
|---------|--------------|------|---------------- ----------|
| Set device mode | a1 b1 01 | b1 a1 | 01: Random mode 02: Sequential mode 03: Read and write mode |
| Write card preparation | a0 b0 | b0 a0 | Send before sending data |
| ?? | ac ac 00 04 00 00 02 1c | ca ca | Send 540 bytes of data |
| ?? | ab ab 02 1c | ba ba | I don’t know why a package with the same meaning was sent |
| Send data | dd aa 00 96 ... 00 01 | aa dd | 0x96 data length, followed by data, the last two bytes are the packet sequence number |
| ?? | bc bc | cb cb | end transfer |
| ?? | cc dd | dd cc | I don’t know why I am sending this package |



## Interaction process

### Official mini program interaction process

```
c->s: a0 b0
s->c: b0 a0
c->s: ac ac 00 04 00 00 02 1c //540
s->c: ca ca
c->s: ab ab 02 1c
s->c: ba ba
c->s:

0000 dd aa 00 96 (0x96 bytes of data) 00 00

dd aa 00
96 //150
04 51 91 4c e2 ..//150 bytes
00 00 //seq, start from 0


c->s: aadd
s->c:
0000 dd aa 00 96 (0x96 bytes of data) 00 01
c->s: aadd
s->c:
0000 dd aa 00 96 (0x96 bytes of data) 00 02
c->s: aadd
s->c: dd aa 00 5a (0x5a byte data) 00 03
s->c: aadd
c->s: bcbc
s->c: cbcb
s->c:ccdd
c->s: ddcc
```

### iNFC interaction process
The process is basically the same as above, but there is one more command
```
c->s: a1 b1 01
s->c: b1 a1
c->s: a0b0
s->c: b0a0
...
```