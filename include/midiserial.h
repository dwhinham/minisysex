/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * midiserial.h
 * serial.device MIDI device support.
 */

#ifndef MINISYSEX_MIDI_SERIAL_H
#define MINISYSEX_MIDI_SERIAL_H

#include <stdbool.h>
#include <stdint.h>

#include "midi.h"

const midi_interface_t* minisysex_serial_get_interface();

bool minisysex_serial_open_device();
void minisysex_serial_close_device();
bool minisysex_serial_send_short(uint32_t message);
bool minisysex_serial_send_sysex(const uint8_t* bytes, size_t length);

#endif /* MINISYSEX_MIDI_SERIAL_H */
