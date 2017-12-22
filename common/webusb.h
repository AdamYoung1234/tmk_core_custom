/*
Copyright 2016 Chen Yang <adamyoung.2333@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _WEBUSB_H
#define _WEBUSB_H

#include <stdint.h>
#include "webusb_config.h"
#include "descriptor.h"

/*******************************************************************************
 * WebUSB
 ******************************************************************************/

#define DTYPE_BOS                             0x0F
#define WEBUSB_VENDOR_CODE                    0x01
#define MS_DESCRIPTOR_VENDOR_CODE             0x02
#define DTYPE_DEVICE_CAPABILITY               0x10
#define BOS_PLATFORM_CAPABILITY_DESCRIPTOR    0x05

//WebUSB Request codes
#define WEBUSB_REQUEST_GET_ALLOWED_ORIGINS    1
#define WEBUSB_REQUEST_GET_URL                2

//WebUSB descriptor type
#define WEBUSB_DESCRIPTOR_SET_HEADER          0
#define WEBUSB_CONFIGURATION_SUBSET_HEADER    1
#define WEBUSB_FUNCTION_SUBSET_HEADER         2
#define WEBUSB_URL                            3

void send_webusb_url_descriptor(int index);
void send_webusb_allowed_origins_descriptor(void);

#endif