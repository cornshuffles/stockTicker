/******************************************************************************
* File Name:   http_client.c
*
* Description: This file contains task and functions related to HTTP client
* operation.
*
*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

/* Header file includes. */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cJSON.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "cy8ckit_028_tft_pins.h"
#include <time.h>

/* FreeRTOS header file. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Standard C header file. */
#include <string.h>

/* Cypress secure socket header file. */
#include "cy_secure_sockets.h"

/* Wi-Fi connection manager header files. */
#include "cy_wcm.h"
#include "cy_wcm_error.h"

/* TCP client task header file. */
#include "http_client.h"

/* HTTP Client Library*/
#include "cy_http_client_api.h"

/*******************************************************************************
* Macros
********************************************************************************/
#define BUFFERSIZE         (2048 * 2)
#define SENDRECEIVETIMEOUT (10000)

// Macros for GUI
#define TFT_LEFT_ALIGNED (0)
#define TFT_PRICE        (88)
#define TFT_PERCENT      (210)
#define TFT_ROW_ONE      (0)
#define TFT_ROW_TWO      (30)
#define TFT_ROW_THREE    (50)
#define TFT_ROW_FOUR     (70)
#define TFT_ROW_FIVE     (90)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
cy_rslt_t connect_to_wifi_ap(void);
void disconnect_callback(void *arg);

/*******************************************************************************
* Global Variables
********************************************************************************/
volatile bool connected = false;
uint32_t apiKeySelect = 0;

