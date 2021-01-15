/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * midi.h
 * MIDI device interface.
 */

#ifndef MINISYSEX_MIDI_H
#define MINISYSEX_MIDI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
	bool (*open)();
	void (*close)();
	bool (*send_short)(uint32_t);
	bool (*send_sysex)(const uint8_t*, size_t);
} midi_interface_t;

#endif /* MINISYSEX_MIDI_H */
