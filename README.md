# TinySea

TinySea provide functions to compute sailing boat routing based on multiple criteria

<!-- vim-markdown-toc GitLab -->

* [Installation](#installation)
    * [Build](#build)
    * [Create Conan package](#create-conan-package)
* [Algorithm](#algorithm)

<!-- vim-markdown-toc -->

## Installation

### Build

To build this project you must have `conan`, `cmake` and a compiler that support cpp17.

```bash
git clone https://github.com/jorisv/tiny_sea
mkdir build
cd build
conan install ../tiny_sea -s compiler.cppstd=17 -b missing
cmake ../tiny_sea
make && ctest
```

### Create Conan package

```bash
conan create . -s compiler.cppstd=17 -b missing
```

## Algorithm

The algorithm is a Hybrid A\*.