/*******************************************************************************
 * Function Name: http_client_task
 *******************************************************************************
 * Summary:
 *  Task used to establish a secure connection to a remote TCP server, request
 *  data from the server, then print the data to the tft dispaly.
 *
 * Parameters:
 *  void *args : Task parameter defined during task creation (unused).
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void http_client_task(void *arg) {
	cy_rslt_t result;

	/* The pins above are defined by the CY8CKIT-028-TFT library. If the display is being used on different hardware the mappings will be different. */
	const mtb_st7789v_pins_t tft_pins = {.db08 = CY8CKIT_028_TFT_PIN_DISPLAY_DB8,
										 .db09 = CY8CKIT_028_TFT_PIN_DISPLAY_DB9,
										 .db10 = CY8CKIT_028_TFT_PIN_DISPLAY_DB10,
										 .db11 = CY8CKIT_028_TFT_PIN_DISPLAY_DB11,
										 .db12 = CY8CKIT_028_TFT_PIN_DISPLAY_DB12,
										 .db13 = CY8CKIT_028_TFT_PIN_DISPLAY_DB13,
										 .db14 = CY8CKIT_028_TFT_PIN_DISPLAY_DB14,
										 .db15 = CY8CKIT_028_TFT_PIN_DISPLAY_DB15,
										 .nrd = CY8CKIT_028_TFT_PIN_DISPLAY_NRD,
										 .nwr = CY8CKIT_028_TFT_PIN_DISPLAY_NWR,
										 .dc = CY8CKIT_028_TFT_PIN_DISPLAY_DC,
										 .rst = CY8CKIT_028_TFT_PIN_DISPLAY_RST};

	/* Initialize the display */
	mtb_st7789v_init8(&tft_pins);
	GUI_Init();
	GUI_SetBkColor(GUI_BLACK);          // Background Color
	GUI_SetColor(GUI_WHITE);            // Text Color
	GUI_SetFont(&GUI_Font32B_ASCII);    // Font Size

	// Connect to wifi - configuration settings are in http_client.h
	result = connect_to_wifi_ap();
	CY_ASSERT(result == CY_RSLT_SUCCESS);

	// Initialize http client lib
	result = cy_http_client_init();
	if(result != CY_RSLT_SUCCESS) {
		printf("HTTP Client Library Initialization Failed!\n");
		CY_ASSERT(0);
	}

	// Initialize serverInfo and credentials structs
	cy_awsport_server_info_t serverInfo;
	cy_awsport_ssl_credentials_t credentials;
	(void)memset(&credentials, 0, sizeof(credentials));
	(void)memset(&serverInfo, 0, sizeof(serverInfo));

	// Server Info - configuration settings are in http_client.h
	serverInfo.host_name = SERVERHOSTNAME;
	serverInfo.port = SERVERPORT;

	// Set the credentials information
	credentials.root_ca = SSL_ROOTCA_PEM;
	credentials.root_ca_size = sizeof(SSL_ROOTCA_PEM);
	credentials.root_ca_verify_mode = CY_AWS_ROOTCA_VERIFY_REQUIRED;
	credentials.client_cert = SSL_CLIENTCERT_PEM;
	credentials.client_cert_size = sizeof(SSL_CLIENTCERT_PEM);
	credentials.private_key = SSL_CLIENTKEY_PEM;
	credentials.private_key_size = sizeof(SSL_CLIENTKEY_PEM);

	// Disconnection Callback
	cy_http_disconnect_callback_t disconnectCallback = (void *)disconnect_callback;

	// Client Handle
	cy_http_client_t clientHandle;

	// Create the HTTP Client
	result = cy_http_client_create(&credentials, &serverInfo, disconnectCallback, NULL, &clientHandle);
	if(result != CY_RSLT_SUCCESS) {
		printf("HTTP Client Creation Failed!\n");
		CY_ASSERT(0);
	}

	// For time.h
	setenv("TZ", "EST5EDT", 1);

	// Main program loop
	while(1) {
		if(!connected) {
			// Connect to the HTTP Server
			result = cy_http_client_connect(clientHandle, SENDRECEIVETIMEOUT, SENDRECEIVETIMEOUT);
			if(result != CY_RSLT_SUCCESS) {
				printf("HTTP Client Connection Failed!\nResult: %lx\n", result);
			} else {
				printf("\nConnected to HTTP Server Successfully\n\n");
				connected = true;
			}
		}

		// Create and send get request, parse and display data
		if(connected) {
			printf("Creating request. Connection state: %d\n", connected);

			// Create Request
			uint8_t buffer[BUFFERSIZE];
			cy_http_client_request_header_t request;
			request.buffer = buffer;
			request.buffer_len = BUFFERSIZE;
			request.method = CY_HTTP_CLIENT_METHOD_GET;
			request.range_start = -1;
			request.range_end = -1;
			switch(apiKeySelect) {
				case 0:
					request.resource_path = AMDSTOCKQUOTERESOURCE_0;
					printf("Key 0\n");
					break;
				case 1:
					request.resource_path = AMDSTOCKQUOTERESOURCE_1;
					printf("Key 1\n");
					break;
				case 2:
					request.resource_path = AMDSTOCKQUOTERESOURCE_2;
					printf("Key 2\n");
					break;
				default:
					apiKeySelect = 0;
					request.resource_path = AMDSTOCKQUOTERESOURCE_0;
					printf("DEFAULT\n");
					break;
			}

			// Create Header
			cy_http_client_header_t header;
			header.field = "Host";
			header.field_len = strlen("Host");
			header.value = SERVERHOSTNAME;
			header.value_len = strlen(SERVERHOSTNAME);
			uint32_t num_header = 1;

			result = cy_http_client_write_header(clientHandle, &request, &header, num_header);
			if(result != CY_RSLT_SUCCESS) {
				printf("HTTP Client Header Write Failed!\n");
				CY_ASSERT(0);
			}

			// Var to hold the servers responses
			cy_http_client_response_t response;

			printf("Sending HTTP Request\n");
			result = cy_http_client_send(clientHandle, &request, NULL, 0, &response);
			if(result != CY_RSLT_SUCCESS) {
				printf("HTTP Client Send Failed!\n");
			}

			// Print response message
			printf("Response received:\n");
			for(int i = 1; i < response.body_len - 2; i++) {
				printf("%c", response.body[i]);
			}
			printf("\n\n");

			// Parse the JSON received
			cJSON *root = cJSON_ParseWithLength((const char *)response.body + 1, response.body_len - 2);    // Read the JSON
			cJSON *symbol = cJSON_GetObjectItem(root, "symbol");                                            // Ticker
			cJSON *price = cJSON_GetObjectItem(root, "price");                                              // Current Price
			cJSON *change = cJSON_GetObjectItem(root, "changesPercentage");                                 // % change over the day
			cJSON *dayLow = cJSON_GetObjectItem(root, "dayLow");                                            // Todays low
			cJSON *dayHigh = cJSON_GetObjectItem(root, "dayHigh");                                          // Todays high
			cJSON *open = cJSON_GetObjectItem(root, "open");                                                // Todays open price
			cJSON *previousClose = cJSON_GetObjectItem(root, "previousClose");                              // Previous closing price
			cJSON *timestamp = cJSON_GetObjectItem(root, "timestamp");                                      // Unix time stamp

			// Disconnect from server
			result = cy_http_client_disconnect(clientHandle);
			if(result != CY_RSLT_SUCCESS) {
				printf("HTTP Client Disconnect Failed!\n");
			} else {
				printf("Disconnected from HTTP Server\n");
				connected = false;
			}

			// Check if website actually gave us data
			if((int)cJSON_IsString(symbol) == 0) {
				apiKeySelect = (apiKeySelect + 1) % NUM_API_KEYS;
				printf("Request failed. New key select: %ld\n", apiKeySelect);
				continue;    // Jump to next iteration of loop
			}

			printf("Drawing Info.\n");
			// Display the stock info
			GUI_Clear();    // Clear the display
			char symbol_s[25];
			GUI_SetFont(&GUI_Font32B_ASCII);    // Font Size
			sprintf(symbol_s, "%s", symbol->valuestring);
			GUI_DispStringAt(symbol_s, TFT_LEFT_ALIGNED, TFT_ROW_ONE);

			char price_s[25];
			sprintf(price_s, "$%.2f", price->valuedouble);
			GUI_DispStringAt(price_s, TFT_PRICE, TFT_ROW_ONE);

			char change_s[25];
			if(change->valuedouble >= 0) {
				GUI_SetColor(GUI_GREEN);    // Text Color
				sprintf(change_s, "+%.2f%%\n", change->valuedouble);
			} else {
				GUI_SetColor(GUI_RED);    // Text Color
				sprintf(change_s, "%.2f%%\n", change->valuedouble);
			}
			GUI_DispStringAt(change_s, TFT_PERCENT, TFT_ROW_ONE);
			GUI_SetColor(GUI_WHITE);    // Text Color

			char open_close[25];
			GUI_SetFont(&GUI_Font24B_ASCII);    // Font Size
			sprintf(open_close, "$%.2f / $%.2f", previousClose->valuedouble, open->valuedouble);
			GUI_DispStringAt("PC/O", TFT_LEFT_ALIGNED, TFT_ROW_TWO);
			GUI_DispStringAt(open_close, TFT_PRICE, TFT_ROW_TWO);

			char day_low_high[25];
			sprintf(day_low_high, "$%.2f / $%.2f", dayLow->valuedouble, dayHigh->valuedouble);
			GUI_DispStringAt("DL/DH", TFT_LEFT_ALIGNED, TFT_ROW_THREE);
			GUI_DispStringAt(day_low_high, TFT_PRICE, TFT_ROW_THREE);

			time_t updateTime = (time_t)timestamp->valueint;
			GUI_SetFont(&GUI_Font20B_ASCII);    // Font Size
			GUI_DispStringAt(ctime(&updateTime), TFT_LEFT_ALIGNED, TFT_ROW_FIVE);

			printf("Deleting stuff.\n");
			// Delete CJSON objects
			cJSON_Delete(root);

			printf("Deleted stuff.\n");
		}    // if(connected)

		printf("Waiting 2 mins.\n");
		// Wait 2 mins before re-connecting and querying again
		vTaskDelay(pdMS_TO_TICKS(120000));

		printf("Waited 2 mins.\n");
	}
}

