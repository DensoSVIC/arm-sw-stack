# Copyright (c) 2023 Arm Limited or its affiliates. All rights reserved.
#
# SPDX-License-Identifier: MIT
ARG DOCKERHUB_MIRROR=

FROM ${DOCKERHUB_MIRROR}ubuntu:focal

RUN apt-get update && \
    apt-get install -y locales && \
    localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8
ENV LANG=en_US.utf8
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get install --no-install-recommends -y \
    ca-certificates \
    curl \
    git \
    gnupg2 \
    openssh-client \
    python3 \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    software-properties-common \
    wget \
   && apt-key adv --keyserver keyserver.ubuntu.com \
        --recv-keys 9A2D61F6BB03CED7522B8E7D6657DBE0CC86BB64 \
   && add-apt-repository ppa:rmescandon/yq \
   && apt-get update \
   && apt-get install yq -y \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN python3 -m pip install \
    j2cli \
    j2cli[yaml] \
    pyyaml
