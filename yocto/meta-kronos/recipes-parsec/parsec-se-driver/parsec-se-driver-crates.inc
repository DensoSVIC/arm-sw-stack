#
# Based on: https://github.com/PelionIoT/meta-edge/blob/5b8705c23cfc2ddfa0a7596efdae3546bd49d647/recipes-edge/parsec-se-driver/parsec-se-driver.inc
# In open-source project: meta-edge
# Original file: SPDX-FileCopyrightText: <text>Copyright (c) 2023
# Izuma and affiliates</text>
# Modifications: SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited
# and/or its affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# Changes:
# 1) Update crates versions.
# 2) Add checksum SRC_URI for every crate sources
#

LIC_FILES_CHKSUM = "\
    file://LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57 \
"

# /* cSpell:disable */
SRC_URI:append = " \
    crate://crates.io/aho-corasick/1.1.2 \
    crate://crates.io/anyhow/1.0.75 \
    crate://crates.io/atty/0.2.14 \
    crate://crates.io/autocfg/1.1.0 \
    crate://crates.io/bincode/1.3.3 \
    crate://crates.io/bindgen/0.66.1 \
    crate://crates.io/bitflags/2.4.1 \
    crate://crates.io/bytes/1.5.0 \
    crate://crates.io/cc/1.0.83 \
    crate://crates.io/cexpr/0.6.0 \
    crate://crates.io/cfg-if/1.0.0 \
    crate://crates.io/clang-sys/1.6.1 \
    crate://crates.io/cmake/0.1.50 \
    crate://crates.io/derivative/2.2.0 \
    crate://crates.io/either/1.9.0 \
    crate://crates.io/env_logger/0.7.1 \
    crate://crates.io/errno/0.3.8 \
    crate://crates.io/form_urlencoded/1.2.1 \
    crate://crates.io/glob/0.3.1 \
    crate://crates.io/hermit-abi/0.1.19 \
    crate://crates.io/home/0.5.5 \
    crate://crates.io/humantime/1.3.0 \
    crate://crates.io/idna/0.5.0 \
    crate://crates.io/itertools/0.10.5 \
    crate://crates.io/lazy_static/1.4.0 \
    crate://crates.io/lazycell/1.3.0 \
    crate://crates.io/libc/0.2.151 \
    crate://crates.io/libloading/0.7.4 \
    crate://crates.io/linux-raw-sys/0.4.12 \
    crate://crates.io/log/0.4.20 \
    crate://crates.io/memchr/2.6.4 \
    crate://crates.io/minimal-lexical/0.2.1 \
    crate://crates.io/nom/7.1.3 \
    crate://crates.io/num/0.4.1 \
    crate://crates.io/num-bigint/0.4.4 \
    crate://crates.io/num-complex/0.4.4 \
    crate://crates.io/num-derive/0.4.1 \
    crate://crates.io/num-integer/0.1.45 \
    crate://crates.io/num-iter/0.1.43 \
    crate://crates.io/num-rational/0.4.1 \
    crate://crates.io/num-traits/0.2.17 \
    crate://crates.io/once_cell/1.19.0 \
    crate://crates.io/parsec-client/0.16.0 \
    crate://crates.io/parsec-interface/0.29.1 \
    crate://crates.io/peeking_take_while/0.1.2 \
    crate://crates.io/percent-encoding/2.3.1 \
    crate://crates.io/prettyplease/0.2.15 \
    crate://crates.io/proc-macro2/1.0.70 \
    crate://crates.io/prost/0.9.0 \
    crate://crates.io/prost-derive/0.9.0 \
    crate://crates.io/psa-crypto/0.12.0 \
    crate://crates.io/psa-crypto-sys/0.12.0 \
    crate://crates.io/quick-error/1.2.3 \
    crate://crates.io/quote/1.0.33 \
    crate://crates.io/regex/1.10.2 \
    crate://crates.io/regex-automata/0.4.3 \
    crate://crates.io/regex-syntax/0.8.2 \
    crate://crates.io/rustc-hash/1.1.0 \
    crate://crates.io/rustix/0.38.28 \
    crate://crates.io/same-file/1.0.6 \
    crate://crates.io/secrecy/0.8.0 \
    crate://crates.io/serde/1.0.193 \
    crate://crates.io/serde_derive/1.0.193 \
    crate://crates.io/shlex/1.2.0 \
    crate://crates.io/syn/1.0.109 \
    crate://crates.io/syn/2.0.41 \
    crate://crates.io/termcolor/1.4.0 \
    crate://crates.io/tinyvec/1.6.0 \
    crate://crates.io/tinyvec_macros/0.1.1 \
    crate://crates.io/unicode-bidi/0.3.14 \
    crate://crates.io/unicode-ident/1.0.12 \
    crate://crates.io/unicode-normalization/0.1.22 \
    crate://crates.io/url/2.5.0 \
    crate://crates.io/uuid/0.8.2 \
    crate://crates.io/walkdir/2.4.0 \
    crate://crates.io/which/4.4.2 \
    crate://crates.io/winapi/0.3.9 \
    crate://crates.io/winapi-i686-pc-windows-gnu/0.4.0 \
    crate://crates.io/winapi-util/0.1.6 \
    crate://crates.io/winapi-x86_64-pc-windows-gnu/0.4.0 \
    crate://crates.io/windows-sys/0.48.0 \
    crate://crates.io/windows-sys/0.52.0 \
    crate://crates.io/windows-targets/0.48.5 \
    crate://crates.io/windows-targets/0.52.0 \
    crate://crates.io/windows_aarch64_gnullvm/0.48.5 \
    crate://crates.io/windows_aarch64_gnullvm/0.52.0 \
    crate://crates.io/windows_aarch64_msvc/0.48.5 \
    crate://crates.io/windows_aarch64_msvc/0.52.0 \
    crate://crates.io/windows_i686_gnu/0.48.5 \
    crate://crates.io/windows_i686_gnu/0.52.0 \
    crate://crates.io/windows_i686_msvc/0.48.5 \
    crate://crates.io/windows_i686_msvc/0.52.0 \
    crate://crates.io/windows_x86_64_gnu/0.48.5 \
    crate://crates.io/windows_x86_64_gnu/0.52.0 \
    crate://crates.io/windows_x86_64_gnullvm/0.48.5 \
    crate://crates.io/windows_x86_64_gnullvm/0.52.0 \
    crate://crates.io/windows_x86_64_msvc/0.48.5 \
    crate://crates.io/windows_x86_64_msvc/0.52.0 \
    crate://crates.io/zeroize/1.7.0 \
    crate://crates.io/zeroize_derive/1.4.2 \
"
# /* cSpell:enable */

