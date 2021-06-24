#include <stdio.h>

#include "pico_uart_transports.h"
#include "pico/stdlib.h"
#include "drv2605.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

rcl_publisher_t publisher;
rcl_subscription_t subscriber;
std_msgs__msg__Int32 msg;

void ping_cb(const void * msgin) {
  std_msgs__msg__Int32 * ping_num = (std_msgs__msg__Int32 *)msgin;
  ping_num->data++;
  gpio_put(LED_PIN, 0);
  rcl_publish(&publisher, ping_num, NULL);
  gpio_put(LED_PIN, 1);
}

int main() {
  rmw_uros_set_custom_transport(
		true,
		NULL,
		pico_serial_transport_open,
		pico_serial_transport_close,
		pico_serial_transport_write,
		pico_serial_transport_read
	);

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  rcl_node_t node;
  rcl_allocator_t allocator;
  rclc_support_t support;
  rclc_executor_t executor;

  allocator = rcl_get_default_allocator();

  rcl_ret_t ret = rmw_uros_ping_agent(1000, 120);
  if (ret != RCL_RET_OK) {
    gpio_put(LED_PIN, 0);
    return ret;
  }

  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "ping_pong", "pico", &support);
  
  rclc_publisher_init_default(
    &publisher, 
    &node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "pong"
  );
  
  rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "ping"
  );

  if (!drv2605_init()) {
    printf("Failed to initialize the haptic driver\n");
  }

  select_library(1);
  set_mode(DRV2605_MODE_INTTRIG);

  int effect = 1;
  printf("11.2 Waveform Library Effects List\n");

  while(effect < 117) {
    identify_effect(effect);

    set_waveform(0, effect);
    set_waveform(1, 0);
    go();

    sleep_ms(1000);

    ++effect;
    if (effect > 117) {
      effect = 1;
    }
  }

  return 0;
}