#
# Based on: https://github.com/PelionIoT/meta-edge/blob/5b8705c23cfc2ddfa0a7596efdae3546bd49d647/recipes-edge/parsec-se-driver/parsec-se-driver.inc
# In open-source project: meta-edge
# Original file: SPDX-FileCopyrightText: <text>Copyright (c) 2023
# Izuma and affiliates</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Add checksum SRC_URI for every crate sources
#

LIC_FILES_CHKSUM = "\
    file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57 \
"

# /* cSpell:disable */
SRC_URI:append = " \
    crate://crates.io/aho-corasick/0.7.18 \
    crate://crates.io/ansi_term/0.11.0 \
    crate://crates.io/anyhow/1.0.44 \
    crate://crates.io/atty/0.2.14 \
    crate://crates.io/autocfg/1.0.1 \
    crate://crates.io/bincode/1.3.3 \
    crate://crates.io/bindgen/0.56.0 \
    crate://crates.io/bitflags/1.3.2 \
    crate://crates.io/bytes/0.5.6 \
    crate://crates.io/cc/1.0.71 \
    crate://crates.io/cexpr/0.4.0 \
    crate://crates.io/cfg-if/1.0.0 \
    crate://crates.io/clang-sys/1.2.2 \
    crate://crates.io/clap/2.33.3 \
    crate://crates.io/cmake/0.1.46 \
    crate://crates.io/derivative/2.2.0 \
    crate://crates.io/either/1.6.1 \
    crate://crates.io/env_logger/0.7.1 \
    crate://crates.io/env_logger/0.8.4 \
    crate://crates.io/form_urlencoded/1.0.1 \
    crate://crates.io/glob/0.3.0 \
    crate://crates.io/hermit-abi/0.1.19 \
    crate://crates.io/humantime/1.3.0 \
    crate://crates.io/humantime/2.1.0 \
    crate://crates.io/idna/0.2.3 \
    crate://crates.io/itertools/0.8.2 \
    crate://crates.io/lazy_static/1.4.0 \
    crate://crates.io/lazycell/1.3.0 \
    crate://crates.io/libc/0.2.104 \
    crate://crates.io/libloading/0.7.1 \
    crate://crates.io/log/0.4.14 \
    crate://crates.io/matches/0.1.9 \
    crate://crates.io/memchr/2.4.1 \
    crate://crates.io/nom/5.1.2 \
    crate://crates.io/num-bigint/0.3.3 \
    crate://crates.io/num-complex/0.3.1 \
    crate://crates.io/num-derive/0.3.3 \
    crate://crates.io/num-integer/0.1.44 \
    crate://crates.io/num-iter/0.1.42 \
    crate://crates.io/num-rational/0.3.2 \
    crate://crates.io/num-traits/0.2.14 \
    crate://crates.io/num/0.3.1 \
    crate://crates.io/parsec-client/0.13.0 \
    crate://crates.io/parsec-interface/0.25.0 \
    crate://crates.io/peeking_take_while/0.1.2 \
    crate://crates.io/percent-encoding/2.1.0 \
    crate://crates.io/proc-macro2/1.0.30 \
    crate://crates.io/prost-derive/0.6.1 \
    crate://crates.io/prost/0.6.1 \
    crate://crates.io/psa-crypto-sys/0.9.0 \
    crate://crates.io/psa-crypto/0.9.0 \
    crate://crates.io/quick-error/1.2.3 \
    crate://crates.io/quote/1.0.10 \
    crate://crates.io/regex-syntax/0.6.25 \
    crate://crates.io/regex/1.5.4 \
    crate://crates.io/rustc-hash/1.1.0 \
    crate://crates.io/same-file/1.0.6 \
    crate://crates.io/secrecy/0.7.0 \
    crate://crates.io/serde/1.0.130 \
    crate://crates.io/serde_derive/1.0.130 \
    crate://crates.io/shlex/0.1.1 \
    crate://crates.io/strsim/0.8.0 \
    crate://crates.io/syn/1.0.80 \
    crate://crates.io/synstructure/0.12.6 \
    crate://crates.io/termcolor/1.1.2 \
    crate://crates.io/textwrap/0.11.0 \
    crate://crates.io/tinyvec/1.5.0 \
    crate://crates.io/tinyvec_macros/0.1.0 \
    crate://crates.io/unicode-bidi/0.3.7 \
    crate://crates.io/unicode-normalization/0.1.19 \
    crate://crates.io/unicode-width/0.1.9 \
    crate://crates.io/unicode-xid/0.2.2 \
    crate://crates.io/url/2.2.2 \
    crate://crates.io/users/0.10.0 \
    crate://crates.io/uuid/0.8.2 \
    crate://crates.io/vec_map/0.8.2 \
    crate://crates.io/version_check/0.9.3 \
    crate://crates.io/walkdir/2.3.2 \
    crate://crates.io/which/3.1.1 \
    crate://crates.io/winapi-i686-pc-windows-gnu/0.4.0 \
    crate://crates.io/winapi-util/0.1.5 \
    crate://crates.io/winapi-x86_64-pc-windows-gnu/0.4.0 \
    crate://crates.io/winapi/0.3.9 \
    crate://crates.io/zeroize/1.3.0 \
    crate://crates.io/zeroize_derive/1.2.0 \
"

