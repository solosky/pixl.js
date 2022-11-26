## 将打印的内容写入src/amiibo_private.c文件
filepath = "D:/repositories/bins/key_retail.bin"

tmp = """
#include <stdint.h>

const uint8_t amiibo_key_retail[] = {"""
f = open(filepath, 'rb')
data= []
while True:
    c = f.read(1)
    if not c:
        break
    data.append(hex(ord(c)) )
tmp = tmp + ','.join(data)
tmp = tmp + '};'
with open('src/amiibo_private.c','w') as f:
    f.write(tmp)
