/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Tesla Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _SOC_TURBO_TRAV_GPT_H_
#define _SOC_TURBO_TRAV_GPT_H_

void get_gpt_part_uuid_name(unsigned char *buf, int partno, char *uuid_str, int uuid_len,
			    char *name_str, int name_len);

#endif /* _SOC_TURBO_TRAV_GPT_H_ */
