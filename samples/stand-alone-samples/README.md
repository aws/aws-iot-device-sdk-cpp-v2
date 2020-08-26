# Stand-alone Samples

## building instruction

Firstly, build the project following the instructions from [project README](../../). Then, change directory into stand-alone-samples.

In stand-alone-samples/shadow/shadow_sync/ or stand-alone-samples/basic_pub_sub/:

``` sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="<absolute path sdk-cpp-workspace dir>" ..
cmake --build .
```

The binary executable file will be under build/.
