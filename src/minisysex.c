/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * minisysex.c
 * Program entry point.
 */

#include <stdlib.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/alerts.h>

#include "log.h"
#include "midicamd.h"
#include "midiserial.h"

static const midi_interface_t* interface = NULL;
static const uint8_t mt32_all_params_reset[] = {0xF0, 0x41, 0x10, 0x16, 0x12, 0x7F, 0x00, 0x00, 0x01, 0xF7};

int main(int argc, char** argv)
{
	// Init logger
#ifdef DEBUG
	minisysex_log_init(LOG_DEBUG);
#else
	minisysex_log_init(LOG_INFO);
#endif

	// First try CAMD
	minisysex_log(LOG_DEBUG, "Trying to open camd.library");
	interface = minisysex_camd_get_interface();
	if (!interface->open())
	{
		// Fall back on serial.device
		minisysex_log(LOG_DEBUG, "Trying to open serial.device");
		interface = minisysex_serial_get_interface();
		if (!interface->open())
		{
			minisysex_log(LOG_ERROR, "Couldn't open any MIDI devices");
			return EXIT_FAILURE;
		}
		else
		{
			minisysex_log(LOG_DEBUG, "Using serial.device");
		}
	}
	else
	{
		minisysex_log(LOG_DEBUG, "Using camd.library");
	}

	// Example: MT-32 reset
	interface->send_sysex(mt32_all_params_reset, sizeof(mt32_all_params_reset));

	// All notes off for each part
	for (uint8_t i = 0; i < 8; ++i)
		interface->send_short(0x7CB0 | i);

	interface->close();

	return EXIT_SUCCESS;
}
