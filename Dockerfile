FROM ubuntu

WORKDIR /code


# Summa Dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y software-properties-common \
    libnetcdf-dev \
    libnetcdff-dev \
    liblapack-dev

# OpenWQ Dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt install -y \
        libopenblas-dev \
        libarpack2-dev \
        libsuperlu-dev \
        libglvnd-core-dev


RUN apt update -y \
    && apt upgrade -y \
    && DEBIAN_FRONTEND="noninteractive" apt install -y \
         cmake \
         g++ \
         git \
         libssl-dev \
         make \
         gfortran \
         gdb \
    && apt-get autoclean

RUN apt update -y \
    && DEBIAN_FRONTEND="noninteractive" apt install -y \
    build-essential \
    mesa-common-dev \
    mesa-utils \
    freeglut3-dev \
    ninja-build 

ADD . /code

ENV VTK_DIR=/code/lib/vtk/build/

RUN cp /code/lib/armadillo/armadillo-10.7.3/libarmadillo.so /usr/lib/

WORKDIR /code/lib/armadillo/armadillo-10.7.3/

RUN make install

WORKDIR /code


