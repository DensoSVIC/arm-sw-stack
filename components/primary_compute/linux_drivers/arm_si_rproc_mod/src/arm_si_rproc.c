/*
 * SPDX-FileCopyrightText: <text>Copyright 2023 Arm Limited and/or its
 * affiliates <open-source-office@arm.com></text>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * ARM Safety Island Remote Processor driver
 *
 * Rahul Singh <rahul.singh@arm.com>
 */

#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/mailbox_client.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/remoteproc.h>
#include <linux/slab.h>
#include <linux/workqueue.h>


#define ARM_SI_MBX_RX		(0)
#define ARM_SI_MBX_TX		(1)
#define ARM_SI_MAX_VRINGS	(2)
#define ARM_SI_MBX_MAX		(7)
#define ARM_SI_MBX_VQ0_ID	(0)
#define ARM_SI_MBX_VQ1_ID	(1)
#define ARM_SI_MBX_DETACH_ID	(4)
#define ARM_SI_MBX_ATTACH_ID	(5)
#define ARM_SI_MBX_ACK_ID	(6)
#define ARM_SI_MBX_VQ0_RX	"vq0_rx"
#define ARM_SI_MBX_VQ1_RX	"vq1_rx"
#define ARM_SI_MBX_VQ0_TX	"vq0_tx"
#define ARM_SI_MBX_VQ1_TX	"vq1_tx"
#define ARM_SI_MBX_DETACH	"detach"
#define ARM_SI_MBX_ATTACH	"attach"
#define ARM_SI_MBX_ACK		"ack"

#define CHANNEL_READY_TIMEOUT_MS    (100)

static int timeout = CHANNEL_READY_TIMEOUT_MS;
module_param(timeout, uint, 0);
MODULE_PARM_DESC(timeout, "SI acknowledgement timeout in milliseconds");

/*
 * This is defined in drivers/remoteproc/remoteproc_internal.h
 * in the kernel source
 */
irqreturn_t rproc_vq_interrupt(struct rproc *rproc, int vq_id);

/**
 * struct arm_si_mbox
 *
 * @name: mbox name
 * @chan: pointer to mbox channel
 * @client: mbox client
 * @vq_work: work struct
 * @vq_id: notify id for the virtqueue
 */
struct arm_si_mbox {
	const unsigned char name[20];
	struct mbox_chan *chan;
	struct mbox_client client;
	struct work_struct vq_work;
	int vq_id;
};

/**
 * struct arm_si_rproc_mr
 * hold the dev->cpu bus address mapping
 *
 * @dev_addr: dev addr of the shared memory region
 * @bus_addr: cpu bus addr of the shared memory region
 */
struct arm_si_rproc_mr {
	u64 dev_addr;
	u64 bus_addr;
};

/**
 * struct arm_si_rproc_rsctbl
 * hold the base and size of rproc resource table
 *
 * @start: base address of the resource table
 * @size: size of the resource table
 * @rsc_va: resource table virtual address.
 *
 */
struct arm_si_rproc_rsctbl {
	resource_size_t start;
	resource_size_t size;
	void __iomem *rsc_va;
};

/**
 * struct arm_si_channel
 *
 * @dev: device of SI channel instance
 * @memr: pointer to memory range struct
 * @rsctbl: pointer to rsctbl
 * @wq: workqueue struct
 * @ack: completion primitive to get acknowledgement from remote SI
 * @mbox: array of mbox
 * @rproc: rproc handle
 */
struct arm_si_channel {
	struct device *dev;
	struct arm_si_rproc_mr *memr;
	struct arm_si_rproc_rsctbl *rsctbl;
	struct workqueue_struct *wq;
	struct completion ack;
	struct arm_si_mbox mbox[ARM_SI_MBX_MAX];
	struct rproc *rproc;
};

/**
 * struct arm_si_rproc
 *
 * @dev: SI rproc device node
 * @channel_count: number of channel
 * @si_channels: Array of pointers pointing to SI channels
 */
struct arm_si_rproc {
	struct device *dev;
	int channel_count;
	struct arm_si_channel **chs;
};

/**
 * arm_si_rproc_mem_alloc()
 * @rproc: rproc instance
 * @mem: mem descriptor to map reserved memory-regions
 *
 * Callback to map va for memory-region's carveout.
 *
 * return 0 on success, otherwise non-zero value on failure
 */
