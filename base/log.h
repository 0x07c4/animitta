#ifndef ANIMITTA_LOG_H
#define ANIMITTA_LOG_H

// #define LOG_ENABLE

#ifdef LOG_ENABLE
#define LOG_INFO(fmt, ...)                                                     \
  fprintf(stderr, "[INFO ] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...)                                                     \
  fprintf(stderr, "[WARN ] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_ERROR(fmt, ...)                                                    \
  fprintf(stderr, "[ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_DEBUG(fmt, ...)                                                    \
  fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)

#define LOG_WARN(fmt, ...)

#define LOG_ERROR(fmt, ...)

#define LOG_DEBUG(fmt, ...)
#endif

#endif // !ANIMITTA_LOG_H
