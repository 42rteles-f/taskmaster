#include "logger.h"

int main(void)
{
    t_logger_config config = {
        LOG_OUTPUT_BOTH,
        "taskmaster.log",
        LOG_LEVEL_DEBUG};

    if (logger_init(&config) != 0)
        return 1;

    LOG_INFO("Taskmaster started");
    LOG_DEBUG("Loading configuration: %s", "taskmaster.conf");
    LOG_WARN("Worker %d is taking too long", 3);
    LOG_ERROR("Could not start worker: %s", "permission denied");

    logger_close();
    return 0;
}
