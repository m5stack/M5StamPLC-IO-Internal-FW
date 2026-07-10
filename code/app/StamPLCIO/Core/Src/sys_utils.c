#include "sys_utils.h"

volatile INA226_config config_queue[CONFIG_QUEUE_SIZE];
static volatile uint8_t queue_head = 0, queue_tail = 0;

uint8_t dequeue_config_request(INA226_config *cfg) 
{
  __disable_irq();
  
  uint8_t success = 0;
  if(queue_head != queue_tail) {       // 
    *cfg = config_queue[queue_tail];
    queue_tail = (queue_tail + 1) % CONFIG_QUEUE_SIZE;
    success = 1;
  }
  
  __enable_irq();

  return success;  // 返回操作状态 
}

void enqueue_config_request(INA226_config cfg) 
{
    config_queue[queue_head] = cfg;
    queue_head = (queue_head + 1) % CONFIG_QUEUE_SIZE;
}

uint8_t config_queue_has_data(void) 
{
    return (queue_head != queue_tail);
}
