/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * midiserial.c
 * MIDI device support.
 */

#include <devices/serial.h>
#include <proto/alib.h>
#include <proto/exec.h>

#include "log.h"
#include "midiserial.h"

#define MIDI_BAUD_RATE 31250L

static struct MsgPort* msg_port = NULL;
static struct IOExtSer* io_serial = NULL;

static inline bool is_channel(uint8_t status)			{ return status >= 0x80 && status <= 0xEF; }
static inline bool is_system_exclusive(uint8_t status) 	{ return status == 0xF0; }
static inline bool is_system_common(uint8_t status)		{ return status >= 0xF1 && status <= 0xF7; }

static inline uint8_t num_data_bytes(uint8_t status)
{
	if (is_channel(status))
	{
		// Program change or channel pressure/aftertouch
		if (status >= 0xC0 && status <= 0xDF)
			return 1;

		return 2;
	}

	if (is_system_common(status))
	{
		switch (status)
		{
			case 0xF1:
			case 0xF3:
				return 1;

			case 0xF2:
				return 2;
		}
	}

	return 0;
}

const midi_interface_t* minisysex_serial_get_interface()
{
	static const midi_interface_t midi_interface =
	{
		.open = minisysex_serial_open_device,
		.close = minisysex_serial_close_device,
		.send_short = minisysex_serial_send_short,
		.send_sysex = minisysex_serial_send_sysex
	};

	return &midi_interface;
}

bool minisysex_serial_open_device()
{
	if (!(msg_port = CreatePort(NULL, 0L)))
	{
		minisysex_log(LOG_DEBUG, "Failed to create port");
		return false;
	}

	if (!(io_serial = (struct IOExtSer*) CreateExtIO(msg_port, sizeof(struct IOExtSer))))
	{
		minisysex_log(LOG_DEBUG, "Failed to create I/O request");
		return false;
	}

	if (OpenDevice(SERIALNAME, 0L, (struct IORequest*) io_serial, 0L))
	{
		minisysex_log(LOG_DEBUG, "Failed to open " SERIALNAME);
		return false;
	}

	io_serial->io_Baud = MIDI_BAUD_RATE;
	io_serial->io_SerFlags = SERF_RAD_BOOGIE | SERF_XDISABLED;
	io_serial->io_WriteLen = 8;
	io_serial->io_StopBits = 1;
	io_serial->IOSer.io_Command = SDCMD_SETPARAMS;
	DoIO((struct IORequest*) io_serial);

	return true;
}

void minisysex_serial_close_device()
{
	if (io_serial)
	{
		if (io_serial->IOSer.io_Device)
			CloseDevice((struct IORequest*) io_serial);

		DeleteExtIO((struct IORequest*) io_serial);
		io_serial = NULL;
	}

	if (msg_port)
	{
		DeletePort(msg_port);
		msg_port = NULL;
	}
}

bool minisysex_serial_send_short(uint32_t message)
{
	UBYTE midi_message[3] =
	{
		message & 0xFF,			// Status byte
		message >> 8 & 0xFF,	// Data byte 1
		message >> 16 & 0xFF	// Data byte 2
	};

	if (is_system_exclusive(midi_message[0]))
		return false;

	io_serial->IOSer.io_Length = (ULONG) num_data_bytes(midi_message[0]) + 1;
	io_serial->IOSer.io_Data = (APTR) midi_message;
	io_serial->IOSer.io_Command = CMD_WRITE;
	DoIO((struct IORequest*) io_serial);
	return true;
}

bool minisysex_serial_send_sysex(const uint8_t* bytes, size_t length)
{
	if (!length || bytes[0] != 0xF0 || bytes[length - 1] != 0xF7)
		return false;

	io_serial->IOSer.io_Length = (ULONG) length;
	io_serial->IOSer.io_Data = (APTR) bytes;
	io_serial->IOSer.io_Command = CMD_WRITE;
	DoIO((struct IORequest*) io_serial);
	return true;
}
