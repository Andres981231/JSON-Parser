FROM ubuntu:16.04

WORKDIR /opt

ENV PROJ_DIR=/opt/toy_json
COPY include ${PROJ_DIR}/include
COPY src ${PROJ_DIR}/src
COPY test ${PROJ_DIR}/test
COPY cmake ${PROJ_DIR}/cmake
COPY CMakeLists.txt ${PROJ_DIR}
COPY docker/apt_source.txt /etc/apt/sources.list

RUN apt-get update && \
    apt-get install --yes \
        build-essential \
        git \
        cmake \
        vim && \
    rm -rf /var/lib/apt/lists/*
