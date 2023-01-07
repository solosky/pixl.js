FROM ubuntu:22.04

RUN mkdir /builds && \
    apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y make cmake zip curl wget git doxygen graphviz python3 python3-pip libusb-1.0-0 && \
    apt-get autoclean -y && \
    apt-get autoremove -y && \
    apt-get clean

RUN wget -O archive.tar.bz2 "https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz?rev=7bd049b7a3034e64885fa1a71c12f91d&hash=732D909FA8F68C0E1D0D17D08E057619" && \
    tar -vxf archive.tar.bz2 -C /opt && \
    rm archive.tar.bz2 

ENV PATH=/opt/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi/bin:$PATH

WORKDIR /builds
ENV LC_ALL=C.UTF-8 LANG=C.UTF-8

RUN wget "https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/desktop-software/nrf-command-line-tools/sw/versions-10-x-x/10-18-1/nrf-command-line-tools_10.18.1_amd64.deb" -O nrf-command-line-tools_10.18.1_amd64.deb && \
    dpkg -i nrf-command-line-tools_10.18.1_amd64.deb && \
    rm nrf-command-line-tools_10.18.1_amd64.deb  


RUN wget "https://github.com/NordicSemiconductor/pc-nrfutil/releases/download/v6.1.7/nrfutil-linux" -O /usr/bin/nrfutil && \
    chmod +x /usr/bin/nrfutil


RUN wget "https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip" -O nrf5_sdk_17.1.0_ddde560.zip && \
    unzip -d /opt nrf5_sdk_17.1.0_ddde560.zip && \
    rm nrf5_sdk_17.1.0_ddde560.zip 

ENV NRF52_SDK_ROOT=/opt/nRF5_SDK_17.1.0_ddde560
ENV GNU_INSTALL_ROOT=/opt/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi/bin/

RUN cd /opt/nRF5_SDK_17.1.0_ddde560/external/micro-ecc && \
    sed 's/\r//g' build_all.sh > build_all2.sh && \
    sh build_all2.sh