#define _POSIX_C_SOURCE 200809L

#include "logger.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

static FILE *g_log_file = NULL;
static t_log_level g_min_level = LOG_LEVEL_INFO;
static FILE *g_console = NULL;
static int g_initialized = 0;
static int g_disabled = 0;

static const char *logger_level_name(t_log_level level)
{
	switch (level)
	{
	case LOG_LEVEL_DEBUG:
		return "DEBUG";
	case LOG_LEVEL_INFO:
		return "INFO";
	case LOG_LEVEL_WARN:
		return "WARN";
	case LOG_LEVEL_ERROR:
		return "ERROR";
	case LOG_LEVEL_FATAL:
		return "FATAL";
	case LOG_LEVEL_OFF:
		return "OFF";
	default:
		return "UNKNOWN";
	}
}

int logger_init(const t_logger_config *config)
{
	FILE *file;
	FILE *console;

	if (config == NULL || config->output < LOG_OUTPUT_NONE || config->output > LOG_OUTPUT_BOTH || config->min_level < LOG_LEVEL_OFF || config->min_level > LOG_LEVEL_FATAL)
	{
		errno = EINVAL;
		return -1;
	}

	file = NULL;
	console = NULL;

	if (config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH)
	{
		if (config->file_path == NULL || config->file_path[0] == '\0')
		{
			errno = EINVAL;
			return -1;
		}

		if (config->output == LOG_OUTPUT_FILE || config->output == LOG_OUTPUT_BOTH)
		{
			if (logger_create_directory() == -1)
				return (-1);

			char final_file_path[1024];

			int dirPathResult = snprintf(
				final_file_path,
				sizeof(final_file_path),
				"logs/%s",
				config->file_path);

			if (dirPathResult < 0 || (size_t)dirPathResult >= sizeof(final_file_path))
			{
				errno = ENAMETOOLONG;
				return (-1);
			}

			file = fopen(final_file_path, "a");
			if (file == NULL)
				return (-1);
		}
	}

	if (config->output == LOG_OUTPUT_STDOUT)
		console = stdout;
	else if (config->output == LOG_OUTPUT_STDERR || config->output == LOG_OUTPUT_BOTH)
		console = stderr;

	if (g_log_file != NULL)
		fclose(g_log_file);

	g_log_file = file;
	g_console = console;
	g_min_level = config->min_level;
	g_disabled = (config->output == LOG_OUTPUT_NONE);
	g_initialized = 1;

	return 0;
}

void logger_close(void)
{
	if (g_log_file != NULL)
	{
		(void)fflush(g_log_file);
		(void)fclose(g_log_file);
		g_log_file = NULL;
	}
}

int logger_log(t_log_level level, const char *format, ...)
{
	time_t now;
	struct tm local_time;
	char timestamp[32];
	va_list args;
	va_list args_copy;
	FILE *outputs[2];
	int output_count;
	int i;
	int result;

	if (level <= LOG_LEVEL_OFF || level > LOG_LEVEL_FATAL || format == NULL)
	{
		errno = EINVAL;
		return (-1);
	}
	if (g_disabled || level < g_min_level)
		return (0);

	output_count = 0;
	if (g_console != NULL)
		outputs[output_count++] = g_console;
	if (g_log_file != NULL)
		outputs[output_count++] = g_log_file;

	if (output_count == 0)
	{
		errno = EBADF;
		return (-1);
	}

	now = time(NULL);
	if (now == (time_t)-1 || localtime_r(&now, &local_time) == NULL)
	{
		errno = EIO;
		return (-1);
	}

	if (strftime(timestamp, sizeof(timestamp),
				 "%Y-%m-%d %H:%M:%S", &local_time) == 0)
	{
		errno = EOVERFLOW;
		return (-1);
	}

	va_start(args, format);
	for (i = 0; i < output_count; i++)
	{
		if (fprintf(outputs[i], "[%s] %-5s ",
					timestamp, logger_level_name(level)) < 0)
		{
			va_end(args);
			return (-1);
		}

		va_copy(args_copy, args);
		result = vfprintf(outputs[i], format, args_copy);
		va_end(args_copy);

		if (result < 0 || fputc('\n', outputs[i]) == EOF || fflush(outputs[i]) == EOF)
		{
			va_end(args);
			return (-1);
		}
	}
	va_end(args);
	return (0);
}

int logger_create_directory()
{
	struct stat st;

	if (mkdir("logs", 0755) == 0)
		return (0);

	if (errno == EEXIST)
	{
		if (stat("logs", &st) == 0 && S_ISDIR(st.st_mode))
			return (0);

		errno = ENOTDIR;
	}

	return (-1);
}
