#include <stdio.h>

#include "pico_uros_sdk/pico_uart_transports.h"
#include "pico/stdlib.h"
#include "drv2605.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/u_int8.h>
#include <std_msgs/msg/string.h>
#include <rmw_microros/rmw_microros.h>

#define ARRAY_LEN 200

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

rcl_publisher_t publisher;
rcl_subscription_t subscriber;
std_msgs__msg__UInt8 effect_id;
std_msgs__msg__String description;

void vibrate_cb(const void * msgin) {
  const std_msgs__msg__UInt8 * msg = (const std_msgs__msg__UInt8 *)msgin;
  gpio_put(LED_PIN, 1);
  sprintf(description.data.data, identify_effect(msg->data));
  description.data.size = strlen(description.data.data);
  set_waveform(0, msg->data);
  set_waveform(1, 0);
  go();
  rcl_publish(&publisher, &description, NULL);
  gpio_put(LED_PIN, 0);
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
  gpio_put(LED_PIN, 1);

  rcl_node_t node;
  rcl_allocator_t allocator;
  rclc_support_t support;
  rclc_executor_t executor;

  allocator = rcl_get_default_allocator();

  rcl_ret_t ret = rmw_uros_ping_agent(1000, 120);
  if (ret != RCL_RET_OK) {

    return ret;
  }

  if (!drv2605_init()) {
    return -1;
  }

  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "haptic_control", "pico", &support);
  
  rclc_publisher_init_default(
    &publisher, 
    &node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "vib_description"
  );
  
  rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
    "vibrate"
  );

  executor = rclc_executor_get_zero_initialized_executor();
  rclc_executor_init(&executor, &support.context, 1, &allocator);
  rclc_executor_add_subscription(&executor, &subscriber, &effect_id, &vibrate_cb, ON_NEW_DATA);

  description.data.data = (char * ) malloc(ARRAY_LEN * sizeof(char));
  description.data.size = 0;
  description.data.capacity = ARRAY_LEN;

  gpio_put(LED_PIN, 0);

  rclc_executor_spin(&executor);

  rcl_subscription_fini(&subscriber, &node);
  rcl_publisher_fini(&publisher, &node);
  rcl_node_fini(&node);

  return 0;
}