#ifndef LOGGER_H
#define LOGGER_H

typedef enum e_log_level
{
    LOG_LEVEL_OFF = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} t_log_level;

typedef enum e_logger_output
{
    LOG_OUTPUT_NONE = 0,
    LOG_OUTPUT_STDOUT,
    LOG_OUTPUT_STDERR,
    LOG_OUTPUT_FILE,
    LOG_OUTPUT_BOTH
} t_logger_output;

typedef struct s_logger_config
{
    t_logger_output output;
    const char *file_path;
    t_log_level min_level;
} t_logger_config;

/*
 * Opens path in append mode.
 * Returns 0 on success, -1 on failure (errno is preserved by fopen).
 * Call from the main process during startup, before logging.
 */
int logger_init(const t_logger_config *config);

/* Flushes and closes the log file. Safe to call more than once. */
void logger_close(void);

/* Sets the minimum level. Messages below it are discarded. */
void logger_set_level(t_log_level level);

/*
 * Writes a timestamped message. Returns 0 on success, -1 on I/O error
 * or if the logger has not been initialized.
 * format follows printf conventions.
 */
int logger_log(t_log_level level, const char *format, ...);

int logger_create_directory();

#define LOG_DEBUG(...) logger_log(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) logger_log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) logger_log(LOG_LEVEL_WARN, __VA_ARGS__)
#define LOG_ERROR(...) logger_log(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) logger_log(LOG_LEVEL_FATAL, __VA_ARGS__)

#endif
