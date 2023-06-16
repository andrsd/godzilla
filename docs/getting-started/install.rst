Installation
============

.. tabs::

   .. group-tab:: Conda

      Create a new environment

      .. code-block::

         conda create -n godzilla

      Setup conda forge

      .. code-block::

         conda config --add channels conda-forge
         conda config --set channel_priority strict

      Install dependencies

      .. code-block::

         conda install python clang gfortran zlib autoconf automake libtool cmake openmp lcov

      Build MPICH 3.4.2

      .. code-block::

         ./configure --prefix=$CONDA_PREFIX \
            --enable-shared \
            --enable-sharedlibs=clang \
            --enable-fast=O3 \
            --with-pm=hydra \
            --disable-java \
            --with-device=ch3:nemesis \
            --enable-g=meminit \
            --disable-opencl \
            CC=clang CXX=clang++ FC=gfortran

      Build PETSc 3.16.1

      .. code-block::

         ./configure --prefix=$CONDA_PREFIX \
            --shared-libraries \
            --with-debugging=no \
            --with-cxx-dialect=cxx14 \
            --with-pic=yes \
            --with-ssl=no \
            --download-chaco=yes \
            --download-ptscotch=yes \
            --download-ml=yes \
            --download-hypre=yes \
            --download-fblaslapack=yes \
            --download-metis=yes \
            --download-parmetis=yes \
            --download-superlu=yes \
            --download-superlu_dist=yes \
            --download-scalapack=yes \
            --download-mumps=yes \
            --download-exodusii=yes \
            --download-hdf5=yes \
            --download-netcdf=yes \
            --download-pnetcdf=yes \
            --with-zlib-dir=$CONDA_PREFIX \
            CC=mpicc CXX=mpicxx FC=mpif90 \
            PETSC_DIR=`pwd`

      .. note::

         If you installed HDF5 via some other means, do not specify ``--download-hdf5=yes``.
         You can use ``--with-hdf=/path/to/hdf5``, if the library is installed in a non-standard location.

      Build godzilla

      .. code-block::

         cd <path/to/godzilla>
         mkdir build
         cd build
         cmake ..
         make


      **Documentation**

      For documentation, you will also need

      .. code-block::

         conda install doxygen breathe sphinx sphinx_rtd_theme sphinx-tabs

      Build the documentation:

      .. code-block::

         make doc

   .. group-tab:: Linux

      Instructions for linux

   .. group-tab:: MacOS X

      Instructions for MacOS X

   .. group-tab:: Windows

      Not supported