static int arm_si_rproc_mem_alloc(struct rproc *rproc,
				  struct rproc_mem_entry *mem)
{
	void __iomem *va;

	va = ioremap_wc(mem->dma, mem->len);
	if (IS_ERR_OR_NULL(va))
		return -ENOMEM;

	mem->va = (void *)va;

	return 0;
}

/**
 * arm_si_rproc_mem_release()
 * @rproc: rproc instance
 * @mem: mem entry to unmap
 *
 * Unmap memory-region carveout
 *
 * return: always returns 0
 */
static int arm_si_rproc_mem_release(struct rproc *rproc,
				    struct rproc_mem_entry *mem)
{
	iounmap((void __iomem *)mem->va);
	return 0;
}

/**
 * arm_si_rproc_pa_to_da()
 * @rproc: rproc instance
 * @pa: logical physical address
 * @da: remote device address
 *
 * Translate logical physical address to remote device address
 */
static void arm_si_rproc_pa_to_da(struct rproc *rproc, phys_addr_t pa, u64 *da)
{
	struct arm_si_channel *ch = rproc->priv;
	struct arm_si_rproc_mr *p_mr = ch->memr;

	*da = pa - p_mr->bus_addr + p_mr->dev_addr;
	dev_dbg(rproc->dev.parent, "pa %pa to da %llx\n", &pa, *da);
}

/**
 * arm_si_rproc_kick()
 * @rproc: rproc instance
 * @vqid: notifyid for the virtqueue
 *
 * kick a virtqueue (vqid). Send the mbox event to the remote processor
 * to notify which vq to poke at
 */
static void arm_si_rproc_kick(struct rproc *rproc, int vqid)
{
	struct arm_si_channel *ch = rproc->priv;
	unsigned int i = vqid * ARM_SI_MAX_VRINGS + ARM_SI_MBX_TX;
	int err;

	if (WARN_ON(vqid >= ARM_SI_MAX_VRINGS))
		return;

	if (!ch->mbox[i].chan)
		return;

	dev_dbg(&rproc->dev, "kick (vqid:%d mbox:%s)\n", vqid,
		ch->mbox[i].name);

	/*
	 * TODO: convert dev_dbg to dev_err when mbox_send_message
	 * start passing every time.
	 */
	err = mbox_send_message(ch->mbox[i].chan, (void *)&vqid);
	if (err < 0)
		dev_dbg(&rproc->dev, "%s: failed (%s, err:%d)\n",
			__func__, ch->mbox[i].name, err);
}

/**
 * arm_si_rproc_get_loaded_rsc_table()
 * @rproc: rproc instance
 * @table_sz: resource table size out parameter
 *
 * Get resource table installed in memory
 *
 * Return: resource table pointer virtual address as success,
 * < 0 for failure.
 */
static struct resource_table *
arm_si_rproc_get_loaded_rsc_table(struct rproc *rproc, size_t *table_sz)
{
	struct arm_si_channel *ch = rproc->priv;
	struct device *dev = ch->dev;
	struct arm_si_rproc_rsctbl *rsctbl = ch->rsctbl;

	/* The resource table has already been mapped, nothing to do */
	if (rsctbl->rsc_va)
		goto done;

	rsctbl->rsc_va = devm_ioremap_wc(dev, rsctbl->start, rsctbl->size);
	if (IS_ERR_OR_NULL(rsctbl->rsc_va)) {
		dev_err(dev, "Unable to map memory region: %pa+%pa\n",
			&rsctbl->start, &rsctbl->size);
		rsctbl->rsc_va = NULL;
		return ERR_PTR(-ENOMEM);
	}

done:
	*table_sz = rsctbl->size;
	return (struct resource_table *)rsctbl->rsc_va;
}

/**
 * arm_si_rproc_prepare()
 * @rproc: rproc instance
 *
 * Return: 0 for success else < 0 error code
 */
