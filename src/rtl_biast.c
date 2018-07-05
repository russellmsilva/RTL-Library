/*
 * rtl-sdr, turns your Realtek RTL2832 based DVB dongle into a SDR receiver
  * Copyright (C) 2012 by Steve Markgraf <steve@steve-m.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include "getopt/getopt.h"
#endif

#include "rtl-sdr.h"

static rtlsdr_dev_t *dev = NULL;

void usage(void)
{
	printf(
		"rtl_biast, a tool: \n"
		"- for turning the RTL-SDR v.3 bias tee ON and OFF \n"
		"- manage antenna switches via the clock card.\n\n"
		"Example to turn on bias tee (power supply of antenna switches) \n"
		"bias tee: rtl_biast -d 0 -b 1 \n"
		"procedure should be applied for all devices with antenna switches \n\n"
		"Example to switch to the RF Input (device_index must be supervisor device) \n"
		"bias tee: rtl_biast -d 0 -a 0x40 -v 0x00\n"
		"Example to switch to the Noise Input (device_index must be supervisor device) \n"
		"bias tee: rtl_biast -d 0 -a 0x40 -v 0x1f\n\n"
		"Usage:\n"
		"\t[-d device_index (default: 0)]\n"
		"\t[-b bias_on (default: 0)]\n"
		"\t[-a i2c_address of the register (default: 0; current clock cards 0x40)]\n"
		"\t[-v i2c_value of the clock card 8-bit register (default: 0)]\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int i, r, opt;
	uint32_t dev_index = 0;
	uint32_t i2c_addr=0;
	uint8_t bias_on = 0;
	uint8_t i2c_value=0;

	while ((opt = getopt(argc, argv, "d:b:a:v:h?")) != -1) {
		switch (opt) {
		case 'd':
			dev_index = atoi(optarg);
			break;
		case 'b':
			bias_on = atoi(optarg);
			break;
		case 'a':
		    i2c_addr=strtol(optarg, NULL, 16);
		printf( "i2c address: %02x \n", i2c_addr);
        		break;
        case 'v':
           		i2c_value=strtol(optarg, NULL, 16);
           		printf("i2c value: %02x \n", i2c_value);
                break;

		default:
			usage();
			break;
		}
	}

	r = rtlsdr_open(&dev, dev_index);


    //set the bias tee by setting the gpio bit 0 to bias_on
	rtlsdr_set_bias_tee(dev, bias_on);

    //set rtlsdr repeater for the i2communication via RTL2832U
    rtlsdr_set_i2c_repeater(dev, 1);
    //set register to the output
    rtlsdr_i2c_write_reg(dev, i2c_addr, 0x03, 00);
    //set value to the register as described in the table
    rtlsdr_i2c_write_reg(dev, i2c_addr, 0x01, i2c_value);
    //close the i2c_repeater
    rtlsdr_set_i2c_repeater(dev, 0);

exit:
	rtlsdr_close_bt(dev);

	return r >= 0 ? r : -r;
}