/*******************************************************************************
 * Function Name: connect_to_wifi_ap()
 *******************************************************************************
 * Summary:
 *  Connects to Wi-Fi AP using the user-configured credentials, retries up to a
 *  configured number of times until the connection succeeds.
 *
 *******************************************************************************/
cy_rslt_t connect_to_wifi_ap(void) {
	cy_rslt_t result;

	/* Variables used by Wi-Fi connection manager.*/
	cy_wcm_connect_params_t wifi_conn_param;

	cy_wcm_config_t wifi_config = {.interface = CY_WCM_INTERFACE_TYPE_STA};

	cy_wcm_ip_address_t ip_address;

	/* Initialize Wi-Fi connection manager. */
	result = cy_wcm_init(&wifi_config);

	if(result != CY_RSLT_SUCCESS) {
		printf("Wi-Fi Connection Manager initialization failed!\n");
		return result;
	}
	printf("Wi-Fi Connection Manager initialized.\r\n");

	/* Set the Wi-Fi SSID, password and security type. */
	memset(&wifi_conn_param, 0, sizeof(cy_wcm_connect_params_t));
	memcpy(wifi_conn_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
	memcpy(wifi_conn_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
	wifi_conn_param.ap_credentials.security = WIFI_SECURITY_TYPE;

	/* Join the Wi-Fi AP. */
	for(uint32_t conn_retries = 0; conn_retries < MAX_WIFI_CONN_RETRIES; conn_retries++) {
		result = cy_wcm_connect_ap(&wifi_conn_param, &ip_address);

		if(result == CY_RSLT_SUCCESS) {
			printf("Successfully connected to Wi-Fi network '%s'.\n", wifi_conn_param.ap_credentials.SSID);
			printf("IP Address Assigned: %d.%d.%d.%d\n", (uint8)ip_address.ip.v4, (uint8)(ip_address.ip.v4 >> 8), (uint8)(ip_address.ip.v4 >> 16),
				   (uint8)(ip_address.ip.v4 >> 24));
			return result;
		}

		printf("Connection to Wi-Fi network failed with error code %d."
			   "Retrying in %d ms...\n",
			   (int)result, WIFI_CONN_RETRY_INTERVAL_MSEC);

		vTaskDelay(pdMS_TO_TICKS(WIFI_CONN_RETRY_INTERVAL_MSEC));
	}

	/* Stop retrying after maximum retry attempts. */
	printf("Exceeded maximum Wi-Fi connection attempts\n");

	return result;
}

/*******************************************************************************
 * Function Name: disconnect_callback
 *******************************************************************************
 * Summary:
 *  Invoked when the server disconnects
 *
 * Parameters:
 *  void *arg : unused
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void disconnect_callback(void *arg) {
	printf("Disconnected from HTTP Server\n");
	connected = false;
}
