/******************************************************************************
* File Name:   http_client.h
*
* Description: This file contains declaration of task related to HTTP client
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

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

/*******************************************************************************
* Macros
********************************************************************************/
/* Wi-Fi Credentials: Modify WIFI_SSID, WIFI_PASSWORD and WIFI_SECURITY_TYPE
 * to match your Wi-Fi network credentials.
 * Note: Maximum length of the Wi-Fi SSID and password is set to
 * CY_WCM_MAX_SSID_LEN and CY_WCM_MAX_PASSPHRASE_LEN as defined in cy_wcm.h file.
 */

#define WIFI_SSID     "SSID"
#define WIFI_PASSWORD "PSWD"

#define SERVERHOSTNAME "financialmodelingprep.com"
#define SERVERPORT     (443)

#define NUM_API_KEYS            (3)
#define AMDSTOCKQUOTERESOURCE_0 "/api/v3/quote/AMD?apikey=<mykey>"
#define AMDSTOCKQUOTERESOURCE_1 "/api/v3/quote/AMD?apikey=<mykey>"
#define AMDSTOCKQUOTERESOURCE_2 "/api/v3/quote/AMD?apikey=<mykey>"

/* Security type of the Wi-Fi access point. See 'cy_wcm_security_t' structure
 * in "cy_wcm.h" for more details.
 */
#define WIFI_SECURITY_TYPE CY_WCM_SECURITY_WPA2_AES_PSK

/* Maximum number of connection retries to the Wi-Fi network. */
#define MAX_WIFI_CONN_RETRIES (10u)

/* Wi-Fi re-connection time interval in milliseconds */
#define WIFI_CONN_RETRY_INTERVAL_MSEC (1000)

#define MAKE_IPV4_ADDRESS(a, b, c, d) ((((uint32_t)d) << 24) | (((uint32_t)c) << 16) | (((uint32_t)b) << 8) | ((uint32_t)a))

#define SSL_CLIENTCERT_PEM                                               \
	"-----BEGIN CERTIFICATE-----\n"                                      \
	"MIIDazCCAlOgAwIBAgIUfRaVdsPcfXLdJlWWxFUncPKljo8wDQYJKoZIhvcNAQEL\n" \
	"BQAwRTELMAkGA1UEBhMCVVMxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM\n" \
	"GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAeFw0yMzExMDIyMzU0MDlaFw0zMzEw\n" \
	"MzAyMzU0MDlaMEUxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApTb21lLVN0YXRlMSEw\n" \
	"HwYDVQQKDBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQwggEiMA0GCSqGSIb3DQEB\n" \
	"AQUAA4IBDwAwggEKAoIBAQCqys3P1xv9911VsExY7Ba4PRt8LV4bp4ew/rOecfsw\n" \
	"Xh0Azjh5yuK5ijKBFjcOeyxMSkdkU33dLdXLaqXGYC3Y5Eff+q/6ueaHZjN4MSTJ\n" \
	"xaQujZILdkWbD5SXsC3awD0heN2otlzs1MQdtzDaP0M67AsxT2YvdIt95JLE8Gtr\n" \
	"0XuTBA3I6nXboMT76fV8N7Wd5IjZNU37QgOYCvWdQVQsvdeNH0rTIJgI9U1h/dEK\n" \
	"H0MM8zJfT038naE3K4pL+FaexjgvoehT6s4fEAYT/3IQ7xDzb8vyyunN2qHYfke1\n" \
	"k/gimuILMIqBEmTciSIyHedrm97ZDVq8SdR4/dN6FCOzAgMBAAGjUzBRMB0GA1Ud\n" \
	"DgQWBBQutqeyLtOYs9ktr3o+LjkxY5uPgTAfBgNVHSMEGDAWgBQutqeyLtOYs9kt\n" \
	"r3o+LjkxY5uPgTAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCp\n" \
	"YflCpUMQz46FrYlmW77lbi+DjasKAtQMjaO9kNPv8vQcQ3SFHKDZRP5SAgf6HdFB\n" \
	"RQW/f/Oj0RC7wQuF1AsyYcw8CmO0FAG7LqgeGldZWdzbZEzVLYyAyMkGKpzHnvGj\n" \
	"kH/BQbwE1DukwaGYknQrcvkNbpIj6DgVDkEkey5366TqZI+2xoE6/g7Yo0G8/kbM\n" \
	"u5UX2weyaSMtdAm5wajpspLuvMK5QA95yjVRPsoaytByXrKRHc5yLXGHL6uFEhpE\n" \
	"SJfcyx2iDrrwB8YH7rjUO48ZATx68wjZBitSycwSIUYFpzDwbPxrFoKQ4n699yEt\n" \
	"IsZXRZVoSJ2oHMmRVSUv\n"                                             \
	"-----END CERTIFICATE-----\n"