static int arm_si_rproc_prepare(struct rproc *rproc)
{
	struct arm_si_channel *ch = rproc->priv;
	struct device *dev = ch->dev;
	struct device_node *np = dev->of_node;
	struct of_phandle_iterator it;
	struct rproc_mem_entry *rproc_mem;
	struct reserved_mem *rmem;
	struct arm_si_rproc_mr *mem_range;
	struct arm_si_rproc_rsctbl *rsctbl;
	u64 da;
	int array_size, ret, index = 0, i = 0;

	mem_range = kcalloc(1, sizeof(*mem_range), GFP_KERNEL);
	if (!mem_range)
		return -ENOMEM;

	array_size = sizeof(*mem_range) / sizeof(u64);
	ret = of_property_read_u64_array(np, "ranges",
					 (u64 *)mem_range, array_size);
	if (ret) {
		dev_err(dev, "Error while get ranges property: %x\n", ret);
		goto free_mem;
	}
	ch->memr = mem_range;

	rsctbl = kcalloc(1, sizeof(*rsctbl), GFP_KERNEL);
	if (!rsctbl)
		return -ENOMEM;

	/* Register associated reserved memory regions */
	of_phandle_iterator_init(&it, np, "memory-region", NULL, 0);
	while (of_phandle_iterator_next(&it) == 0) {
		rmem = of_reserved_mem_lookup(it.node);
		if (!rmem) {
			dev_err(dev, "Unable to acquire memory-region\n");
			return -EINVAL;
		}

		if (strstr(rmem->name, "rsctbl")) {
			rsctbl->start = rmem->base;
			rsctbl->size = rmem->size;
			ch->rsctbl = rsctbl;
			index++;
			continue;
		}

		/*  No need to map vdev buffer */
		if (strstr(rmem->name, "vdev0buffer")) {
			/* Init reserved memory for vdev buffer */
			rproc_mem = rproc_of_resm_mem_entry_init(&rproc->dev,
								 index,
								 rmem->size,
								 rmem->base,
								 "vdev0buffer");
		} else {
			arm_si_rproc_pa_to_da(rproc, rmem->base, &da);
			rproc_mem = rproc_mem_entry_init(dev, NULL,
							 (dma_addr_t)rmem->base,
							 rmem->size, da,
							 arm_si_rproc_mem_alloc,
							 arm_si_rproc_mem_release,
							 "vdev0vring%d", i++);

			if (rproc_mem)
				rproc_coredump_add_segment(rproc, da,
							   rmem->size);
		}

		if (!rproc_mem)
			return -ENOMEM;

		rproc_add_carveout(rproc, rproc_mem);

		dev_dbg(&rproc->dev,
			"reserved mem carveout %s addr=%llx, size=0x%llx",
			rproc_mem->name, rmem->base, rmem->size);

		index++;
	}

	return 0;

free_mem:
	kfree(mem_range);
	return ret;
}

/**
 * arm_si_rproc_mb_vq_work()
 * @work: work to be done
 *
 * Interrupt the virtqueue for rx
 */
static void arm_si_rproc_mb_vq_work(struct work_struct *work)
{
	struct arm_si_mbox *mb = container_of(work, struct arm_si_mbox, vq_work);
	struct rproc *rproc = dev_get_drvdata(mb->client.dev);

	dev_dbg(&rproc->dev, "vq%d interrupted (mbox:%s)\n", mb->vq_id,
		mb->name);

	if (rproc_vq_interrupt(rproc, mb->vq_id) == IRQ_NONE)
		dev_dbg(&rproc->dev, "no message found in vq%d\n", mb->vq_id);
}

/**
 * arm_si_rproc_mb_callback()
 * @cl: mbox client
 * @data: private data
 *
 * Callback to be called for mbox rx
 */
static void arm_si_rproc_mb_callback(struct mbox_client *cl, void *data)
{
	struct rproc *rproc = dev_get_drvdata(cl->dev);
	struct arm_si_channel *ch = rproc->priv;
	struct arm_si_mbox *mb = container_of(cl, struct arm_si_mbox, client);

	queue_work(ch->wq, &mb->vq_work);
}

/**
 * arm_si_rproc_ack_callback()
 * @cl: mbox client
 * @data: private data
 *
 * Callback to be called for mbox rx when acknowledge from SI
 */
static void arm_si_rproc_ack_callback(struct mbox_client *cl, void *data)
{
	struct rproc *rproc = dev_get_drvdata(cl->dev);
	struct arm_si_channel *ch = rproc->priv;

	complete(&ch->ack);
}

/**
 * arm_si_rproc_free_mbox()
 * @rpoc: rproc instance
 *
 * relinquishes control of a mailbox channel
 */
