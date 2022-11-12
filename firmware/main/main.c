#include <stdio.h>
#include <string.h>
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

#define WIFI_SSID "iptime"
#define WIFI_PASS "01234567"
#define WIFI_RETRY 10
#define AUTH_MODE WIFI_AUTH_OPEN

EventGroupHandle_t wifi_event_group;

/**
 * @brief 이벤트 그룹은 각 이벤트에 여러 비트값들을 제공하는데, 여기서는 아래의 2개의 비트값만 알면 된다
 * BIT0: AP에 연결이 됨
 * BIT1: 재시도 횟수 만큼 계속 연결을 시도 해봤지만 결국 실패함
 */
#define SUCCESS_BIT BIT0
#define FAIL_BIT BIT1

#define SERVER_IP	"54.180.0.192"
#define SERVER_PORT	1129

static const char * TAG = "REMOTE_PC_SW";

int pin_state = -1;

int sock;
uint32_t retry_count = 0;
char * packet = "ESP32_CONNECTED";

// GPIO PART
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
		if (pin_state > -1) {
			pc_sw_control(pin_state);
            pin_state--;
            
            if(pin_state < 0){
                const char * switch_response = "PC_POWER_ON...PLEASE_WAIT_FOR_DISPLAY_ON";
                int err = send(sock, switch_response, strlen(switch_response), 0);
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
		}
        vTaskDelay(GPIO_THREAD_PERIOD / portTICK_PERIOD_MS);
    }
}

// WIFI PART
void event_handler(void * arg, esp_event_base_t event_base, int32_t event_id, void * event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
		esp_wifi_connect();
	}
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		if (retry_count < WIFI_RETRY)
		{
			esp_wifi_connect();
			retry_count++;
			ESP_LOGI(TAG, "Retry connecting...");
		}
		else
		{
			xEventGroupSetBits(wifi_event_group, FAIL_BIT);
		}
		ESP_LOGI(TAG, "Connect FAIL!!");
	}
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t * event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&event->ip_info.ip));
		retry_count = 0;
		xEventGroupSetBits(wifi_event_group, SUCCESS_BIT);
	}
}

void wifi_init_sta(void)
{
	wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
														ESP_EVENT_ANY_ID,
														&event_handler,
														NULL,
														&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
														IP_EVENT_STA_GOT_IP,
														&event_handler,
														NULL,
														&instance_got_ip));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = WIFI_SSID,
			.password = WIFI_PASS,
			.threshold.authmode = AUTH_MODE,
		},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	EventBits_t bits = xEventGroupWaitBits(wifi_event_group, SUCCESS_BIT | FAIL_BIT,
										   pdFALSE, pdFALSE, portMAX_DELAY);

	if (bits & SUCCESS_BIT)
	{
		ESP_LOGI(TAG, "CONNECTING SUCCESS!!\tSSID:%s PASS:%s", WIFI_SSID, WIFI_PASS);
	}
	else if (bits & FAIL_BIT)
	{
		ESP_LOGI(TAG, "CONNECTING FAIL!!\tSSID:%s, PASS:%s", WIFI_SSID, WIFI_PASS);
	}
	else
	{
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}
}

// TCP PART
void tcp_client_task(void * pvParameters)
{
	uint8_t rx_buffer[128];
	char host_ip[] = SERVER_IP;
	int addr_family = 0;
	int ip_protocol = 0;

	while (1)
	{
		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = inet_addr(host_ip);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(SERVER_PORT);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;

		sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (sock < 0)
		{
			ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, SERVER_PORT);

		int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
		if (err != 0)
		{
			ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG, "Successfully connected");

		while (1)
		{
			err = send(sock, packet, strlen(packet), 0);
			if (err < 0)
			{
				ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
				break;
			}

			int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
			if (len < 0)
			{
				ESP_LOGE(TAG, "recv failed: errno %d", errno);
				break;
			}
			else
			{
				rx_buffer[len] = 0;
				ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
				ESP_LOGI(TAG, "%s", rx_buffer);
				if(strstr((char *)rx_buffer, "PC_SW_ON") != NULL) {
                    pin_state = 1;
                    static const char *response_ = "PC_SW_ON_RECEIVED!!";
                    int err = send(sock, response_, strlen(response_), 0);
                    if (err < 0) {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        break;
                    }
                }
			}
			vTaskDelay(2000 / portTICK_PERIOD_MS);
		}

		if (sock != -1)
		{
			ESP_LOGE(TAG, "Shutting down socket and restarting...");
			shutdown(sock, 0);
			close(sock);
		}
	}
	vTaskDelete(NULL);
}

// MAIN
void app_main(void)
{
	configure_pin();
	
	esp_err_t ret = nvs_flash_init();
	// ESP_ERROR_CHECK(nvs_flash_init());
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	wifi_init_sta();
	
	xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);

	xTaskCreate(gpio_task, "gpio", 4096, NULL, 0, NULL);
}
