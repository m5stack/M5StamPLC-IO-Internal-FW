#ifndef __SYS_UTILS_H__
#define __SYS_UTILS_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "main.h"
#include "ina226.h"

#define CONFIG_QUEUE_SIZE 8

uint8_t dequeue_config_request(INA226_config *cfg);
void enqueue_config_request(INA226_config cfg);
uint8_t config_queue_has_data(void);

#ifdef __cplusplus
}
#endif

#endif