static void arm_si_rproc_free_mbox(struct rproc *rproc)
{
	struct arm_si_channel *ch = rproc->priv;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(ch->mbox); i++) {
		if (ch->mbox[i].chan)
			mbox_free_channel(ch->mbox[i].chan);
		ch->mbox[i].chan = NULL;
	}
}

static const struct arm_si_mbox arm_si_rproc_mbox[ARM_SI_MBX_MAX] = {
	{
		.name = ARM_SI_MBX_VQ0_RX,
		.vq_id = ARM_SI_MBX_VQ0_ID,
		.client = {
			.rx_callback = arm_si_rproc_mb_callback,
		},
	},
	{
		.name = ARM_SI_MBX_VQ0_TX,
		.vq_id = ARM_SI_MBX_VQ0_ID,
		.client = {
			.tx_block = false,
			.tx_done = NULL,
		},
	},
	{
		.name = ARM_SI_MBX_VQ1_RX,
		.vq_id = ARM_SI_MBX_VQ1_ID,
		.client = {
			.rx_callback = arm_si_rproc_mb_callback,
		},
	},
	{
		.name = ARM_SI_MBX_VQ1_TX,
		.vq_id = ARM_SI_MBX_VQ1_ID,
		.client = {
			.tx_block = false,
			.tx_done = NULL,
		},
	},
	{
		.name = ARM_SI_MBX_DETACH,
		.client = {
			.tx_block = false,
			.tx_done = NULL,
		},
	},
	{
		.name = ARM_SI_MBX_ATTACH,
		.client = {
			.tx_block = false,
			.tx_done = NULL,
		},
	},
	{
		.name = ARM_SI_MBX_ACK,
		.client = {
			.rx_callback = arm_si_rproc_ack_callback,
		},
	}
};

/**
 * arm_si_rproc_request_mbox()
 * @rpoc: rproc instance
 *
 * Request the control mbox chan
 *
 * Return: 0 for success and < 0 for failure.
 */
static int arm_si_rproc_request_mbox(struct rproc *rproc)
{
	struct arm_si_channel *ch = rproc->priv;
	struct device *dev = ch->dev;
	const unsigned char *name;
	struct mbox_client *cl;
	struct mbox_chan *chan;
	unsigned int i;
	int j, ret;

	/* Initialise mailbox structure table */
	memcpy(ch->mbox, arm_si_rproc_mbox, sizeof(arm_si_rproc_mbox));

	for (i = 0; i < ARM_SI_MBX_MAX; i++) {
		name = ch->mbox[i].name;
		cl = &ch->mbox[i].client;
		cl->dev = dev;

		chan = mbox_request_channel_byname(cl, name);
		if (IS_ERR(chan)) {
			chan = NULL;
			ret = PTR_ERR(ch);
			dev_err(dev, "Cannot get %s mbox\n", name);
			goto err_mbox_chan;
		}
		ch->mbox[i].chan = chan;

		if (cl->rx_callback)
			INIT_WORK(&ch->mbox[i].vq_work,
				  arm_si_rproc_mb_vq_work);
	}

	ch->wq = alloc_workqueue("arm-si-rproc-wq", WQ_CPU_INTENSIVE, 0);
	if (!ch->wq) {
		dev_err(dev, "Cannot create workqueue\n");
		ret = -ENOMEM;
		goto err_mbox_chan;
	}

	return 0;

err_mbox_chan:
	for (j = i - 1; j >= 0; j--)
		if (ch->mbox[j].chan)
			mbox_free_channel(ch->mbox[j].chan);
	return ret;
}

static int arm_si_rproc_attach(struct rproc *rproc)
{
	return 0;
}

static int arm_si_rproc_detach(struct rproc *rproc)
{
	struct arm_si_channel *ch = rproc->priv;
	int ret, idx = ARM_SI_MBX_DETACH_ID;

	if (ch->mbox[idx].chan) {
		ret = mbox_send_message(ch->mbox[idx].chan, NULL);
		if (ret < 0)
			dev_dbg(&rproc->dev, "%s: failed (%s, err:%d)\n",
				__func__, ch->mbox[idx].name, ret);
	}

	return 0;
}

static const struct rproc_ops arm_si_rproc_ops = {
	.kick		= arm_si_rproc_kick,
	.prepare	= arm_si_rproc_prepare,
	.attach		= arm_si_rproc_attach,
	.detach		= arm_si_rproc_detach,
	.get_loaded_rsc_table	= arm_si_rproc_get_loaded_rsc_table,
};

