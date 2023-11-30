#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"
#include "dhcpserver/dhcpserver.h"
#include "dnsserver/dnsserver.h"


#define TCP_PORT 80
#define POLL_TIME_S 5
#define HTTP_GET "GET"
#define HTTP_RESPONSE_HEADERS "HTTP/1.1 %d OK\nContent-Length: %d\nContent-Type: text/html;\
                               charset=utf-8\nConnection: close\n\n"



// This is the SSID and Password to connect to initially.
// It should be a working network! 
// auth mode should be changed accordingly.
char ssid[] = "CPH1607";
char pass[] = "12345678"; 
int mode = CYW43_AUTH_WPA2_MIXED_PSK;

char* ssid_list;
#include "hardware/vreg.h"
#include "hardware/clocks.h"


void run_server() {
    httpd_init();
    ssi_init();
    cgi_init();
    printf("Http server initialized.\n");

    // Keep it stuck here until button 20 pressed on pico maker
    while(gpio_get(20)){
    }
}



int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    


    // Set pico into web server mood
    cyw43_arch_enable_sta_mode();

    sleep_ms(5000);

    // Scan for SSIDs available nearby
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
    sleep_ms(5000);
    printf("Scanning done");

  
    // Connecting to Wifi now.
    // The parameters are configured above as ssid and pass.
    // Need to be changed to Credentials of Network that user actually wants to use (their own wifi network)
    cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 20, 1, 1, 1));
    printf("Connecting to WiFi...\n");
    
    // If Connection timeout, error message is shown. Authentication can be changed in variable mode at top of the file
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, mode, 30000)) {
        printf("failed to connect with WPA2 Mixed PSK. Trying alternative auth!\n\n");
        return 1;
    } 

    else {
        printf("Connected.\n");

        extern cyw43_t cyw43_state;
        uint32_t ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
        printf("IP Address: %lu.%lu.%lu.%lu\n", ip_addr & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr >> 16) & 0xFF, ip_addr >> 24);
    }
    // turn on LED to signal connected
    // IF Pico lights up, connectedd to Wifi!
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    // Web server Started! Program will be paused here until button 20 is pressed on Pico maker. 
    // Button 20 should not be pressed until ssid to spoof is chosen on web server!
    run_server();
    if(!gpio_get(20)){
        printf("changing to AP Mode\n");
    }



    puts("Starting AP");
    // Chosen SSID is a variable from cgi.h
    char* SSID_to_spoof = chosen_SSID;
    char* password = "12345678";
    cyw43_arch_enable_ap_mode(SSID_to_spoof, password, CYW43_AUTH_WPA2_MIXED_PSK);
    // cyw43_arch_enable_ap_mode(ssid2, pass, CYW43_AUTH_WPA2_AES_PSK);
    printf("Spoofing %s\n SSID\n", SSID_to_spoof);



// Configuring Access point to have DHCP and DNS Server. 
// Referenced from pico example. 

    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        printf("failed to allocate state\n");
        // Below was originally called
        //DEBUG_printf("failed to allocate state\n");
        return 1;
    }

    // 'state' is a pointer to type TCP_SERVER_T 
    ip4_addr_t mask;
    IP4_ADDR(ip_2_ip4(&state->gw), 192, 168, 4, 1);
    IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);

    // Start the dhcp server
    // and set picoW IP address from 'state' structure
    // set 'mask' as defined above
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

    // Start the dns server
    // and set picoW IP address from 'state' structure
    dns_server_t dns_server;
    dns_server_init(&dns_server, &state->gw);
    

    while(1);
    
}
