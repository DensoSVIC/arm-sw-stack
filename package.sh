#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

rm -rf ${SCRIPT_DIR}/package
mkdir ${SCRIPT_DIR}/package

cd ${SCRIPT_DIR}/package
wget -O nosys-baremetal-1.1.1.zip https://firmwares-us-east-1-avh-s3-arm-com.s3.amazonaws.com/nosys-baremetal-hipc-1.1.1.coreimg-13a26521-98ae-4332-8360-ebce2c903978 
unzip nosys-baremetal-1.1.1.zip
rm nosys-baremetal-1.1.1.zip firmware ap_flash boot_flash virtio_0 sdcard lcm_otp

mkdir firmware
cat << EOF > "firmware/load.txt"
name:rse-rom-image.img                          load:0x11000000
name:encrypted_cm_provisioning_bundle_0.bin     load:0x31000000
name:encrypted_dm_provisioning_bundle.bin       load:0x31080000
EOF
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/encrypted_cm_provisioning_bundle_0.bin firmware/
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/encrypted_dm_provisioning_bundle.bin firmware/
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/rse-rom-image.img firmware/
cd ${SCRIPT_DIR}/package/firmware/
zip ${SCRIPT_DIR}/package/firmware.zip encrypted_cm_provisioning_bundle_0.bin  encrypted_dm_provisioning_bundle.bin  load.txt  rse-rom-image.img
rm -rf ${SCRIPT_DIR}/package/firmware/

cd ${SCRIPT_DIR}/package
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/rse-flash-image.img boot_flash
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/ap-flash-image.img ap_flash
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/efi-capsule-update-disk-image-fvp-rd-kronos.img sdcard
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/rse-nvm-image.img lcm_otp
cp ${SCRIPT_DIR}/build/tmp_baremetal/deploy/images/fvp-rd-kronos/baremetal-image-fvp-rd-kronos.wic virtio_0
mv ${SCRIPT_DIR}/package/firmware.zip ${SCRIPT_DIR}/package/firmware

rm Info.json
cat << EOF > "Info.json"
{
    "Build": "EBS (Baremetal)",
    "Version": "1.1.1",
    "Type": "iot",
    "DeviceIdentifier": "kronos-nosys",
    "UniqueIdentifier": "EBS (Baremetal)"
}
EOF

zip ${SCRIPT_DIR}/ebs-zephyr-1.1.1.zip *
rm -rf ${SCRIPT_DIR}/package