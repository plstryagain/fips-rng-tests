# FIPS 140-2 statistical random number generator tests  
  
This is implementation of the FIPS 140-2 statistical random number generator tests in C.  
  
## Build  
  
```sh
mkdir build
cd ./build
cmake ..
make
```
  
To build test programm you need to build OpenSSL first.  
  
```sh
# Build OpenSSL
export OPENSSL_ROOT_DIR=/path/to/openssl
mkdir build
cd ./build
cmake -DBUILD_TEST=1 ..
make
```