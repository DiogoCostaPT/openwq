
FROM ubuntu:20.04
#FROM amd64/ubuntu:20.04
WORKDIR /code
# Update ubuntu's apt list
RUN apt-get -y update && apt-get install -y
# Install g++
RUN apt-get -y install g++
# Install dbg
RUN apt-get -y install gdb gdbserver
# Install cmake
RUN apt-get -y install cmake
######################################
# Armadillo
#####################################
# Install armadillo and its dependencies
RUN apt-get -y install liblapack-dev
RUN apt-get -y install libblas-dev
RUN apt-get -y install libopenblas-dev
RUN apt-get -y install libboost-all-dev
RUN apt-get -y install hdf5-tools
# Get Armadillo (get 10.8.2 version, but this can be changed)
# RUN apt-get -y install libarmadillo-dev # very old version in aptitude (not suitable)
RUN apt-get install wget
RUN wget http://sourceforge.net/projects/arma/files/armadillo-10.3.0.tar.xz
RUN apt install xz-utils
RUN tar -xf armadillo-10.3.0.tar.xz
WORKDIR /code/armadillo-10.3.0
RUN cmake . -D DETECT_HDF5=true -DCMAKE_C_FLAGS="-DH5_USE_110_API"
RUN make install

######################################
# Copy local folders to image
#####################################
# Copy the OpenWQ files to the container
WORKDIR /code
COPY . /code

## Install Dependencies for SUMMA
RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y software-properties-common \
    libnetcdf-dev \
    libnetcdff-dev \
    liblapack-dev \
    gfortran