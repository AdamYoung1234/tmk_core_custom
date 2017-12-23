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

#include "webusb.h"
#include "webusb_config.h"
#include "lufa.h"
#include <string.h>
#include <stdlib.h>

void webusb_send_url_descriptor(int index) {
    if(index > NUM_ALLOWED_ORIGINS || index < 1)
        return;

    uint8_t descriptor_length = strlen(ALLOWED_ORIGINS[index - 1].url) + 3;
    uint8_t* descriptor = (uint8_t*)malloc(sizeof(uint8_t) * descriptor_length);

    if(descriptor != NULL) {
        descriptor[0] = descriptor_length;
        descriptor[1] = WEBUSB_URL;
        descriptor[2] = ALLOWED_ORIGINS[index - 1].prefix;

        for(uint8_t i=0; i<descriptor_length - 3; i++) {
            descriptor[3 + i] = (uint8_t)ALLOWED_ORIGINS[index - 1].url[i];
        }
        Endpoint_Write_Control_Stream_LE(descriptor, descriptor_length);
        Endpoint_ClearOUT();
    }
    free(descriptor);
}

void webusb_send_allowed_origins_descriptor() {
    uint8_t descriptor_length = 12 + NUM_ALLOWED_ORIGINS;
    uint8_t descriptor[12 + NUM_ALLOWED_ORIGINS];
    //Allowed Origins Header
    descriptor[0] = 0x05;   //length
    descriptor[1] = WEBUSB_DESCRIPTOR_SET_HEADER;
    descriptor[2] = 0x0C + NUM_ALLOWED_ORIGINS; //total length low
    descriptor[3] = 0x00;   //totoal length high
    descriptor[4] = 0x01;   ///number of configuration subset headers
    //Configuration Subset Header
    descriptor[5] = 0x04;   //length
    descriptor[6] = WEBUSB_CONFIGURATION_SUBSET_HEADER; //descriptor typle
    descriptor[7] = 0x01;   //Configuration to which this section applies
    descriptor[8] = 0x01; //Number of function subset headers following this descriptor
    //Fucntion Subset Header
    descriptor[9] = 0x03 + NUM_ALLOWED_ORIGINS; //length
    descriptor[10] = WEBUSB_FUNCTION_SUBSET_HEADER;  //descriptor type
    descriptor[11] = WEBUSB_INTERFACE;  //Interface number
    //allowed origins
    for(int i=1; i<=NUM_ALLOWED_ORIGINS; i++) {
        descriptor[11 + i] = i;
    }

    Endpoint_Write_Control_Stream_LE(descriptor, descriptor_length);
    Endpoint_ClearOUT();
}

void receiveWebUSBData(void) {
    uint8_t ep = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(WEBUSB_OUT_EPNUM);
    /* Check to see if a packet has been sent from the host */
    if (Endpoint_IsOUTReceived()) {
        /* Check to see if the packet contains data */
        if (Endpoint_IsReadWriteAllowed()) {
            /* Create a temporary buffer to hold the read in report from the host */
            uint8_t ConsoleData[WEBUSB_EPSIZE];

            /* Read Console Report Data */
            Endpoint_Read_Stream_LE(&ConsoleData, sizeof(ConsoleData), NULL);

            /* Process Console Report Data */
            console_parse_raw_data(ConsoleData);
        }

        /* Finalize the stream transfer to send the last packet */
        Endpoint_ClearOUT();
    }

    Endpoint_SelectEndpoint(ep);
}

void webusb_send_in_data(uint8_t command_id, uint8_t has_crc_check , uint8_t payload_length, uint8_t more_packet, 
                         uint8_t payload[], uint8_t crc_l, uint8_t crc_h) {
    uint8_t current_ep =  Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(WEBUSB_IN_EPNUM);

    if (!Endpoint_IsEnabled() || !Endpoint_IsConfigured()) {
        Endpoint_SelectEndpoint(current_ep);
        return;
    }

    //if(Endpoint_IsINReady()) {
    //Endpoint_ClearIN();
    //}

    if(Endpoint_IsReadWriteAllowed()) {
        Endpoint_Write_8((has_crc_check << CRC_CHECK_POS) + command_id);
        Endpoint_Write_8((more_packet << MORE_PACKET_POS) + payload_length);

        for(int i=0; i<payload_length; i++) {
            Endpoint_Write_8(payload[i]);
        }

        Endpoint_Write_8(crc_l);
        Endpoint_Write_8(crc_h);

        while(Endpoint_IsReadWriteAllowed()) {
            Endpoint_Write_8(0);
        }

        if (Endpoint_IsINReady()) {
            Endpoint_ClearIN();
        }
    }

    Endpoint_SelectEndpoint(current_ep);
}