/**
 * arm_si_add_rproc_core()
 * @cdev: Device node of each SI channel
 *
 * Allocate and add struct rproc object for each SI channel
 * This is called for each individual SI channel
 *
 * Return: arm_si_channel object for success else error code pointer
 */
static struct arm_si_channel *arm_si_add_rproc_core(struct device *cdev)
{
	struct arm_si_channel *ch;
	struct rproc *rproc;
	int ret, idx = ARM_SI_MBX_ATTACH_ID;

	/* Allocate rproc instance */
	rproc = devm_rproc_alloc(cdev, cdev->of_node->name,
				 &arm_si_rproc_ops,
				 NULL, sizeof(struct arm_si_channel));
	if (!rproc) {
		dev_err(cdev,
			"Failed to allocate memory for rproc instance\n");
		return ERR_PTR(-ENOMEM);
	}

	rproc->state = RPROC_DETACHED;
	ch = (struct arm_si_channel *)rproc->priv;
	ch->dev = cdev;
	dev_set_drvdata(cdev, rproc);

	ret = arm_si_rproc_request_mbox(rproc);
	if (ret)
		return ERR_PTR(ret);

	if (ch->mbox[idx].chan) {
		init_completion(&ch->ack);
		ret = mbox_send_message(ch->mbox[idx].chan, NULL);
		if (ret < 0)
			dev_dbg(&rproc->dev, "%s: failed (%s, err:%d)\n",
				__func__, ch->mbox[idx].name, ret);

		if (!wait_for_completion_timeout(&ch->ack,
			    msecs_to_jiffies(timeout))) {
			ret = -ENODEV;
			dev_warn(cdev, "probe failed with error %d\n", ret);
			goto free_rproc;
		}
	}

	/* Add SI rproc channel */
	ret = devm_rproc_add(cdev, rproc);
	if (ret) {
		dev_err(cdev, "Failed to add SI rproc\n");
		goto free_rproc;
	}

	ch->rproc = rproc;
	return ch;

free_rproc:
	arm_si_rproc_free_mbox(rproc);
	rproc_free(rproc);
	return ERR_PTR(ret);
}

/*
 * arm_si_rproc_init()
 * @si_rproc: pointer to arm_si_rproc type object
 *
 * Create and initialize SI rproc type object
 *
 * Return: 0 for success and error code for failure.
 */
static int arm_si_rproc_init(struct arm_si_rproc *si_rproc)
{
	struct device *dev = si_rproc->dev;
	struct device_node *dev_node = dev_of_node(dev);
	struct platform_device *child_pdev;
	struct arm_si_channel **chs;
	struct device **child_devs;
	struct device_node *child;
	int channel_count, ret, i;

	/*
	 * Number of channels is decided by number of child nodes of
	 * SI rproc subsystem node in dts
	 */
	channel_count = of_get_available_child_count(dev_node);
	if (channel_count == 0) {
		dev_err(dev, "Invalid number of SI channel %d", channel_count);
		return -EINVAL;
	}

	child_devs = kcalloc(channel_count, sizeof(struct device *),
			     GFP_KERNEL);
	if (!child_devs)
		return -ENOMEM;

	chs = kcalloc(channel_count,
		      sizeof(struct arm_si_channel *), GFP_KERNEL);
	if (!chs) {
		kfree(child_devs);
		return -ENOMEM;
	}

	i = 0;
	for_each_available_child_of_node(dev_node, child) {
		child_pdev = of_find_device_by_node(child);
		if (!child_pdev) {
			of_node_put(child);
			ret = -ENODEV;
			goto release_chs;
		}

		child_devs[i] = &child_pdev->dev;

		/* create and add rproc instance of type struct rproc */
		chs[i] = arm_si_add_rproc_core(&child_pdev->dev);
		if (IS_ERR(chs[i])) {
			of_node_put(child);
			ret = PTR_ERR(chs[i]);
			chs[i] = NULL;
			continue;
		}

		i++;
	}

	si_rproc->channel_count = i;
	si_rproc->chs = chs;

	kfree(child_devs);
	return 0;

release_chs:
	while (i >= 0) {
		if (chs[i]) {
			of_reserved_mem_device_release(chs[i]->dev);
			rproc_del(chs[i]->rproc);
			rproc_free(chs[i]->rproc);
		}
		i--;
	}
	kfree(chs);
	kfree(child_devs);
	return ret;
}

