<p align="center">
<img src="img/VERILOGO.png" width="300">
</p>

To run AES0 simulation:
```bash
$ verilator -cc aes_192_sed.v -f input.vc --Mdir build -o simu --exe simulation.cpp
$ make -C build/ -f Vaes_192_sed.mk simu
$ ./build/simu
```

To run the full attack:
```bash
$ make
$ ./attack.sh
```
