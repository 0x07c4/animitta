#ifndef ANIMITTA_LOG_H
#define ANIMITTA_LOG_H

#define ANIMITTA_LOG_LEVEL_ERROR 1
#define ANIMITTA_LOG_LEVEL_WARN 2
#define ANIMITTA_LOG_LEVEL_INFO 3
#define ANIMITTA_LOG_LEVEL_DEBUG 4

#ifndef ANIMITTA_LOG_LEVEL
#ifdef NDEBUG
#define ANIMITTA_LOG_LEVEL ANIMITTA_LOG_LEVEL_ERROR
#else
#define ANIMITTA_LOG_LEVEL ANIMITTA_LOG_LEVEL_DEBUG
#endif
#endif

#if ANIMITTA_LOG_LEVEL >= ANIMITTA_LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...)                                                      \
  fprintf(stderr, "[INFO ] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if ANIMITTA_LOG_LEVEL >= ANIMITTA_LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...)                                                      \
  fprintf(stderr, "[WARN ] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if ANIMITTA_LOG_LEVEL >= ANIMITTA_LOG_LEVEL_ERROR
#define LOG_ERROR(fmt, ...)                                                     \
  fprintf(stderr, "[ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

#if ANIMITTA_LOG_LEVEL >= ANIMITTA_LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...)                                                     \
  fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#endif // !ANIMITTA_LOG_H
