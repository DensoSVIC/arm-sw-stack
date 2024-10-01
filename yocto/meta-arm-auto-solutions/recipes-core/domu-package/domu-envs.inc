#
# SPDX-FileCopyrightText: <text>Copyright 2023-2024 Arm Limited and/or its
# affiliates <open-source-office@arm.com></text>
#
# SPDX-License-Identifier: MIT

# This class adds configuration parameters for DOMU.
#
# The number of DOMU instances is defined in DOMU_INSTANCES. Each
# DOMU can then be independently configured via Bitbake variables that
# reference the DOMU's integer instance index, from 1 to the value of
# DOMU_INSTANCES, inclusive. The available variables are as follows,
# where ${IDX} corresponds to a target DOMU's integer instance index:
#
#   (*) DOMU${IDX}_HOSTNAME
#   (*) DOMU${IDX}_MEMORY_SIZE
#   (*) DOMU${IDX}_NUMBER_OF_CPUS
#   (*) DOMU${IDX}_EXTRA
#
# If no custom value for any of the above parameters is provided, the default
# value is used.

DOMU_SVE_SETTING ?= "${@ '128' if d.getVar('SVE_DISABLE_FLAG', True) != '1' else 'disabled'}"
DOMU_HOSTNAME_PREFIX ?= "domu"
DOMU_MEMORY_SIZE_DEFAULT ?= "1024"
DOMU_NUMBER_OF_CPUS_DEFAULT ?= "4"
DOMU_INSTANCES ?= "2"
DOMU_ENV_PATH = "${WORKDIR}/envs"

# Set extra configuration via DOMU${IDX}_EXTRA.
DOMU1_BRIDGES ?= "vif = ['script=vif-openvswitch,bridge=ovsbr0,vifname=domu1.ext0', 'script=vif-openvswitch,bridge=brsi0.100,vifname=domu1.ethsi0', 'script=vif-openvswitch,bridge=brsi1.200,vifname=domu1.ethsi1', 'script=vif-openvswitch,bridge=brsi2.300,vifname=domu1.ethsi2']"
DOMU1_NUMBER_OF_CPUS ?= "2"
# Refer to https://xenbits.xen.org/docs/4.17-testing/man/xl.cfg.5.html#CPU-Allocation
# about how to set the CPULIST.
DOMU1_VCPU_PIN ?= "cpus = [\"1\", \"2\"]"
DOMU1_MPAM ?= "mpam = [\"slc=0xc0\"]"
DOMU1_SVE ?= "sve = \"${DOMU_SVE_SETTING}\""
DOMU1_PCI_PASSTHROUGH ?= "pci = [\"${DOMU1_PCI_ID}\"]"
DOMU1_EXTRA ?= "${DOMU1_BRIDGES}\\n${DOMU1_VCPU_PIN}\\n${DOMU1_MPAM}\\n${DOMU1_SVE}\\n${DOMU1_PCI_PASSTHROUGH}\\n"

DOMU2_BRIDGES ?= "vif = ['script=vif-openvswitch,bridge=ovsbr0,vifname=domu2.ext0', 'script=vif-openvswitch,bridge=brsi0.100,vifname=domu2.ethsi0', 'script=vif-openvswitch,bridge=brsi1.200,vifname=domu2.ethsi1', 'script=vif-openvswitch,bridge=brsi2.300,vifname=domu2.ethsi2']"
DOMU2_NUMBER_OF_CPUS ?= "1"
DOMU2_VCPU_PIN ?= "cpus = [\"3\"]"
DOMU2_MPAM ?= "mpam = [\"slc=0x30\"]"
DOMU2_SVE ?= "sve = \"${DOMU_SVE_SETTING}\""
DOMU2_PCI_PASSTHROUGH ?= ""
DOMU2_EXTRA ?= "${DOMU2_BRIDGES}\\n${DOMU2_VCPU_PIN}\\n${DOMU2_MPAM}\\n${DOMU2_SVE}\\n${DOMU2_PCI_PASSTHROUGH}\\n"

python() {
    domu_instances = d.getVar("DOMU_INSTANCES")

    if (not domu_instances or
       not 0 < int(domu_instances) <= 2):
        raise bb.parse.SkipRecipe(
              f"DOMU_INSTANCES=\"{domu_instances}\" not correctly set")

    # Include DOMU${IDX}_* variables in do_install[vardeps]
    for idx in range(1, int(domu_instances) + 1):
        for var_sufix in ["_HOSTNAME", "_MEMORY_SIZE", "_NUMBER_OF_CPUS", "_EXTRA"]:
            var = f" DOMU{idx}{var_sufix}"
            d.appendVarFlag('do_install', 'vardeps', var)
}

python gen_domu_env_and_extra_conf() {
    domu_instances = d.getVar("DOMU_INSTANCES")
    outdir = d.getVar('DOMU_ENV_PATH')
    bb.utils.remove(f"{outdir}/*", False, True)
    bb.utils.mkdirhier(outdir)

    link_name = f"domus"
    # Write common configurations of all domus into one file
    with open(os.path.join(outdir, link_name) + '.env', 'w') as envf:
        for idx in range(1, int(domu_instances) + 1):
            for var_sufix in ["_HOSTNAME", "_MEMORY_SIZE", "_NUMBER_OF_CPUS"]:
                var = f"DOMU{idx}{var_sufix}"
                value = d.getVar(var)

                if not value:
                    if var_sufix == "_HOSTNAME":
                        value = f"{d.getVar('DOMU_HOSTNAME_PREFIX')}{idx}"
                    else:
                        value = d.getVar(f"DOMU{var_sufix}_DEFAULT")

                if value:
                    envf.write('%s="%s"\n' % (var, value.strip()))

    # Write extra configuration of domu idx into file domu{idx}-extra.cfg
    # Extra configuration for domu idx can be set by DOMU{idx}_EXTRA
    for idx in range(1, int(domu_instances) + 1):
        var = f"DOMU{idx}_EXTRA"
        value = d.getVar(var)
        if value:
            link_name = f"domu{idx}-extra"
            value = value.replace("\\n", "\n")
            value = value.replace("\\", "")
            with open(os.path.join(outdir, link_name) + '.cfg', 'w') as envf:
                envf.write(value)
}

gen_domu_env_and_extra_conf[doc] = "Python function that generates DomU \
environment and extra configurations."