# /* cSpell:disable */
SRC_URI[aho-corasick-1.1.2.sha256sum] = "b2969dcb958b36655471fc61f7e416fa76033bdd4bfed0678d8fee1e2d07a1f0"
SRC_URI[anyhow-1.0.75.sha256sum] = "a4668cab20f66d8d020e1fbc0ebe47217433c1b6c8f2040faf858554e394ace6"
SRC_URI[atty-0.2.14.sha256sum] = "d9b39be18770d11421cdb1b9947a45dd3f37e93092cbf377614828a319d5fee8"
SRC_URI[autocfg-1.1.0.sha256sum] = "d468802bab17cbc0cc575e9b053f41e72aa36bfa6b7f55e3529ffa43161b97fa"
SRC_URI[bincode-1.3.3.sha256sum] = "b1f45e9417d87227c7a56d22e471c6206462cba514c7590c09aff4cf6d1ddcad"
SRC_URI[bindgen-0.66.1.sha256sum] = "f2b84e06fc203107bfbad243f4aba2af864eb7db3b1cf46ea0a023b0b433d2a7"
SRC_URI[bitflags-2.4.1.sha256sum] = "327762f6e5a765692301e5bb513e0d9fef63be86bbc14528052b1cd3e6f03e07"
SRC_URI[bytes-1.5.0.sha256sum] = "a2bd12c1caf447e69cd4528f47f94d203fd2582878ecb9e9465484c4148a8223"
SRC_URI[cc-1.0.83.sha256sum] = "f1174fb0b6ec23863f8b971027804a42614e347eafb0a95bf0b12cdae21fc4d0"
SRC_URI[cexpr-0.6.0.sha256sum] = "6fac387a98bb7c37292057cffc56d62ecb629900026402633ae9160df93a8766"
SRC_URI[cfg-if-1.0.0.sha256sum] = "baf1de4339761588bc0619e3cbc0120ee582ebb74b53b4efbf79117bd2da40fd"
SRC_URI[clang-sys-1.6.1.sha256sum] = "c688fc74432808e3eb684cae8830a86be1d66a2bd58e1f248ed0960a590baf6f"
SRC_URI[cmake-0.1.50.sha256sum] = "a31c789563b815f77f4250caee12365734369f942439b7defd71e18a48197130"
SRC_URI[derivative-2.2.0.sha256sum] = "fcc3dd5e9e9c0b295d6e1e4d811fb6f157d5ffd784b8d202fc62eac8035a770b"
SRC_URI[either-1.9.0.sha256sum] = "a26ae43d7bcc3b814de94796a5e736d4029efb0ee900c12e2d54c993ad1a1e07"
SRC_URI[env_logger-0.7.1.sha256sum] = "44533bbbb3bb3c1fa17d9f2e4e38bbbaf8396ba82193c4cb1b6445d711445d36"
SRC_URI[errno-0.3.8.sha256sum] = "a258e46cdc063eb8519c00b9fc845fc47bcfca4130e2f08e88665ceda8474245"
SRC_URI[form_urlencoded-1.2.1.sha256sum] = "e13624c2627564efccf4934284bdd98cbaa14e79b0b5a141218e507b3a823456"
SRC_URI[glob-0.3.1.sha256sum] = "d2fabcfbdc87f4758337ca535fb41a6d701b65693ce38287d856d1674551ec9b"
SRC_URI[hermit-abi-0.1.19.sha256sum] = "62b467343b94ba476dcb2500d242dadbb39557df889310ac77c5d99100aaac33"
SRC_URI[home-0.5.5.sha256sum] = "5444c27eef6923071f7ebcc33e3444508466a76f7a2b93da00ed6e19f30c1ddb"
SRC_URI[humantime-1.3.0.sha256sum] = "df004cfca50ef23c36850aaaa59ad52cc70d0e90243c3c7737a4dd32dc7a3c4f"
SRC_URI[idna-0.5.0.sha256sum] = "634d9b1461af396cad843f47fdba5597a4f9e6ddd4bfb6ff5d85028c25cb12f6"
SRC_URI[itertools-0.10.5.sha256sum] = "b0fd2260e829bddf4cb6ea802289de2f86d6a7a690192fbe91b3f46e0f2c8473"
SRC_URI[lazy_static-1.4.0.sha256sum] = "e2abad23fbc42b3700f2f279844dc832adb2b2eb069b2df918f455c4e18cc646"
SRC_URI[lazycell-1.3.0.sha256sum] = "830d08ce1d1d941e6b30645f1a0eb5643013d835ce3779a5fc208261dbe10f55"
SRC_URI[libc-0.2.151.sha256sum] = "302d7ab3130588088d277783b1e2d2e10c9e9e4a16dd9050e6ec93fb3e7048f4"
SRC_URI[libloading-0.7.4.sha256sum] = "b67380fd3b2fbe7527a606e18729d21c6f3951633d0500574c4dc22d2d638b9f"
SRC_URI[linux-raw-sys-0.4.12.sha256sum] = "c4cd1a83af159aa67994778be9070f0ae1bd732942279cabb14f86f986a21456"
SRC_URI[log-0.4.20.sha256sum] = "b5e6163cb8c49088c2c36f57875e58ccd8c87c7427f7fbd50ea6710b2f3f2e8f"
SRC_URI[memchr-2.6.4.sha256sum] = "f665ee40bc4a3c5590afb1e9677db74a508659dfd71e126420da8274909a0167"
SRC_URI[minimal-lexical-0.2.1.sha256sum] = "68354c5c6bd36d73ff3feceb05efa59b6acb7626617f4962be322a825e61f79a"
SRC_URI[nom-7.1.3.sha256sum] = "d273983c5a657a70a3e8f2a01329822f3b8c8172b73826411a55751e404a0a4a"
SRC_URI[num-0.4.1.sha256sum] = "b05180d69e3da0e530ba2a1dae5110317e49e3b7f3d41be227dc5f92e49ee7af"
SRC_URI[num-bigint-0.4.4.sha256sum] = "608e7659b5c3d7cba262d894801b9ec9d00de989e8a82bd4bef91d08da45cdc0"
SRC_URI[num-complex-0.4.4.sha256sum] = "1ba157ca0885411de85d6ca030ba7e2a83a28636056c7c699b07c8b6f7383214"
SRC_URI[num-derive-0.4.1.sha256sum] = "cfb77679af88f8b125209d354a202862602672222e7f2313fdd6dc349bad4712"
SRC_URI[num-integer-0.1.45.sha256sum] = "225d3389fb3509a24c93f5c29eb6bde2586b98d9f016636dff58d7c6f7569cd9"
SRC_URI[num-iter-0.1.43.sha256sum] = "7d03e6c028c5dc5cac6e2dec0efda81fc887605bb3d884578bb6d6bf7514e252"
SRC_URI[num-rational-0.4.1.sha256sum] = "0638a1c9d0a3c0914158145bc76cff373a75a627e6ecbfb71cbe6f453a5a19b0"
SRC_URI[num-traits-0.2.17.sha256sum] = "39e3200413f237f41ab11ad6d161bc7239c84dcb631773ccd7de3dfe4b5c267c"
SRC_URI[once_cell-1.19.0.sha256sum] = "3fdb12b2476b595f9358c5161aa467c2438859caa136dec86c26fdd2efe17b92"
SRC_URI[parsec-client-0.16.0.sha256sum] = "a36f9d8e27166cf0586913812454174286e094d594cc8b28d8a8d02d64406bbc"
SRC_URI[parsec-interface-0.29.1.sha256sum] = "cc706e09209b30f10baa35709d41b9cc01d4931b21c00679f59db96cd1650add"
SRC_URI[peeking_take_while-0.1.2.sha256sum] = "19b17cddbe7ec3f8bc800887bab5e717348c95ea2ca0b1bf0837fb964dc67099"
SRC_URI[percent-encoding-2.3.1.sha256sum] = "e3148f5046208a5d56bcfc03053e3ca6334e51da8dfb19b6cdc8b306fae3283e"
SRC_URI[prettyplease-0.2.15.sha256sum] = "ae005bd773ab59b4725093fd7df83fd7892f7d8eafb48dbd7de6e024e4215f9d"
SRC_URI[proc-macro2-1.0.70.sha256sum] = "39278fbbf5fb4f646ce651690877f89d1c5811a3d4acb27700c1cb3cdb78fd3b"
SRC_URI[prost-0.9.0.sha256sum] = "444879275cb4fd84958b1a1d5420d15e6fcf7c235fe47f053c9c2a80aceb6001"
SRC_URI[prost-derive-0.9.0.sha256sum] = "f9cc1a3263e07e0bf68e96268f37665207b49560d98739662cdfaae215c720fe"
SRC_URI[psa-crypto-0.12.0.sha256sum] = "89c2256e525b9a45ec3bbb3382a43dd8809240279e0aab8ea7ee220e9295445b"
SRC_URI[psa-crypto-sys-0.12.0.sha256sum] = "f170cac3a328e1678916b276067ec170a5a51db1b9b8b4c00b44c2839819a963"
SRC_URI[quick-error-1.2.3.sha256sum] = "a1d01941d82fa2ab50be1e79e6714289dd7cde78eba4c074bc5a4374f650dfe0"
SRC_URI[quote-1.0.33.sha256sum] = "5267fca4496028628a95160fc423a33e8b2e6af8a5302579e322e4b520293cae"
SRC_URI[regex-1.10.2.sha256sum] = "380b951a9c5e80ddfd6136919eef32310721aa4aacd4889a8d39124b026ab343"
SRC_URI[regex-automata-0.4.3.sha256sum] = "5f804c7828047e88b2d32e2d7fe5a105da8ee3264f01902f796c8e067dc2483f"
SRC_URI[regex-syntax-0.8.2.sha256sum] = "c08c74e62047bb2de4ff487b251e4a92e24f48745648451635cec7d591162d9f"
SRC_URI[rustc-hash-1.1.0.sha256sum] = "08d43f7aa6b08d49f382cde6a7982047c3426db949b1424bc4b7ec9ae12c6ce2"
SRC_URI[rustix-0.38.28.sha256sum] = "72e572a5e8ca657d7366229cdde4bd14c4eb5499a9573d4d366fe1b599daa316"
SRC_URI[same-file-1.0.6.sha256sum] = "93fc1dc3aaa9bfed95e02e6eadabb4baf7e3078b0bd1b4d7b6b0b68378900502"
SRC_URI[secrecy-0.8.0.sha256sum] = "9bd1c54ea06cfd2f6b63219704de0b9b4f72dcc2b8fdef820be6cd799780e91e"
SRC_URI[serde-1.0.193.sha256sum] = "25dd9975e68d0cb5aa1120c288333fc98731bd1dd12f561e468ea4728c042b89"
SRC_URI[serde_derive-1.0.193.sha256sum] = "43576ca501357b9b071ac53cdc7da8ef0cbd9493d8df094cd821777ea6e894d3"
SRC_URI[shlex-1.2.0.sha256sum] = "a7cee0529a6d40f580e7a5e6c495c8fbfe21b7b52795ed4bb5e62cdf92bc6380"
SRC_URI[syn-1.0.109.sha256sum] = "72b64191b275b66ffe2469e8af2c1cfe3bafa67b529ead792a6d0160888b4237"
SRC_URI[syn-2.0.41.sha256sum] = "44c8b28c477cc3bf0e7966561e3460130e1255f7a1cf71931075f1c5e7a7e269"
SRC_URI[termcolor-1.4.0.sha256sum] = "ff1bc3d3f05aff0403e8ac0d92ced918ec05b666a43f83297ccef5bea8a3d449"
SRC_URI[tinyvec-1.6.0.sha256sum] = "87cc5ceb3875bb20c2890005a4e226a4651264a5c75edb2421b52861a0a0cb50"
SRC_URI[tinyvec_macros-0.1.1.sha256sum] = "1f3ccbac311fea05f86f61904b462b55fb3df8837a366dfc601a0161d0532f20"
SRC_URI[unicode-bidi-0.3.14.sha256sum] = "6f2528f27a9eb2b21e69c95319b30bd0efd85d09c379741b0f78ea1d86be2416"
SRC_URI[unicode-ident-1.0.12.sha256sum] = "3354b9ac3fae1ff6755cb6db53683adb661634f67557942dea4facebec0fee4b"
SRC_URI[unicode-normalization-0.1.22.sha256sum] = "5c5713f0fc4b5db668a2ac63cdb7bb4469d8c9fed047b1d0292cc7b0ce2ba921"
SRC_URI[url-2.5.0.sha256sum] = "31e6302e3bb753d46e83516cae55ae196fc0c309407cf11ab35cc51a4c2a4633"
SRC_URI[uuid-0.8.2.sha256sum] = "bc5cf98d8186244414c848017f0e2676b3fcb46807f6668a97dfe67359a3c4b7"
SRC_URI[walkdir-2.4.0.sha256sum] = "d71d857dc86794ca4c280d616f7da00d2dbfd8cd788846559a6813e6aa4b54ee"
SRC_URI[which-4.4.2.sha256sum] = "87ba24419a2078cd2b0f2ede2691b6c66d8e47836da3b6db8265ebad47afbfc7"
SRC_URI[winapi-0.3.9.sha256sum] = "5c839a674fcd7a98952e593242ea400abe93992746761e38641405d28b00f419"
SRC_URI[winapi-i686-pc-windows-gnu-0.4.0.sha256sum] = "ac3b87c63620426dd9b991e5ce0329eff545bccbbb34f3be09ff6fb6ab51b7b6"
SRC_URI[winapi-util-0.1.6.sha256sum] = "f29e6f9198ba0d26b4c9f07dbe6f9ed633e1f3d5b8b414090084349e46a52596"
SRC_URI[winapi-x86_64-pc-windows-gnu-0.4.0.sha256sum] = "712e227841d057c1ee1cd2fb22fa7e5a5461ae8e48fa2ca79ec42cfc1931183f"
SRC_URI[windows-sys-0.48.0.sha256sum] = "677d2418bec65e3338edb076e806bc1ec15693c5d0104683f2efe857f61056a9"
SRC_URI[windows-sys-0.52.0.sha256sum] = "282be5f36a8ce781fad8c8ae18fa3f9beff57ec1b52cb3de0789201425d9a33d"
SRC_URI[windows-targets-0.48.5.sha256sum] = "9a2fa6e2155d7247be68c096456083145c183cbbbc2764150dda45a87197940c"
SRC_URI[windows-targets-0.52.0.sha256sum] = "8a18201040b24831fbb9e4eb208f8892e1f50a37feb53cc7ff887feb8f50e7cd"
SRC_URI[windows_aarch64_gnullvm-0.48.5.sha256sum] = "2b38e32f0abccf9987a4e3079dfb67dcd799fb61361e53e2882c3cbaf0d905d8"
SRC_URI[windows_aarch64_gnullvm-0.52.0.sha256sum] = "cb7764e35d4db8a7921e09562a0304bf2f93e0a51bfccee0bd0bb0b666b015ea"
SRC_URI[windows_aarch64_msvc-0.48.5.sha256sum] = "dc35310971f3b2dbbf3f0690a219f40e2d9afcf64f9ab7cc1be722937c26b4bc"
SRC_URI[windows_aarch64_msvc-0.52.0.sha256sum] = "bbaa0368d4f1d2aaefc55b6fcfee13f41544ddf36801e793edbbfd7d7df075ef"
SRC_URI[windows_i686_gnu-0.48.5.sha256sum] = "a75915e7def60c94dcef72200b9a8e58e5091744960da64ec734a6c6e9b3743e"
SRC_URI[windows_i686_gnu-0.52.0.sha256sum] = "a28637cb1fa3560a16915793afb20081aba2c92ee8af57b4d5f28e4b3e7df313"
SRC_URI[windows_i686_msvc-0.48.5.sha256sum] = "8f55c233f70c4b27f66c523580f78f1004e8b5a8b659e05a4eb49d4166cca406"
SRC_URI[windows_i686_msvc-0.52.0.sha256sum] = "ffe5e8e31046ce6230cc7215707b816e339ff4d4d67c65dffa206fd0f7aa7b9a"
SRC_URI[windows_x86_64_gnu-0.48.5.sha256sum] = "53d40abd2583d23e4718fddf1ebec84dbff8381c07cae67ff7768bbf19c6718e"
SRC_URI[windows_x86_64_gnu-0.52.0.sha256sum] = "3d6fa32db2bc4a2f5abeacf2b69f7992cd09dca97498da74a151a3132c26befd"
SRC_URI[windows_x86_64_gnullvm-0.48.5.sha256sum] = "0b7b52767868a23d5bab768e390dc5f5c55825b6d30b86c844ff2dc7414044cc"
SRC_URI[windows_x86_64_gnullvm-0.52.0.sha256sum] = "1a657e1e9d3f514745a572a6846d3c7aa7dbe1658c056ed9c3344c4109a6949e"
SRC_URI[windows_x86_64_msvc-0.48.5.sha256sum] = "ed94fce61571a4006852b7389a063ab983c02eb1bb37b47f8272ce92d06d9538"
SRC_URI[windows_x86_64_msvc-0.52.0.sha256sum] = "dff9641d1cd4be8d1a070daf9e3773c5f67e78b4d9d42263020c057706765c04"
SRC_URI[zeroize-1.7.0.sha256sum] = "525b4ec142c6b68a2d10f01f7bbf6755599ca3f81ea53b8431b7dd348f5fdb2d"
SRC_URI[zeroize_derive-1.4.2.sha256sum] = "ce36e65b0d2999d2aafac989fb249189a141aee1f53c612c1f37d72631959f69"
# /* cSpell:enable */
