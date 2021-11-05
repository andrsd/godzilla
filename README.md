godzilla
=====

[![codecov](https://codecov.io/gh/andrsd/godzilla/branch/godzilla/graph/badge.svg?token=7KL45W9Z4G)](https://codecov.io/gh/andrsd/godzilla)

![godzilla facepalm](https://img.memecdn.com/godzilla-facepalm_o_204968.jpg)


Build environment in conda
--------------------------

Create a new environment
```
conda create -n godzilla
```

Setup conda forge
```
TODO
```

Install dependencies
```
conda install python clang gfortran zlib autoconf automake libtool cmake openmp lcov gtest gmock
```

Build MPICH
```
./configure --prefix=$CONDA_PREFIX --enable-shared --enable-sharedlibs=clang --enable-fast=O2 --enable-debufinfo --enable-two-level-namespace CC=clang CXX=clang++ FC=gfortran
make
make install
```

Build PETSc
```
./configure --prefix=$CONDA_PREFIX --shared-libraries --with-debugging=no --with-cxx-dialect=C++11 --with-pic=yes --with-ssl=no --download-hypre=yes --download-fblaslapack=yes --download-metis=yes --download-parmetis=yes --download-superlu_dist=yes --download-scalapack=yes --download-mumps=yes --download-exodusii --download-hdf5 --download-netcdf --download-pnetcdf --with-zlib-dir=$CONDA_PREFIX CC=$CC CXX=$CXX FC=$FC LDFLAGS=$LDFLAGS PETSC_DIR=`pwd`
```

Build libMesh
```
cd <path/to/lib>
mkdir build
cd build
../configure
make
make install
```

Build MOOSE:
```
cd <path/to/moose>/framework
make
```

Set env vars:
```
CC=mpicc
CXX=mpicxx
LDFLAGS="-Wl,-rpath,$CONDA_PREFIX/lib -L$CONDA_PREFIX/lib"
FC=mpif90
PETSC_DIR=$CONDA_PREFIX
LIBMESH_DIR=<path/to/libmesh>/installed
MOOSE_DIR=<path/to/moose>/moose
```

Build
-----

```
cd <path/to/godzilla>
mkdir build
cmake ..
make
```

Documentation
-------------

For documentation, you will also need
```
conda install doxygen doxyrest sphinx
pip install sphinx-rtd-theme
```

Environement
```
export DOXYREST_PATH=/path/to/doxyrest
```
