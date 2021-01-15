/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * log.c
 * Error/debug logging.
 */

#include <stdarg.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "log.h"

static log_level_t log_level = LOG_ERROR;
static BPTR output_handle = NULL;

// d0: new character
// a3: PutChData (destination buffer)
static void put_ch_proc()
{
	__asm("move.b d0,(a3)+");
}

void minisysex_log_init(log_level_t level)
{
	log_level = level;
	output_handle = Output();
}

void minisysex_log(log_level_t level, const char* message, ...)
{
	if (!output_handle || level < log_level)
		return;

	char buffer[512];
	va_list args;
	va_start(args, message);

	RawDoFmt(message, args, (VOID*)put_ch_proc, buffer);

	LONG length = 0;
	while (buffer[length])
		++length;

	buffer[++length] = '\n';
	buffer[++length] = '\0';
	Write(output_handle, buffer, length);
}
