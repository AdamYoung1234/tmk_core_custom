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
#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include <stdint.h>
#include <stdbool.h>

#define CONSOLE_EP_SIZE 32
#define CRC_CHECK_MASK 0b01111111
#define MORE_PACKET_MASK 0b01111111
#define CRC_CHECK_POS 7
#define MORE_PACKET_POS 7
#define CONSOLE_IN_BUFFER_SIZE 64

typedef struct
{
    uint8_t  command_id;
    uint8_t  length;
    uint8_t  *payload;
    uint8_t  more_packet;
    uint8_t  has_crc_check;
    uint16_t crc;
} console_data_t;

enum console_command_id
{
    CONSOLE_COMMAND_EEP_FILE = 0x10,
    CONSOLE_COMMAND_DEBUG_PRINT = 0x21
};

bool console_parse_raw_data(uint8_t raw_data[]);
bool console_crc_validate(console_data_t console_data, uint16_t crc);
bool console_parse_command_data(console_data_t console_data);
bool console_parse_eep_file(console_data_t console_data);
void console_print_raw_data(console_data_t console_data);

#endif
