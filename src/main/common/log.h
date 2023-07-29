#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "platform.h"

#include "config/parameter_group.h"

#include "common/utils.h"

// Log levels. Defined as preprocessor constants instead of
// a number to allow compile-time comparisons.
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_VERBOSE 3
#define LOG_LEVEL_DEBUG 4

typedef enum {
    LOG_TOPIC_SYSTEM,           // 0, mask = 1
    LOG_TOPIC_GYRO,             // 1, mask = 2
    LOG_TOPIC_BARO,             // 2, mask = 4
    LOG_TOPIC_PITOT,            // 3, mask = 8
    LOG_TOPIC_PWM,              // 4, mask = 16
    LOG_TOPIC_TIMER,            // 5, mask = 32
    LOG_TOPIC_IMU,              // 6, mask = 64
    LOG_TOPIC_TEMPERATURE,      // 7, mask = 128
    LOG_TOPIC_POS_ESTIMATOR,    // 8, mask = 256
    LOG_TOPIC_VTX,              // 9, mask = 512
    LOG_TOPIC_OSD,              // 10, mask = 1024

    LOG_TOPIC_COUNT,
} logTopic_e;

STATIC_ASSERT(LOG_TOPIC_COUNT < 32, too_many_log_topics);

typedef struct logConfig_s {
    uint8_t level; // from LOG_LEVEL_ constants. All messages equal or below this verbosity level are printed.
    uint32_t topics; // All messages with topics in this bitmask (1 << topic) will be printed regardless of their level.
} logConfig_t;

PG_DECLARE(logConfig_t, logConfig);

void logInit(void);
void _logf(logTopic_e topic, unsigned level, const char *fmt, ...) __attribute__ ((format (printf, 3, 4)));
void _logBufferHex(logTopic_e topic, unsigned level, const void *buffer, size_t size);

#if defined(USE_LOG)
#define LOG_ERROR(topic, fmt, ...) _logf(LOG_TOPIC_ ## topic, LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_BUFFER_ERROR(topic, buf, size) _logBufferHex(LOG_TOPIC_ ## topic, LOG_LEVEL_ERROR, buf, size)
#else
#define LOG_ERROR(...)
#define LOG_BUFFER_ERROR(...)
#endif

#if defined(USE_LOG)
#define LOG_WARNING(topic, fmt, ...) _logf(LOG_TOPIC_ ## topic, LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define LOG_BUF_WARNING(topic, buf, size) _logBufferHex(LOG_TOPIC_ ## topic, LOG_LEVEL_WARNING, buf, size)
#else
#define LOG_WARNING(...)
#define LOG_BUF_WARNING(...)
#endif

#if defined(USE_LOG)
#define LOG_INFO(topic, fmt, ...) _logf(LOG_TOPIC_ ## topic, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_BUF_INFO(topic, buf, size) _logBufferHex(LOG_TOPIC_ ## topic, LOG_LEVEL_INFO, buf, size)
#else
#define LOG_INFO(...)
#define LOG_BUF_INFO(...)
#endif

#if defined(USE_LOG)
#define LOG_VERBOSE(topic, fmt, ...) _logf(LOG_TOPIC_ ## topic, LOG_LEVEL_VERBOSE, fmt, ##__VA_ARGS__)
#define LOG_BUF_VERBOSE(topic, buf, size) _logBufferHex(LOG_TOPIC_ ## topic, LOG_LEVEL_VERBOSE, buf, size)
#else
#define LOG_VERBOSE(...)
#define LOG_BUF_VERBOSE(...)
#endif

#if defined(USE_LOG)
#define LOG_DEBUG(topic, fmt, ...) _logf(LOG_TOPIC_ ## topic, LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_BUF_DEBUG(topic, buf, size) _logBufferHex(LOG_TOPIC_ ## topic, LOG_LEVEL_DEBUG, buf, size)
#else
#define LOG_DEBUG(...)
#define LOG_BUF_DEBUG(...)
#endif
