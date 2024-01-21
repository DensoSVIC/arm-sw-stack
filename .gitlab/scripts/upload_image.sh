#!/bin/bash
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

KASFILES=$(./.gitlab/scripts/jobs-to-kas "${KAS_CONFIGS}")
export FVP_PV
FVP_PV=$(kas shell --update --force-checkout "${KASFILES}" -c \
"bitbake-getvar --value -r fvp-rd-kronos-native PV")

if [[ -d "${DEPLOY_DIR}" ]]; then
  TARFILE="${CI_PROJECT_DIR}/${IMAGE_TARFILE_NAME}-${CI_PIPELINE_ID}.tgz"
  tar -zcvf "${TARFILE}" -C "${DEPLOY_DIR}/images" fvp-rd-kronos
  cat << EOF > ./upload_spec.json
{
  "files": [
    {
      "pattern": "${TARFILE}",
      "target": "${ARTIFACTORY_ARTIFACT_PATH}/"
    }
  ]
}
EOF
  jf rt upload --spec=./upload_spec.json \
    --build-name="${ARTIFACTORY_IMAGE_BUILD_PATH}" \
    --build-number="${CI_PIPELINE_ID}"
fi

# Collect environment variables and attach them to the build
export RECIPEINFO
RECIPEINFO="${DEPLOY_DIR}/licenses/fvp-rd-kronos-native/recipeinfo"
jf rt build-collect-env \
  "${ARTIFACTORY_IMAGE_BUILD_PATH}" "${CI_PIPELINE_ID}"

# Publish build to Artifactory
jf rt build-publish --build-url "${CI_PIPELINE_URL}" \
  --url "${ARTIFACTORY_BASE_URL}" --password "${ARTIFACTORY_KEY}" \
  "${ARTIFACTORY_IMAGE_BUILD_PATH}" "${CI_PIPELINE_ID}"
