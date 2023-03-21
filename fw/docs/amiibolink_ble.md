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

```
c->s: a0 b0
s->c: b0 a0
c->s: ac ac 00 04 00 00 02 1c //540
s->c: ca ca
c->s: ab ab 02 1c
s->c: ba ba
c->s:

0000   dd aa 00 96 04 51 91 4c e2 6f 40 81 4c 48 0f e0   .....Q.L.o@.LH..
0010   f1 10 ff ee a5 ee a7 00 44 cf 33 99 5f 29 20 0b   ........D.3._) .
0020   bc b7 bf ff f7 26 d4 d0 27 d0 e1 7d 4f 5d f7 b1   .....&..'..}O]..
0030   ef ff fa 54 ca 65 51 55 7b 86 41 ce 80 f4 cb 27   ...T.eQU{.A....'
0040   e4 6f eb af 98 16 3a 15 11 be b7 63 a6 20 32 5f   .o....:....c. 2_
0050   fc bc 7d ba bc d5 31 ec 00 05 00 00 00 39 01 02   ..}...1......9..
0060   05 12 15 07 f5 94 5b c5 ba 81 39 75 d0 ca ac 08   ......[...9u....
0070   16 bd e4 50 91 c4 0f 49 e9 f3 9e 9a 7d 9b 7c 2c   ...P...I....}.|,
0080   7f cf c4 c6 b6 a4 9b 27 fe 08 b8 4d 53 a0 d8 0a   .......'...MS...
0090   b6 d5 57 3f a5 b9 0b 3d 41 4c 00 00               ..W?...=AL..

dd aa 00 
96 //150  
04 51 91 4c e2 ..//150 bytes 
00 00 //seq, start from 0


c->s: aadd
s->c:
0000   dd aa 00 96 13 b1 da eb ff 16 d8 7d b3 b0 05 a1   ...........}....
0010   d4 6f a2 39 ac bf a9 1b 18 4a ae 3e 55 b6 20 39   .o.9.....J.>U. 9
0020   65 59 1c 10 50 3c 7f 76 ec 16 ae 8a 93 0a 57 f6   eY..P<.v......W.
0030   e5 0e 55 07 ea 88 96 b8 3a 95 b9 7d 09 69 2e db   ..U.....:..}.i..
0040   13 d2 09 b6 0e 64 6f aa aa 8e be 40 7a 20 55 ca   .....do....@z U.
0050   65 5f 16 ad fa 2d b4 e9 5d 7c fb c9 57 7e b6 57   e_...-..]|..W~.W
0060   a2 80 9a 9b 35 8c 99 4d be 03 c5 9e ee c0 69 42   ....5..M......iB
0070   36 d0 ca 80 55 9f d4 6d 24 ff aa 93 97 51 af 75   6...U..m$....Q.u
0080   f8 c5 aa ad 65 9a 71 78 1c 88 21 d6 e8 c9 49 bf   ....e.qx..!...I.
0090   58 c8 47 20 be 74 52 46 0a 02 00 01               X.G .tRF....
c->s: aadd
s->c:
0000   dd aa 00 96 57 08 a5 c6 79 50 03 75 b1 fd 35 62   ....W...yP.u..5b
0010   f4 40 1f 88 59 f8 ad 40 29 c9 08 21 ce bd b0 6e   .@..Y..@)..!...n
0020   0e b3 14 8e 41 e4 fc b9 e9 8d d2 ee 16 99 b7 02   ....A...........
0030   37 97 4d ea 05 16 a9 82 a9 f6 63 9f 4a 27 cb 80   7.M.......c.J'..
0040   35 ca 1c 82 c5 9f 3d 56 6d 1c d0 3d ba ff 71 41   5.....=Vm..=..qA
0050   6a b8 83 5f 48 4b 31 82 d1 47 77 03 40 a9 bd 8b   j.._HK1..Gw.@...
0060   54 71 bb d7 86 2b a6 da 29 79 fa d7 29 2c 8b 31   Tq...+..)y..),.1
0070   24 b3 ee ef 9d 29 6c ce 53 4d 0f 3e ac b1 63 35   $....)l.SM.>..c5
0080   93 75 2a 84 cd a5 f9 ac 9d 6e 6d ce 27 71 77 6d   .u*......nm.'qwm
0090   62 cd 5b 7d 4e 2d 9e da 36 c1 00 02               b.[}N-..6...
c->s: aadd
s->c:  dd aa 00 5a 04 16 a3 a3 bc 18 ac f2 fe 93 67 ca   ...Z..........g.
0010   1d 7c b6 05 84 8f 5d 43 8e dc 41 7f 7b dd 35 9d   .|....]C..A.{.5.
0020   b4 99 9c 88 f9 56 32 9a 09 06 43 ad 4a 70 c8 d6   .....V2...C.Jp..
0030   f7 91 3a 9a d8 be e1 54 77 bb b3 ae a8 12 9e 7f   ..:....Tw.......
0040   1b d7 d2 78 cc f6 b9 23 90 5a 01 00 0f bd 00 00   ...x...#.Z......
0050   00 04 5f 00 00 00 00 00 00 00 00 00 00 00 00 03   .._.............
s->c: aadd
c->s: bcbc
s->c: cbcb
s->c: ccdd
c->s: ddcc
```