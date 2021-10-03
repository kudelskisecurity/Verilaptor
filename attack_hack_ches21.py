#!/usr/bin/env python3
import phoenixAES
from binascii import hexlify, unhexlify
import sys

# Redirect output.
sys.stdout = sys.stderr

print("[+] DFA on 13th round\n")
subkey13 = phoenixAES.crack_file("tracefile_r11", verbose=0)

print("[+] DFA on 12th round\n")
subkey12 = phoenixAES.crack_file("tracefile_r10", lastroundkeys=[unhexlify(subkey13)], verbose=0)

print()
# Restore stdout
sys.stdout = sys.__stdout__
print(subkey12 + subkey13[0:16])
