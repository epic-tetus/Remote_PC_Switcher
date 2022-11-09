#include <stdio.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "driver/gpio.h"

#define PC_SW_PIN           13

#define GPIO_THREAD_PERIOD  2000

static const char * TAG = "REMOTE_PC_SW";

bool pin_state = false;

void configure_pin(void)
{
    gpio_reset_pin(PC_SW_PIN);

    gpio_set_direction(PC_SW_PIN, GPIO_MODE_OUTPUT);
}

void pc_sw_control(int state)
{
    gpio_set_level(PC_SW_PIN, state);
}

void gpio_task(void *pvParameters)
{
    while (1) {
        gpio_set_level(PC_SW_PIN, pin_state);
		pin_state = !pin_state;
        vTaskDelay(GPIO_THREAD_PERIOD / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
	configure_pin();
	
	xTaskCreate(gpio_task, "gpio", 4096, NULL, 0, NULL);
}
