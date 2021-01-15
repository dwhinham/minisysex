/**
 * minisysex
 * Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>
 *
 * log.h
 * Error/debug logging.
 */

#ifndef MINISYSEX_LOG_H
#define MINISYSEX_LOG_H

typedef enum
{
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
} log_level_t;

void minisysex_log_init(log_level_t level);
void minisysex_log(log_level_t level, const char* message, ...);

#endif /* MINISYSEX_LOG_H */
