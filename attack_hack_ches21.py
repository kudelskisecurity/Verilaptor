#!/usr/bin/env python3
import phoenixAES
from binascii import hexlify, unhexlify
import sys

# Redirect output.
sys.stdout = sys.stderr

print("[+] DFA on 13th round\n")
subkey13 = phoenixAES.crack_file("tracefile_r11", verbose=0)

# 10th round fault c
# onversion
print("[+] DFA on 12th round\n")
lastroundkey = unhexlify(subkey13)
with open("tracefile_r10", "r") as faulted_file:
    faulted = faulted_file.read().strip().split("\n")

# Rewind one round with round key found
with open("tracefile_r10_convert", "w") as faulted_file:
    for i in faulted:
        final_state = unhexlify(i)
        previous_state = bytes(phoenixAES.rewind(final_state,lastroundkeys=[lastroundkey], encrypt=True, mimiclastround=True))
        faulted_file.write(hexlify(previous_state).decode() + "\n")

subkey12 = phoenixAES.crack_file("tracefile_r10_convert", verbose=0)
subkey12 = unhexlify(subkey12)
print()

# Restore stdout
sys.stdout = sys.__stdout__
print(hexlify(phoenixAES.MC(subkey12) + lastroundkey[0:8]).decode())