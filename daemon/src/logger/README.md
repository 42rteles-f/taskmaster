# logger

Small reusable POSIX C logger for 42 projects.

## Build and test

```sh
make
make test
```

The test creates `test-taskmaster.log` in the current directory.

## E.G Use from Taskmaster

Include `logger.h`, compile `src/logger.c` into your project (or link `liblogger.a`),
then initialize the logger before writing messages:

```c
if (logger_init("logs/taskmaster.log") != 0) {
    /* Report startup failure to stderr; do not call the logger here. */
    perror("logger_init");
    return 1;
}
logger_set_level(LOG_LEVEL_INFO);
LOG_INFO("Taskmaster started");
```

The `logs` directory must already exist. Decide explicitly whether failure to open the
required log file should abort Taskmaster startup; the subject requires event logging
to a local file, so failing startup is a defensible policy.

Call `logger_close()` during orderly shutdown.

## Design notes

- Uses only C/POSIX facilities; no third-party dependencies.
- Opens in append mode and flushes each message so recent events are written promptly.
- The logger returns errors instead of exiting the process.
- Keep logger use in the parent Taskmaster process initially. Do not reuse an inherited
  `FILE *` from a forked child; initialize logging independently if child-side logging is
  ever required.
- This initial version is intended for a single process and is not a cross-process logging
  service. Add explicit synchronization or a dedicated logging process if multiple
  processes need to write to the same file.
- The API is deliberately small. Consider rotation, PID fields, and configurable output
  only if the project needs them.
