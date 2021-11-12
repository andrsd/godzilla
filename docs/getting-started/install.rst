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

      Build PETSc

      .. code-block::

         ./configure --prefix=$CONDA_PREFIX \
            --shared-libraries \
            --with-debugging=no \
            --with-cxx-dialect=C++11 \
            --with-pic=yes \
            --with-ssl=no \
            --download-mpich=yes \
            --download-hypre=yes \
            --download-fblaslapack=yes \
            --download-metis=yes \
            --download-parmetis=yes \
            --download-superlu_dist=yes \
            --download-scalapack=yes \
            --download-mumps=yes \
            --download-exodusii \
            --download-hdf5 \
            --download-netcdf \
            --download-pnetcdf \
            --with-zlib-dir=$CONDA_PREFIX
            PETSC_DIR=`pwd`


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

         conda install doxygen doxyrest sphinx
         pip install sphinx-rtd-theme sphinx-tabs

      Environment

      .. code-block::

         export DOXYREST_PATH=$CONDA_PREFIX/share/doxyrest

      Build the documentation:

      .. code-block::

         make doc

   .. group-tab:: Linux

      Instructions for linux

   .. group-tab:: MacOS X

      Instrcutions for MacOS X

   .. group-tab:: Windows

      Not supported
