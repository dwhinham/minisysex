/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * midicamd.c
 * serial.device MIDI device support.
 */

/* FIXME: Kludge to get CAMD includes working with -mcrt=nix13 */
#include <exec/types.h>
typedef ULONG Tag;

#include <midi/mididefs.h>
#include <proto/camd.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include "log.h"
#include "midicamd.h"

struct Library* CamdBase = NULL;
static struct MidiNode* midi_node = NULL;
static struct MidiLink* midi_link = NULL;

const midi_interface_t* minisysex_camd_get_interface()
{
	static const midi_interface_t midi_interface =
	{
		.open = minisysex_camd_open_device,
		.close = minisysex_camd_close_device,
		.send_short = minisysex_camd_send_short,
		.send_sysex = minisysex_camd_send_sysex
	};

	return &midi_interface;
}

bool minisysex_camd_open_device()
{
	if (!(CamdBase = OpenLibrary("camd.library", 0L)))
	{
		minisysex_log(LOG_DEBUG, "Failed to open camd.library");
		return false;
	}

	if (!(midi_node = CreateMidi(MIDI_MsgQueue, 2048, MIDI_SysExSize, 10000L, TAG_END)))
	{
		minisysex_log(LOG_DEBUG, "Failed to create MIDI node");
		return false;
	}

	midi_link = AddMidiLink(
		midi_node,
		MLTYPE_Sender,
		MLINK_Name, (ULONG) "minisysex.out",
		MLINK_Location, (ULONG) "out.0",
		TAG_END
	);

	if (!midi_link)
	{
		minisysex_log(LOG_DEBUG, "Failed to create MIDI link");
		return false;
	}

	return true;
}

void minisysex_camd_close_device()
{
	if (midi_link)
	{
		RemoveMidiLink(midi_link);
		midi_link = NULL;
	}

	if (midi_node)
	{
		DeleteMidi(midi_node);
		midi_node = NULL;
	}
}

bool minisysex_camd_send_short(uint32_t message)
{
	MidiMsg midi_message =
	{
		.mm_Status = message & 0xFF,
		.mm_Data1 = message >> 8 & 0xFF,
		.mm_Data2 = message >> 16 & 0xFF,
		.mm_Port = 0
	};

	PutMidi(midi_link, midi_message.mm_Msg);
	return true;
}

bool minisysex_camd_send_sysex(const uint8_t* bytes, size_t length)
{
	if (length == 0 || bytes[0] != MS_SysEx || bytes[length - 1] != MS_EOX)
		return false;

	PutSysEx(midi_link, (UBYTE*) bytes);
	return true;
}