/*
 * arm_si_rproc_exit()
 * @data: pdev data
 *
 * Destroy and deinitialize SI rproc type object
 *
 * Return: 0 for success and error code for failure.
 */
static void arm_si_rproc_exit(void *data)
{
	struct platform_device *pdev = (struct platform_device *)data;
	struct arm_si_rproc *si_rproc;
	struct arm_si_channel *ch;
	int i, ret;

	si_rproc = (struct arm_si_rproc *)platform_get_drvdata(pdev);
	if (!si_rproc)
		return;

	for (i = 0; i < si_rproc->channel_count; i++) {
		ch = si_rproc->chs[i];
		of_reserved_mem_device_release(ch->dev);
		if (ch->rproc->state == RPROC_ATTACHED) {
			ret = rproc_detach(ch->rproc);
			if (ret)
				dev_err(&ch->rproc->dev,
					"Failed to detach rpoc\n");
		}
		rproc_del(ch->rproc);
		arm_si_rproc_free_mbox(ch->rproc);
		destroy_workqueue(ch->wq);
		rproc_free(ch->rproc);
	}

	kfree(si_rproc->chs);
	kfree(si_rproc);
	platform_set_drvdata(pdev, NULL);
}

/*
 * arm_si_rproc_shutdown()
 * @pdev: domain platform device for SI rproc
 *
 * inform the remote processor for shutdown
 *
 * Return: 0 for success and < 0 for failure.
 */
static void arm_si_rproc_shutdown(struct platform_device *pdev)
{
	struct arm_si_rproc *si_rproc;
	struct arm_si_channel *ch;
	int i, ret;

	si_rproc = (struct arm_si_rproc *)platform_get_drvdata(pdev);
	if (!si_rproc)
		return;

	for (i = 0; i < si_rproc->channel_count; i++) {
		ch = si_rproc->chs[i];
		if (ch->rproc->state == RPROC_ATTACHED) {
			ret = rproc_detach(ch->rproc);
			if (ret)
				dev_err(&ch->rproc->dev,
					"Failed to detach rpoc\n");
		}
	}
}

/*
 * arm_si_rproc_probe()
 * @pdev: domain platform device for SI rproc
 *
 * parse device-tree, allocate required resources and rproc ops
 *
 * Return: 0 for success and < 0 for failure.
 */
static int arm_si_rproc_probe(struct platform_device *pdev)
{
	struct arm_si_rproc *si_rproc;
	struct device *dev = &pdev->dev;
	int ret;

	si_rproc = kzalloc(sizeof(*si_rproc), GFP_KERNEL);
	if (!si_rproc)
		return -ENOMEM;

	si_rproc->dev = dev;

	ret = devm_of_platform_populate(dev);
	if (ret) {
		dev_err_probe(dev, ret, "Failed to populate platform dev\n");
		kfree(si_rproc);
		return ret;
	}

	platform_set_drvdata(pdev, si_rproc);

	ret = arm_si_rproc_init(si_rproc);
	if (ret) {
		kfree(si_rproc);
		platform_set_drvdata(pdev, NULL);
		dev_err_probe(dev, ret, "SI rproc init failed\n");
		return ret;
	}

	ret = devm_add_action_or_reset(dev, arm_si_rproc_exit, pdev);
	if (ret)
		return ret;

	return 0;
}

/* Match table for OF platform binding */
static const struct of_device_id arm_si_rproc_match[] = {
	{ .compatible = "arm,si-rproc", },
	{ /* end of list */ },
};
MODULE_DEVICE_TABLE(of, arm_si_rproc_match);

static struct platform_driver arm_si_rproc_driver = {
	.probe = arm_si_rproc_probe,
	.shutdown = arm_si_rproc_shutdown,
	.driver = {
		.name = "arm-si-rproc",
		.of_match_table = arm_si_rproc_match,
	},
};
module_platform_driver(arm_si_rproc_driver);

MODULE_AUTHOR("Rahul Singh <rahul.singh@arm.com>");
MODULE_DESCRIPTION("ARM Safety Island remote processor driver");
MODULE_LICENSE("GPL");
