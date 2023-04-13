abc\_py
--------
A simple Python interface for logic synthesis framework [abc](https://github.com/berkeley-abc/abc).
You are welcome to cite ` Keren Zhu, Mingjie Liu, Hao Chen, Zheng Zhao and David Z. Pan,"Exploring Logic Optimizations with Reinforcement Learning and Graph Convolutional Network," 2nd ACM/IEEE Workshop on Machine Learning for CAD (MLCAD), Nov. 2020. `



--------
# Install

# abc
[abc](https://github.com/berkeley-abc/abc)

abc\_py requires a static library for abc, and to make it shareable. `-fPIC` flag needs to be added to gcc or Clang.

Then compile the abc
```
 make ABC_USE_NO_READLINE=1 ABC_USE_PIC=1 libabc.a
```

Note that ABC is defining several macros at build. Please check their Makafile and define them too in CMakeLists.txt, otherwise there might be errors when including the headers. 

# pybind11
[pybind11](https://github.com/pybind/pybind11)

Please see the official [document](http://pybind11.readthedocs.org/en/master) for installing the pybind11.

# Makefile Configure

The project is tested with `cmake 3.13.2`. [cmake](https://cmake.org/)
Export `CXX` to the compiler you like. The tested is made on `g++ 6.3.0 and 9.2.0`.

Correctly set the path to abc directory. Either export to system variable `ABC_DIR` or add flag to cmake `cmake -DABC_DIR=<path>`. 
Make sure the libabc.a is inside `${ABC_DIR}`.

Set the path to pybind11. Either export to system variable `PYBIND11_DIR` or add flag to cmake `cmake -DPYBIND11_DIR=<path>`.

The cmake will automatically find the system Python.
To use the other Python, add cmake flags `cmake -DPYTHON_INCLUDE_DIR=<path> -DPYTHON_LIBRARIES=<static libarary>`.
For example, `-DPYTHON_INCLUDE_DIR=<path>/include/python3.7m -DPYTHON_LIBRARIES=<path>/lib/libpython3.7m.a`

It's also important to find the correct platform configuration for ABC (`error: #error unknown platform`). The ABC uses different data types for different data configuration.
In the ABC software, there is a script `arch_flags` to identify the data types of your platform. It will gives the configuration, for example, `-DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4`, and those are the compile flags need to be included. They need be added to `CMAKE_CXX_FLAGS`. Please edit the `CMakeLists.txt`, and add the flags. An common example is `set(CMAKE_CXX_FLAGS "-DLIN64 -DSIZEOF_VOID_P=8 -DSIZEOF_LONG=8 -DSIZEOF_INT=4 -std=c++14 -Wall -march=native -fopenmp -fPIC") `

# Build
```
mkdir build
cd build
cmake ..
make
cd ../../
pip install abc_py/
```
--------
# Usage

`import abc_py` like the standard Python library.

--------
# Acknolwedgement

I want to thank  binderwang and Zehua Pei for investigating some building issues of this software.

--------
# Contact.
Keren Zhu, The University of Texas at Austin
[UTDA](https://www.cerc.utexas.edu/utda/)

Please let me know if there is any issue or suggestions.
