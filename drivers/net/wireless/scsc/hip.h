/*****************************************************************************
 *
 * Copyright (c) 2012 - 2016 Samsung Electronics Co., Ltd. All rights reserved
 *
 ****************************************************************************/

#ifndef __SLSI_HIP_H__
#define __SLSI_HIP_H__

#include <linux/types.h>
#include <linux/device.h>

struct slsi_dev;

/* This structure describes the chip and HIP core lib
 * information that exposed to the OS layer.
 */
struct slsi_card_info {
	u16 chip_id;
	u32 fw_build;
	u16 fw_hip_version;
	u32 sdio_block_size;
};

/* HIP States:
 *     STOPPED  : (default) state, avoid running the HIP
 *     STARTING : HIP is being initialised, avoid running the HIP
 *     STARTED  : HIP cycles can run
 *     STOPPING : HIP is being de-initialised, avoid running the HIP
 *     BLOCKED  : HIP TX CMD53 failure or WLAN subsystem crashed indication from Hydra,
 *                                                                avoid running the HIP
 */
enum slsi_hip_state {
	SLSI_HIP_STATE_STOPPED,
	SLSI_HIP_STATE_STARTING,
	SLSI_HIP_STATE_STARTED,
	SLSI_HIP_STATE_STOPPING,
	SLSI_HIP_STATE_BLOCKED
};

struct slsi_hip {
	struct slsi_dev       *sdev;
	struct slsi_card_info card_info;
	/* a std mutex */
	struct mutex          hip_mutex;

	/* refer to enum slsi_hip_state */
	atomic_t              hip_state;
};

#define SLSI_HIP_PARAM_SLOT_COUNT 2

/****************************** Configuration ********************************/
struct slsi_hip_card_params {
	u32 zero_copy;
	u32 traffic_qlen;
	u32 th_max_slots;
	u32 fh_max_slots;
	u32 led_mask;
	u32 poll_period;
	u32 tx_window_segment_size;
	u32 tx_force_pull_mode;
	s32 mparam_slot_count[SLSI_HIP_PARAM_SLOT_COUNT];
	u32 minVifDurationEapol;
	u32 minVifDurationDhcp;
	u32 minVifDurationArp;
	u16 minVifDurationPacketFilter;
};

int slsi_hip_init(struct slsi_dev *sdev, struct device *dev);
void slsi_hip_deinit(struct slsi_dev *sdev);

int slsi_hip_start(struct slsi_dev *sdev, struct slsi_hip_card_params *hip_param);
int slsi_hip_setup(struct slsi_dev *sdev);
int slsi_hip_stop(struct slsi_dev *sdev);

/* Forward declaration */
struct sap_api;
struct sk_buff;

/* Register SAP with HIP layer */
int slsi_hip_sap_register(struct sap_api *sap_api);
/* Unregister SAP with HIP layer */
int slsi_hip_sap_unregister(struct sap_api *sap_api);
/* SAP rx proxy */
int slsi_hip_rx(struct slsi_dev *sdev, struct sk_buff *skb);
/* SAP setup once we receive SAP versions */
int slsi_hip_sap_setup(struct slsi_dev *sdev);
/* Allow the SAP to act on a buffer in the free list. */
int slsi_hip_tx_done(struct slsi_dev *sdev, u16 colour);

#endif
