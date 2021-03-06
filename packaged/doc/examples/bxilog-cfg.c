#include <unistd.h>
#include <sysexits.h>

#include <bxi/base/err.h>
#include <bxi/base/log.h>

// Create a logger for my module/main
SET_LOGGER(MY_LOGGER, "my.logger");
// And other loggers to play with
SET_LOGGER(LOGGER_A, "a.logger");
SET_LOGGER(LOGGER_AB, "a.b.logger");
SET_LOGGER(LOGGER_AC, "a.c.logger");

char ** LEVEL_NAMES = NULL;

void log_stuff(bxilog_logger_p logger) {
    PANIC(logger, "A panic message");
    ALERT(logger, "An alert message");
    CRITICAL(logger, "A critical message");
    ERROR(logger, "An error message");
    WARNING(logger, "A warning message");
    NOTICE(logger, "A notice message");
    OUT(logger, "A normal message");
    INFO(logger, "An info message");
    DEBUG(logger, "A debug message");
    FINE(logger, "A fine message");
    TRACE(logger, "A trace message");
    LOWEST(logger, "A lowest message");

    OUT(logger,
        "A log message with a NULL character between the following "
        "two quotes: '%c', this is after the quotes", 0);
    OUT(logger, "A multiline message\n\tNext line");
}

void display_loggers(size_t n, bxilog_logger_p loggers[n]) {

    for (size_t i = 0; i < n; i++) {
        OUT(MY_LOGGER, "%s: %s",
            loggers[i]->name,
            LEVEL_NAMES[loggers[i]->level]);
    }
}

int main(int argc, char** argv) {
    if (argc != 1) exit(EX_SOFTWARE);

    // Produce logs on stdout/stderr, and also in /tmp/foo.log
    bxilog_config_p config = bxilog_basic_config(argv[0], "/tmp/foo.log",
                                                 BXI_TRUNC_OPEN_FLAGS,
                                                 BXILOG_FILTERS_ALL_ALL);
    bxierr_p err = bxilog_init(config);
    // Use BXILOG_REPORT for error reporting, the error is destroyed.
    // If the logging library raises an error,
    // it has not been initialized. In such a case, the implementation takes care of that
    // and display a message on stderr.
    BXILOG_REPORT(MY_LOGGER, BXILOG_CRITICAL, err,
                  "Can't initialize the BXI Logging library");

    // Fetching log level names
    size_t n = 0;
    n = bxilog_level_names(&LEVEL_NAMES);
    // Use BXIASSERT() instead of assert(), this guarantee all logs
    // are flushed before exiting.
    BXIASSERT(MY_LOGGER, n > 0 && NULL != LEVEL_NAMES);

    // Fetching all registered loggers
    bxilog_logger_p *loggers = {NULL};
    n = bxilog_registry_getall(&loggers);
    BXIASSERT(MY_LOGGER, n>0 && NULL != loggers);

    OUT(MY_LOGGER, "Before configuration:");
    display_loggers(n, loggers);
    log_stuff(LOGGER_A);
    log_stuff(LOGGER_AB);
    log_stuff(LOGGER_AC);

    // Logger's level is determined according to all handlers filters.
    // TODO: illustrate how to change the configuration at runtime
//    bxilog_config_parse_set_filters(":lowest,a:output,a.b:warning");
    OUT(MY_LOGGER, "After configuration:");
    display_loggers(n, loggers);
    log_stuff(LOGGER_A);
    log_stuff(LOGGER_AB);
    log_stuff(LOGGER_AC);

    err = bxilog_finalize(true);
    BXILOG_REPORT(MY_LOGGER, BXILOG_CRITICAL, err,
                  "Can't finalize the BXI logging library");

    BXIFREE(loggers);
    return 0;
}
