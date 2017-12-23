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
#include "console_command.h"
#include <avr/eeprom.h>
#include "debug.h"

bool console_parse_raw_data(uint8_t raw_data[]) {
    //process raw data
    uint8_t command_id = raw_data[0];
    uint8_t length = raw_data[1];
    uint8_t payload[length];
    uint8_t has_crc_check = command_id >> CRC_CHECK_POS;
    uint8_t more_packet = length >> MORE_PACKET_POS;

    command_id &= MORE_PACKET_MASK;
    length &= CRC_CHECK_MASK;
    //assemble crc data
    uint16_t crc = 0;
    if(has_crc_check) {
        crc = (raw_data[length + 3] << 8) + (raw_data[length + 3]);
    } else {
        crc = 0;
    }

    for(uint8_t i=2, j=0; j<length; i++,j++) {
        payload[j] = raw_data[i];
    }

    console_data_t console_data_body;
    console_data_body.command_id = command_id;
    console_data_body.more_packet = more_packet;
    console_data_body.length = length;
    console_data_body.payload = payload;
    console_data_body.has_crc_check = has_crc_check;
    console_data_body.crc = crc;

    if(!console_crc_validate(console_data_body, crc)) {
        return false;
    }

    console_parse_command_data(console_data_body);
    return true;
}

bool console_crc_validate(console_data_t console_data, uint16_t crc) {
    return true;
}

bool console_parse_command_data(console_data_t console_data)
{
    switch(console_data.command_id) {
        case CONSOLE_COMMAND_EEP_FILE:
            console_parse_eep_file(console_data);
            break;

        default:
            print("command id is invalid\n");
            break;
    }
    return true;
}

bool console_parse_eep_file(console_data_t console_data)
{
    console_print_raw_data(console_data);

    static uint16_t address = 0x0;
    static bool more_packet = false;
    uint8_t real_data_pos = 0;

    if(console_data.more_packet && (!more_packet)) {
        address = (console_data.payload[1] << 8) + console_data.payload[0];
        real_data_pos = 2;
        more_packet = true;
    } else if(!console_data.more_packet && more_packet) {
        more_packet = false;
    } else if(!console_data.more_packet && !more_packet) {
        address = (console_data.payload[1] << 8) + console_data.payload[0];
        real_data_pos = 2;
    }

    //write data
    for(uint8_t i=0; i<console_data.length-real_data_pos; i++) {
        eeprom_update_byte((uint8_t *)address, console_data.payload[i+real_data_pos]);
        address++;
    }
    return true;
}

void console_print_raw_data(console_data_t console_data)
{
    print("======= data =======\n");
    print("command id: ");
    phex(console_data.command_id);
    print("\n");
    print("length: ");
    phex(console_data.length);
    print("\n");
    print("more packet: ");
    phex(console_data.more_packet);
    print("\n");
    print("has CRC: ");
    phex(console_data.has_crc_check);
    print("\n");
    print("data: ");
    for(uint8_t i=0; i<console_data.length; i++) {
        phex(console_data.payload[i]);
        print(" ");
    }
    print("\n");
    print("====================\n");
}
