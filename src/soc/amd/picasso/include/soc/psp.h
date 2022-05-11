/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_PSP_H__
#define __PICASSO_PSP_H__

#include <types.h>

struct soc_debug_info {
	u32 version;
	u32 mp0_c2pmsg_0;
	u32 mp0_c2pmsg_1;
	u32 mp0_c2pmsg_2;
	u32 mp0_c2pmsg_3;
	u32 mp0_c2pmsg_4;
	u32 mp0_c2pmsg_5;
	u32 mp0_c2pmsg_6;
	u32 mp0_c2pmsg_7;
	u32 mp0_c2pmsg_8;
	u32 mp0_c2pmsg_9;
	u32 mp0_c2pmsg_10;
	u32 mp0_c2pmsg_11;
	u32 mp0_c2pmsg_12;
	u32 mp0_c2pmsg_13;
	u32 mp0_c2pmsg_14;
	u32 mp0_c2pmsg_15;
};

int soc_get_debug_info(struct soc_debug_info *info);

#endif /* __PICASSO_PSP_H__ */
