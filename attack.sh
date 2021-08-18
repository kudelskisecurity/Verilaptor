#!/bin/bash

# Compute faults from simulation
./simu/build/veriraptor

# DFA on AES 192
ROUNDKEY=$(./attack_hack_ches21.py)

echo Concatenated round keys:
echo $ROUNDKEY
echo 

# Result is the 1.5 last round key, still need to invert AES keyschedule with
echo "[+] Inverse key schedule"
./Stark/aes_keyschedule $ROUNDKEY 11