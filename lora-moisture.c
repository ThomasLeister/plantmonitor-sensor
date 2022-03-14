/*
 * LoRa Moisture Sensor
 * By Thomas Leister <thomas.leister@mailbox.org>
 * 
 * Reads GPIO28 as an analog moisture sensor.
 */

#include <stdio.h>
#include <string.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"

#include "pico/stdlib.h"
#include "pico/lorawan.h"

// edit with LoRaWAN Node Region and OTAA settings 
#include "config.h"

const struct lorawan_sx12xx_settings sx12xx_settings = {
    .spi = {
        .inst = spi1,
        .mosi = 11,
        .miso = 12,
        .sck  = 10,
        .nss  = 3
    },
    .reset = 15,
    .busy = 2,
    .dio1  = 20
};

// OTAA settings
const struct lorawan_otaa_settings otaa_settings = {
    .device_eui   = LORAWAN_DEVICE_EUI,
    .app_eui      = LORAWAN_APP_EUI,
    .app_key      = LORAWAN_APP_KEY,
    .channel_mask = LORAWAN_CHANNEL_MASK
};

// variables for receiving data
int receive_length = 0;
uint8_t receive_buffer[242];
uint8_t receive_port = 0;

char devEui[17];

// functions used in main
void moisture_init();
uint16_t moisture_get();

int main(void) {
    stdio_init_all();
    printf("\nPico LoRaWAN - Moisture sensor\n\n");

    moisture_init();

    // Enable debugging
    //lorawan_debug(true);

    //NvmDataMgmtFactoryReset();
    //EepromMcuFlush();

    // initialize the LoRaWAN stack
    printf("Initializing LoRaWAN ... ");

    // Activate via Over-the-air activation
    if (lorawan_init_otaa(&sx12xx_settings, LORAWAN_REGION, &otaa_settings) < 0) {
        printf("failed!!!\n");
        while (1) {
            tight_loop_contents();
        }
    } else {
        printf("success!\n");
    }

    // Start the join process and wait
    printf("Joining LoRaWAN network ...");
    
    // Join network
    lorawan_join();

    while (!lorawan_is_joined()) {
        lorawan_process_timeout_ms(1000);
        printf(".");
    }
    printf(" joined successfully!\n");


    while (1) {
        // get the internal temperature
        uint16_t adc_moisture_bytes = moisture_get();

        // send the internal temperature as a (signed) byte in an unconfirmed uplink message
        printf("Sending moisture level raw ADC value: %d", adc_moisture_bytes);
        if (lorawan_send_unconfirmed(&adc_moisture_bytes, sizeof(adc_moisture_bytes), 2) < 0) {
            printf("failed!!!\n");
        } else {
            printf("success!\n");
        }

        // wait for 5 minutes
        lorawan_process_timeout_ms(300000);
    }
}



void moisture_init() {
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(28);
}


uint16_t moisture_get() {
    // Select ADC2 (GPIO28)
    adc_select_input(2);
    return adc_read();
}
