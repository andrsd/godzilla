godzilla
=====

[![build](https://github.com/andrsd/godzilla/actions/workflows/build.yml/badge.svg?branch=godzilla&event=push)](https://github.com/andrsd/godzilla/actions/workflows/build.yml)
[![Documentation Status](https://readthedocs.org/projects/godzilla-fem/badge/?version=latest)](https://godzilla-fem.readthedocs.io/en/latest/?badge=latest)
[![codecov](https://codecov.io/gh/andrsd/godzilla/branch/godzilla/graph/badge.svg?token=7KL45W9Z4G)](https://codecov.io/gh/andrsd/godzilla)
[![License](http://img.shields.io/:license-mit-blue.svg)](https://andrsd.mit-license.org/)
[![PETSc](https://img.shields.io/badge/PETSc-3.16-red)](https://petsc.org/)


![godzilla facepalm](https://img.memecdn.com/godzilla-facepalm_o_204968.jpg)


Build environment in conda
--------------------------

Create a new environment
```
conda create -n godzilla
```

Setup conda forge
```
conda config --add channels conda-forge
conda config --set channel_priority strict
```

Install dependencies
```
conda install python clang gfortran zlib autoconf automake libtool cmake openmp lcov
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

Set env vars:
```
CC=mpicc
CXX=mpicxx
LDFLAGS="-Wl,-rpath,$CONDA_PREFIX/lib -L$CONDA_PREFIX/lib"
FC=mpif90
PETSC_DIR=$CONDA_PREFIX
```

Build
-----

```
cd <path/to/godzilla>
mkdir build
cd build
cmake ..
make
```

Documentation
-------------

For documentation, you will also need
```
conda install doxygen doxyrest sphinx
pip install sphinx-rtd-theme sphinx-tabs
```

Environement
```
export DOXYREST_PATH=$CONDA_PREFIX/share/doxyrest
```
