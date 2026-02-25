#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )


cd $SCRIPT_DIR
if [ ! -d Micro-XRCE-DDS-Client ]; then
   git clone --single-branch -b v3.0.0 https://github.com/eProsima/Micro-XRCE-DDS-Client.git
else
   echo "Micro-XRCE-DDS-Client already exists, skipping checkout!"
fi