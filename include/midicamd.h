/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * midicamd.h
 * CAMD MIDI device support.
 */

#ifndef MINISYSEX_MIDI_CAMD_H
#define MINISYSEX_MIDI_CAMD_H

#include <stdbool.h>
#include <stdint.h>

#include "midi.h"

const midi_interface_t* minisysex_camd_get_interface();

bool minisysex_camd_open_device();
void minisysex_camd_close_device();
bool minisysex_camd_send_short(uint32_t message);
bool minisysex_camd_send_sysex(const uint8_t* bytes, size_t length);

#endif /* MINISYSEX_MIDI_CAMD_H */
