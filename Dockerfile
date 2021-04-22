FROM --platform=linux/arm/v7 balenalib/raspberrypi3-debian-python:buster as builder

ADD . /src
WORKDIR /src

RUN [ "cross-build-start" ]

# Get binary name based on projects .pro file
RUN export BINARY_NAME=$(ls src/ | grep -e '.pro$' | xargs -n 1 basename | cut -d '.' -f 1); echo $BINARY_NAME

# Install base dependencies
RUN install_packages \
    apt-utils \
    build-essential

# Install build dependencies
RUN apt-get update
RUN apt-get build-dep .

# Build
ENV QT_SELECT=5
RUN qmake src/*.pro
RUN make -j$(nproc)
RUN strip $BINARY_NAME

RUN [ "cross-build-end" ]