# /* cSpell:disable */
SRC_URI[aho-corasick-0.7.18.sha256sum] = "1e37cfd5e7657ada45f742d6e99ca5788580b5c529dc78faf11ece6dc702656f"
SRC_URI[ansi_term-0.11.0.sha256sum] = "ee49baf6cb617b853aa8d93bf420db2383fab46d314482ca2803b40d5fde979b"
SRC_URI[anyhow-1.0.44.sha256sum] = "61604a8f862e1d5c3229fdd78f8b02c68dcf73a4c4b05fd636d12240aaa242c1"
SRC_URI[atty-0.2.14.sha256sum] = "d9b39be18770d11421cdb1b9947a45dd3f37e93092cbf377614828a319d5fee8"
# /* cSpell:disable */
SRC_URI[autocfg-1.0.1.sha256sum] = "cdb031dd78e28731d87d56cc8ffef4a8f36ca26c38fe2de700543e627f8a464a"
# /* cSpell:disable */
SRC_URI[bincode-1.3.3.sha256sum] = "b1f45e9417d87227c7a56d22e471c6206462cba514c7590c09aff4cf6d1ddcad"
# /* cSpell:disable */
SRC_URI[bindgen-0.56.0.sha256sum] = "2da379dbebc0b76ef63ca68d8fc6e71c0f13e59432e0987e508c1820e6ab5239"
# /* cSpell:disable */
SRC_URI[bitflags-1.3.2.sha256sum] = "bef38d45163c2f1dde094a7dfd33ccf595c92905c8f8f4fdc18d06fb1037718a"
SRC_URI[bytes-0.5.6.sha256sum] = "0e4cec68f03f32e44924783795810fa50a7035d8c8ebe78580ad7e6c703fba38"
SRC_URI[cc-1.0.71.sha256sum] = "79c2681d6594606957bbb8631c4b90a7fcaaa72cdb714743a437b156d6a7eedd"
# /* cSpell:disable */
SRC_URI[cexpr-0.4.0.sha256sum] = "f4aedb84272dbe89af497cf81375129abda4fc0a9e7c5d317498c15cc30c0d27"
SRC_URI[cfg-if-1.0.0.sha256sum] = "baf1de4339761588bc0619e3cbc0120ee582ebb74b53b4efbf79117bd2da40fd"
SRC_URI[clang-sys-1.2.2.sha256sum] = "10612c0ec0e0a1ff0e97980647cb058a6e7aedb913d01d009c406b8b7d0b26ee"
SRC_URI[clap-2.33.3.sha256sum] = "37e58ac78573c40708d45522f0d80fa2f01cc4f9b4e2bf749807255454312002"
SRC_URI[cmake-0.1.46.sha256sum] = "b7b858541263efe664aead4a5209a4ae5c5d2811167d4ed4ee0944503f8d2089"
SRC_URI[derivative-2.2.0.sha256sum] = "fcc3dd5e9e9c0b295d6e1e4d811fb6f157d5ffd784b8d202fc62eac8035a770b"
SRC_URI[either-1.6.1.sha256sum] = "e78d4f1cc4ae33bbfc157ed5d5a5ef3bc29227303d595861deb238fcec4e9457"
SRC_URI[env_logger-0.7.1.sha256sum] = "44533bbbb3bb3c1fa17d9f2e4e38bbbaf8396ba82193c4cb1b6445d711445d36"
SRC_URI[env_logger-0.8.4.sha256sum] = "a19187fea3ac7e84da7dacf48de0c45d63c6a76f9490dae389aead16c243fce3"
SRC_URI[form_urlencoded-1.0.1.sha256sum] = "5fc25a87fa4fd2094bffb06925852034d90a17f0d1e05197d4956d3555752191"
SRC_URI[glob-0.3.0.sha256sum] = "9b919933a397b79c37e33b77bb2aa3dc8eb6e165ad809e58ff75bc7db2e34574"
SRC_URI[hermit-abi-0.1.19.sha256sum] = "62b467343b94ba476dcb2500d242dadbb39557df889310ac77c5d99100aaac33"
# /* cSpell:disable */
SRC_URI[humantime-1.3.0.sha256sum] = "df004cfca50ef23c36850aaaa59ad52cc70d0e90243c3c7737a4dd32dc7a3c4f"
# /* cSpell:disable */
SRC_URI[humantime-2.1.0.sha256sum] = "9a3a5bfb195931eeb336b2a7b4d761daec841b97f947d34394601737a7bba5e4"
# /* cSpell:disable */
SRC_URI[idna-0.2.3.sha256sum] = "418a0a6fab821475f634efe3ccc45c013f742efe03d853e8d3355d5cb850ecf8"
# /* cSpell:disable */
SRC_URI[itertools-0.8.2.sha256sum] = "f56a2d0bc861f9165be4eb3442afd3c236d8a98afd426f65d92324ae1091a484"
SRC_URI[lazy_static-1.4.0.sha256sum] = "e2abad23fbc42b3700f2f279844dc832adb2b2eb069b2df918f455c4e18cc646"
# /* cSpell:disable */
SRC_URI[lazycell-1.3.0.sha256sum] = "830d08ce1d1d941e6b30645f1a0eb5643013d835ce3779a5fc208261dbe10f55"
SRC_URI[libc-0.2.104.sha256sum] = "7b2f96d100e1cf1929e7719b7edb3b90ab5298072638fccd77be9ce942ecdfce"
# /* cSpell:disable */
SRC_URI[libloading-0.7.1.sha256sum] = "c0cf036d15402bea3c5d4de17b3fce76b3e4a56ebc1f577be0e7a72f7c607cf0"
SRC_URI[log-0.4.14.sha256sum] = "51b9bbe6c47d51fc3e1a9b945965946b4c44142ab8792c50835a980d362c2710"
SRC_URI[matches-0.1.9.sha256sum] = "a3e378b66a060d48947b590737b30a1be76706c8dd7b8ba0f2fe3989c68a853f"
# /* cSpell:disable */
SRC_URI[memchr-2.4.1.sha256sum] = "308cc39be01b73d0d18f82a0e7b2a3df85245f84af96fdddc5d202d27e47b86a"
SRC_URI[nom-5.1.2.sha256sum] = "ffb4262d26ed83a1c0a33a38fe2bb15797329c85770da05e6b828ddb782627af"
SRC_URI[num-bigint-0.3.3.sha256sum] = "5f6f7833f2cbf2360a6cfd58cd41a53aa7a90bd4c202f5b1c7dd2ed73c57b2c3"
SRC_URI[num-complex-0.3.1.sha256sum] = "747d632c0c558b87dbabbe6a82f3b4ae03720d0646ac5b7b4dae89394be5f2c5"
SRC_URI[num-derive-0.3.3.sha256sum] = "876a53fff98e03a936a674b29568b0e605f06b29372c2489ff4de23f1949743d"
SRC_URI[num-integer-0.1.44.sha256sum] = "d2cc698a63b549a70bc047073d2949cce27cd1c7b0a4a862d08a8031bc2801db"
SRC_URI[num-iter-0.1.42.sha256sum] = "b2021c8337a54d21aca0d59a92577a029af9431cb59b909b03252b9c164fad59"
SRC_URI[num-rational-0.3.2.sha256sum] = "12ac428b1cb17fce6f731001d307d351ec70a6d202fc2e60f7d4c5e42d8f4f07"
SRC_URI[num-traits-0.2.14.sha256sum] = "9a64b1ec5cda2586e284722486d802acf1f7dbdc623e2bfc57e65ca1cd099290"
SRC_URI[num-0.3.1.sha256sum] = "8b7a8e9be5e039e2ff869df49155f1c06bd01ade2117ec783e56ab0932b67a8f"
SRC_URI[parsec-client-0.13.0.sha256sum] = "44feb0437d2d62274a2fca2ad421b7decb621bba7c8679720ad20e4353869de5"
SRC_URI[parsec-interface-0.25.0.sha256sum] = "68c10169382873c646754cccc2311f538cc8bfd75b66b375a27717dd17c288d3"
SRC_URI[peeking_take_while-0.1.2.sha256sum] = "19b17cddbe7ec3f8bc800887bab5e717348c95ea2ca0b1bf0837fb964dc67099"
SRC_URI[percent-encoding-2.1.0.sha256sum] = "d4fd5641d01c8f18a23da7b6fe29298ff4b55afcccdf78973b24cf3175fee32e"
SRC_URI[proc-macro2-1.0.30.sha256sum] = "edc3358ebc67bc8b7fa0c007f945b0b18226f78437d61bec735a9eb96b61ee70"
SRC_URI[prost-derive-0.6.1.sha256sum] = "537aa19b95acde10a12fec4301466386f757403de4cd4e5b4fa78fb5ecb18f72"
SRC_URI[prost-0.6.1.sha256sum] = "ce49aefe0a6144a45de32927c77bd2859a5f7677b55f220ae5b744e87389c212"
SRC_URI[psa-crypto-sys-0.9.0.sha256sum] = "441c9db98d9a676a3c8395b67b6fa253c867fb77c948c30977574a6989d4d56b"
SRC_URI[psa-crypto-0.9.0.sha256sum] = "aae242f8167e9c3743b3479cb95d06e6d7621a416bce9b6f55063cd68c773011"
SRC_URI[quick-error-1.2.3.sha256sum] = "a1d01941d82fa2ab50be1e79e6714289dd7cde78eba4c074bc5a4374f650dfe0"
SRC_URI[quote-1.0.10.sha256sum] = "38bc8cc6a5f2e3655e0899c1b848643b2562f853f114bfec7be120678e3ace05"
SRC_URI[regex-syntax-0.6.25.sha256sum] = "f497285884f3fcff424ffc933e56d7cbca511def0c9831a7f9b5f6153e3cc89b"
SRC_URI[regex-1.5.4.sha256sum] = "d07a8629359eb56f1e2fb1652bb04212c072a87ba68546a04065d525673ac461"
SRC_URI[rustc-hash-1.1.0.sha256sum] = "08d43f7aa6b08d49f382cde6a7982047c3426db949b1424bc4b7ec9ae12c6ce2"
SRC_URI[same-file-1.0.6.sha256sum] = "93fc1dc3aaa9bfed95e02e6eadabb4baf7e3078b0bd1b4d7b6b0b68378900502"
SRC_URI[secrecy-0.7.0.sha256sum] = "0673d6a6449f5e7d12a1caf424fd9363e2af3a4953023ed455e3c4beef4597c0"
# /* cSpell:disable */
SRC_URI[serde-1.0.130.sha256sum] = "f12d06de37cf59146fbdecab66aa99f9fe4f78722e3607577a5375d66bd0c913"
# /* cSpell:disable */
SRC_URI[serde_derive-1.0.130.sha256sum] = "d7bc1a1ab1961464eae040d96713baa5a724a8152c1222492465b54322ec508b"
# /* cSpell:disable */
SRC_URI[shlex-0.1.1.sha256sum] = "7fdf1b9db47230893d76faad238fd6097fd6d6a9245cd7a4d90dbd639536bbd2"
# /* cSpell:disable */
SRC_URI[strsim-0.8.0.sha256sum] = "8ea5119cdb4c55b55d432abb513a0429384878c15dde60cc77b1c99de1a95a6a"
SRC_URI[syn-1.0.80.sha256sum] = "d010a1623fbd906d51d650a9916aaefc05ffa0e4053ff7fe601167f3e715d194"
# /* cSpell:disable */
SRC_URI[synstructure-0.12.6.sha256sum] = "f36bdaa60a83aca3921b5259d5400cbf5e90fc51931376a9bd4a0eb79aa7210f"
# /* cSpell:disable */
SRC_URI[termcolor-1.1.2.sha256sum] = "2dfed899f0eb03f32ee8c6a0aabdb8a7949659e3466561fc0adf54e26d88c5f4"
# /* cSpell:disable */
SRC_URI[textwrap-0.11.0.sha256sum] = "d326610f408c7a4eb6f51c37c330e496b08506c9457c9d34287ecc38809fb060"
# /* cSpell:disable */
SRC_URI[tinyvec-1.5.0.sha256sum] = "f83b2a3d4d9091d0abd7eba4dc2710b1718583bd4d8992e2190720ea38f391f7"
# /* cSpell:disable */
SRC_URI[tinyvec_macros-0.1.0.sha256sum] = "cda74da7e1a664f795bb1f8a87ec406fb89a02522cf6e50620d016add6dbbf5c"
# /* cSpell:disable */
SRC_URI[unicode-bidi-0.3.7.sha256sum] = "1a01404663e3db436ed2746d9fefef640d868edae3cceb81c3b8d5732fda678f"
SRC_URI[unicode-normalization-0.1.19.sha256sum] = "d54590932941a9e9266f0832deed84ebe1bf2e4c9e4a3554d393d18f5e854bf9"
SRC_URI[unicode-width-0.1.9.sha256sum] = "3ed742d4ea2bd1176e236172c8429aaf54486e7ac098db29ffe6529e0ce50973"
SRC_URI[unicode-xid-0.2.2.sha256sum] = "8ccb82d61f80a663efe1f787a51b16b5a51e3314d6ac365b08639f52387b33f3"
SRC_URI[url-2.2.2.sha256sum] = "a507c383b2d33b5fc35d1861e77e6b383d158b2da5e14fe51b83dfedf6fd578c"
SRC_URI[users-0.10.0.sha256sum] = "aa4227e95324a443c9fcb06e03d4d85e91aabe9a5a02aa818688b6918b6af486"
SRC_URI[uuid-0.8.2.sha256sum] = "bc5cf98d8186244414c848017f0e2676b3fcb46807f6668a97dfe67359a3c4b7"
SRC_URI[vec_map-0.8.2.sha256sum] = "f1bddf1187be692e79c5ffeab891132dfb0f236ed36a43c7ed39f1165ee20191"
SRC_URI[version_check-0.9.3.sha256sum] = "5fecdca9a5291cc2b8dcf7dc02453fee791a280f3743cb0905f8822ae463b3fe"
# /* cSpell:disable */
SRC_URI[walkdir-2.3.2.sha256sum] = "808cf2735cd4b6866113f648b791c6adc5714537bc222d9347bb203386ffda56"
SRC_URI[which-3.1.1.sha256sum] = "d011071ae14a2f6671d0b74080ae0cd8ebf3a6f8c9589a2cd45f23126fe29724"
SRC_URI[winapi-i686-pc-windows-gnu-0.4.0.sha256sum] = "ac3b87c63620426dd9b991e5ce0329eff545bccbbb34f3be09ff6fb6ab51b7b6"
SRC_URI[winapi-util-0.1.5.sha256sum] = "70ec6ce85bb158151cae5e5c87f95a8e97d2c0c4b001223f33a334e3ce5de178"
SRC_URI[winapi-x86_64-pc-windows-gnu-0.4.0.sha256sum] = "712e227841d057c1ee1cd2fb22fa7e5a5461ae8e48fa2ca79ec42cfc1931183f"
SRC_URI[winapi-0.3.9.sha256sum] = "5c839a674fcd7a98952e593242ea400abe93992746761e38641405d28b00f419"
# /* cSpell:disable */
SRC_URI[zeroize-1.3.0.sha256sum] = "4756f7db3f7b5574938c3eb1c117038b8e07f95ee6718c0efad4ac21508f1efd"
# /* cSpell:disable */
SRC_URI[zeroize_derive-1.2.0.sha256sum] = "bdff2024a851a322b08f179173ae2ba620445aef1e838f0c196820eade4ae0c7"
