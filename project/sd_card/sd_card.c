/* main.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hw_config.h"
#include "f_util.h"
#include "ff.h"
// #include "/lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico"
// #include "hw_config.h"

/*

This file should be tailored to match the hardware design.

See 
https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/tree/main#customizing-for-the-hardware-configuration

*/



// Commented out cuz configured in config.h
/* SDIO Interface */
// static sd_sdio_if_t sdio_if = {
//     /*
//     Pins CLK_gpio, D1_gpio, D2_gpio, and D3_gpio are at offsets from pin D0_gpio.
//     The offsets are determined by sd_driver\SDIO\rp2040_sdio.pio.
//         CLK_gpio = (D0_gpio + SDIO_CLK_PIN_D0_OFFSET) % 32;
//         As of this writing, SDIO_CLK_PIN_D0_OFFSET is 30,
//             which is -2 in mod32 arithmetic, so:
//         CLK_gpio = D0_gpio -2.
//         D1_gpio = D0_gpio + 1;
//         D2_gpio = D0_gpio + 2;
//         D3_gpio = D0_gpio + 3;
//     */
//     // .CLK_gpio = 10,
//     .CMD_gpio = 11,
//     .D0_gpio = 12,
//     .D1_gpio = 13,
//     .D2_gpio = 14,
//     .D3_gpio = 15,
//     .baud_rate = 115200 //15 * 1000 * 1000  // 15 MHz
// };

/* Hardware Configuration of the SD Card socket "object" */
// static sd_card_t sd_card = {
//     /* "pcName" is the FatFs "logical drive" identifier.
//     (See http://elm-chan.org/fsw/ff/doc/filename.html#vol) */
//     .pcName = "0:",
//     .type = SD_IF_SDIO,
//     .sdio_if_p = &sdio_if
// };



//
// Defining my own functions using on top of the library.
FRESULT initialiseSD(){
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr) 
    {
        printf("test print");
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    else{
        printf("Returning FR");
    }
    
    return fr;
}
// Save file with given filename and content. append if exists
int saveFile(char* filename, char* content, FRESULT fr, FIL file){

    fr = f_open(&file, filename, FA_OPEN_APPEND|FA_WRITE);

    if (FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    if (f_printf(&file, content) < 0) {
        printf("f_printf failed\n");
    }

    printf("Write to sd card successful\n");

    fr = f_close(&file);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    else{
        printf("file closed");
    }
    return true;
}

int readFile(char* filename, FRESULT fr, FIL file){
    char buf[100];

    fr = f_open(&file, filename, FA_READ);
    if (FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);


    printf("File opened for reading\n");
    printf("Data read is being written out now:\n");
    while(f_gets(buf, sizeof(buf), &file)){
        printf(buf);
    };

    
    

    return true;
}


// int main() {
//     stdio_init_all();
//     sleep_ms(10000);

   
//     printf("Starting SD Init!");
//     FRESULT fr = initialiseSD();
//     printf("succesfuuly initialised SD Card\n");
    
//     FIL file;
//     printf("Starting Write to file now!");
//     sleep_ms(1000);
//     for (int i = 0; i < 1000; i++){
//         saveFile("TestingNow.txt", "Anything\n", fr, file);
//     }
    
    
//     FRESULT fr2 = initialiseSD();
//     printf("SD Initialised!");
//     FIL file2;
//     readFile("TestingNow.txt",fr2, file2);

//     sd_card_t *pSD = sd_get_by_num(0);   
//     f_unmount(pSD->pcName);
    
//     puts("Goodbye, world!");
//      for (;;);
// }