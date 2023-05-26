# compiler 2023
group work for compiler 2023 spring

# how to use

cd into the dir `src`
run command `make` to get a execuatble compiler cmmc (C Minus Mnius Compiler) 
It is a IR genaretor
then use given scripts in the dir `scripts` such as cmmc-llc-as-ld.sh to build a executable
```bash
bash ../scripts/cmmc-llc-as-ld.sh ../test/1.cmm b.out
```
in the above example `../test/1.cmm` is the filenameof the input "c--" code file, and `b.out` is the out put

then you can use this command to test
```bash
../test/quicksort-linux-amd64 ./b.out
```

1.cmm for quick sort
2.cmm for mat multiply