/* SSL client private key. */
#define SSL_CLIENTKEY_PEM                                                \
	"-----BEGIN PRIVATE KEY-----\n"                                      \
	"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCqys3P1xv9911V\n" \
	"sExY7Ba4PRt8LV4bp4ew/rOecfswXh0Azjh5yuK5ijKBFjcOeyxMSkdkU33dLdXL\n" \
	"aqXGYC3Y5Eff+q/6ueaHZjN4MSTJxaQujZILdkWbD5SXsC3awD0heN2otlzs1MQd\n" \
	"tzDaP0M67AsxT2YvdIt95JLE8Gtr0XuTBA3I6nXboMT76fV8N7Wd5IjZNU37QgOY\n" \
	"CvWdQVQsvdeNH0rTIJgI9U1h/dEKH0MM8zJfT038naE3K4pL+FaexjgvoehT6s4f\n" \
	"EAYT/3IQ7xDzb8vyyunN2qHYfke1k/gimuILMIqBEmTciSIyHedrm97ZDVq8SdR4\n" \
	"/dN6FCOzAgMBAAECggEAHDUymMkmTmO0fpqHgYg8mGE/UNsNV/PrbUgj0l9w5SQm\n" \
	"FiP5H+RqXcp9XOSWY0XkoxUBhYhkmUsgrGAI0SLBb7kERfHZbseVU3eFuKwlL039\n" \
	"N9MZ89qs1FdWa1clu6NUdVrJATxdTKA3Y9EFuIYKTIj/sD0021GcWEaQFwa+Nyg0\n" \
	"0nzCSyLc0ROOK4N99oe7TO1sl+dedhOWbkYhC6PIfYvI2W4bB7LG3S8w0ZMxRINj\n" \
	"8dHpkH2sd7k3wV/siK5C9TtM+1F3CvGFuwGL2ny4t+JVogwDpvENjRSb7vyJuF8B\n" \
	"lpIz8FASZZ7lnchlDf0RumcUm4tlPKWAtXrmPNbNAQKBgQDe6GfyPtP06LWNfGzv\n" \
	"imBOIPVDtrH8Vl0mte+wT/ThtAxVMZ3prEnidMHWHKYz9+lhWCBjPxNk7mK5VAjt\n" \
	"kPuwSCZKiLCyU/UkbAnFRymJf0zPPO/PCQRATgAABH/RwhjVNcjRvthnZ7qu1uyP\n" \
	"G9DdFCzJ5O2GHzGyV8qD5oZYUQKBgQDEJb9gaf1XCl84RuAiRzqyHY5b5zJgRg0/\n" \
	"Olv/EEJSRkM8+8pPCiF7HTwHDT3QWdFCRNzB0w4BJY684B2NlFJ29BaCl6csn9P3\n" \
	"vFASWTcWDzJNt/mDv9l6DVTWV9yTWgPjaHlD5JAUNL9SMrDAuYnIgl2CQSHksggc\n" \
	"81OT4xd+wwKBgQCW5KRY7Ab3ZwBfgrcLgJSozs64w9fAhYYRSl+y7q1KjGM82JWZ\n" \
	"NTPexELi+gnYbB3j85AZHv21+VmDM0vH5bk7H9wr4FQco5Dy0t/aV83acf5A23NK\n" \
	"p0Plc+e0G1ebWtUMn5gNxC8h8KoC6SLBzYaZ4apgEURo4nz1yVAh8c9HwQKBgQCz\n" \
	"YY4ev2+nzV19mxvZXWjpgawykkJ41zfALj/6CMWOdqGBQQgotqwd6oN+M3bneBxG\n" \
	"DbISNFEnbVCXnSKJ2y57iQiZD9YcMmLoujhEP4DoVKd9ehXfjnF2dIC0sY+Q1o6d\n" \
	"x2q3U20MfZzGsIMDDetbCxY3TBeSDdrrlwBFTDatfwKBgEH0SAf+WhdBBpuQzcQU\n" \
	"PXdfJAv7nvYK1EuYx7SMoc5X2qurYy5zb0HbZCz/LAnbJ5BIs/ta6cJmVrhDPDEM\n" \
	"xzIeqNiLOSwHVbf4x78hisbMWy49idvmDFpC8VSvRs4pFt/84bflYcInXlid0BAB\n" \
	"mNQ/k3fRffW4DdHiYzgoIfmF\n"                                         \
	"-----END PRIVATE KEY-----\n"

#define SSL_ROOTCA_PEM                                                   \
	"-----BEGIN CERTIFICATE-----\n"                                      \
	"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
	"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
	"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
	"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
	"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
	"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
	"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
	"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
	"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
	"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
	"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
	"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
	"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
	"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
	"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
	"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
	"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
	"rqXRfboQnoZsG4q5WTP468SQvvG5\n"                                     \
	"-----END CERTIFICATE-----\n"

/*******************************************************************************
* Function Prototypes
********************************************************************************/
void http_client_task(void *arg);

#endif /* HTTP_CLIENT_H_ */
