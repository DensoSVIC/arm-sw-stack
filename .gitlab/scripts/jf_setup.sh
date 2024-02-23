#!/bin/bash
# SPDX-FileCopyrightText: <text>Copyright 2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Download version 2.51.1 of the jfrog CLI
apt-get -y update; apt-get -y install curl
curl -fL https://install-cli.jfrog.io | bash -s -- 2.51.1

# Upload the deploy dir images
jf config add --interactive=false \
  --artifactory-url="${FVP_SERVER_BASE_URL}" \
  --user="${FVP_SERVER_USER}" --password="${FVP_SERVER_KEY}"
