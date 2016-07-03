/*
 * relayctl.c
 *
 * Simple command-line control of SainSmart USB relay boards
 * based on FTDI chips using bitbang mode.
 *
 * Copyright (c) 2015 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ftdi.h>
#include <string.h>

static void usage() {
    fprintf(stdout, "Usage: relayctl <relay> <state> [sernum]\n");
    fprintf(stdout, "  relay = 1-8\n");
    fprintf(stdout, "  state = 0 or 1\n");
    fprintf(stdout, "  sernum (optional) = 8-digit USB serial number\n");
}

static struct ftdi_context *ftdi = 0;
int vid = 0x0403;
int pid = 0x6001;
char serial_num[] = "00000000";

static int get_device()
{
    int ret = 0;

    if ((ftdi = ftdi_new()) == 0) {
        fprintf(stderr, "relayctl: libftdi init failed\n");
        return 0;
    }
    if (0 == strcmp(serial_num, "00000000")) {
	// serial number arg not specified, find
	// first device with matching vid and pid
	ret = ftdi_usb_open(ftdi, vid, pid);
    }
    else {
	// serial number arg was specified, find
	// device using a descriptor string
	char desc_str[26];
	snprintf(desc_str, sizeof(desc_str)-1, "s:0x%.04X:0x%.04X:%8s", vid, pid, serial_num);
	ret = ftdi_usb_open_string(ftdi, desc_str);
    }

    if (ret < 0) {
        fprintf(stderr, "relayctl: unable to open device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
	ftdi_free(ftdi);
	ftdi = 0;
        return 0;
    }

    return 1;
}

int relayctl(int relay, int state)
{
    int ret;

    ret = ftdi_set_bitmode(ftdi, 0xFF, BITMODE_SYNCBB);
    if (ret < 0) {
	fprintf(stderr, "relayctl: set mode failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
	return 0;
    }
    unsigned char pins;
    ret = ftdi_read_pins(ftdi, &pins);
    if (ret < 0) {
	fprintf(stderr, "relayctl: read data failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
	return 0;
    }
    if (state)
	pins |= (1 << (relay-1));
    else
	pins &= ~(1 << (relay-1));
    ret = ftdi_write_data(ftdi, &pins, 1);
    if (ret < 0) {
	fprintf(stderr, "relayctl: write data failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
	return 0;
    }
    return 1;
}

int main(int argc, char* argv[])
{
    int ret = EXIT_FAILURE;

    if (argc < 3) {
        fprintf(stderr, "relayctl: required args missing\n");
        usage();
        goto out;
    }

    int relay = atol(argv[1]);
    int state = atol(argv[2]) & 1;

    if (relay < 1 | relay > 8) {
	fprintf(stderr, "relayctl: relay number %d out of range\n", relay);
	usage();
	goto out;
    }

    if (argc > 3) {
	// get serial number arg
	// must be exactly 8 chars
	if (8 == strlen(argv[3]))
	    strncpy(serial_num, argv[3], 8);
	else
	    fprintf(stderr, "relayctl: specified serial number is invalid, ignoring\n");
    }

    if (!get_device()) {
        fprintf(stderr, "relayctl: no USB device found\n");
	goto out;
    }

    fprintf(stdout, "relayctl: Turning relay %d %s\n", relay, state ? "ON":"OFF");

    if (!relayctl(relay, state)) {
	fprintf(stderr, "relayctl: set relay state failed\n");
	goto out;
    }

    ret = EXIT_SUCCESS;

out:
    if (ftdi){
	ftdi_usb_close(ftdi);
	ftdi_free(ftdi);
	ftdi = 0;
    }
    return ret;
}
