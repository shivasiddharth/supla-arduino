/**
 * Supla.org NodeMCU WiFi minimal example
 * Author: Programistyk - Kamil Kaminski <kamil@programistyk.pl>
 * 
 * This example shows how to configure SuplaDevice for building for NodeMCU within Arduino IDE
 */


//************************Written by Sid for Sid's E Classroom*****************************
//**********************https://www.youtube.com/c/SidsEClassroom***************************
//*************************https://github.com/shivasiddharth*******************************



#include <srpc.h>
#include <log.h>
#include <eh.h>
#include <proto.h>
#include <IEEE754tools.h>
// We define our own ethernet layer
#define SUPLADEVICE_CPP
#include <SuplaDevice.h>
#include <lck.h>

#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServer.h>
#include <ESP8266WiFiGeneric.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include <Adafruit_NeoPixel.h>

//Pin is set for Wemos. If using any other board, change it accordingly.
#define LED_PIN D6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 12

// Declare NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

unsigned char _red = 255;
unsigned char _green = 255;
unsigned char _blue = 255;
unsigned char _color_brightness = 50;
unsigned char _brightness = 50;

WiFiClient client;

// Setup Supla connection
const char* ssid     = "ENTER YOUR WIFI ID";
const char* password = "ENTER YOUR WIFI PASSWORD";

// Enter the Supla server credentials here from your Supla account
const char* supla_server    = "ADD THE ADDRESS OF YOUR SUPLA SERVER HERE";
const char* supla_location_password = "ADD YOUR LOCATION PASSWORD HERE";
const int location_id = ENTER YOUR LOCATION ID HERE;

#define Start_Address 0
#define R_Address Start_Address + sizeof(char)
#define G_Address R_Address + sizeof(char)
#define B_Address G_Address + sizeof(char)
#define Bri_Address B_Address + sizeof(char)

void get_rgbw_value(int channelNumber, unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *color_brightness, unsigned char *brightness) {

  *brightness = _brightness;
  *color_brightness = _color_brightness;

  *red = _red;
  *green = _green;
  *blue = _blue;

}

void set_rgbw_boot() {
    
  Serial.println(_red);
  Serial.println(_green);
  Serial.println(_blue);
  Serial.println(_color_brightness);
  Serial.println(_brightness); 

  colorSet(strip.Color(int(_red),int(_green),int(_blue)), 50);  
  strip.setBrightness(int(_brightness));
  strip.show();
    
}

void set_rgbw() {
    
  Serial.println(_red);
  Serial.println(_green);
  Serial.println(_blue);
  Serial.println(_color_brightness);
  Serial.println(_brightness);

  colorSet(strip.Color(int(_red),int(_green),int(_blue)), 50);  
  strip.setBrightness(int(_brightness));
  strip.show();

  EEPROM.put(R_Address, _red);
  EEPROM.put(G_Address, _green);
  EEPROM.put(B_Address, _blue);
  EEPROM.put(Bri_Address, _brightness);
  EEPROM.commit();
  EEPROM.end();
    
}

void set_rgbw_value(int channelNumber, unsigned char red, unsigned char green, unsigned char blue, unsigned char color_brightness, unsigned char brightness) {

  _brightness = brightness;
  _color_brightness = color_brightness;
  _red = red;
  _green = green;
  _blue = blue;
  set_rgbw(); 
  
}

void colorSet(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    delay(wait);                           //  Pause for a moment
  }
}


void setup() {
  Serial.begin(115200);

  EEPROM.begin(256);
  EEPROM.get(R_Address, _red);
  EEPROM.get(G_Address, _green);
  EEPROM.get(B_Address, _blue);
  EEPROM.get(Bri_Address, _brightness);  
    
  delay(10);
  
  strip.begin();

  Serial.println("Values before the device was shutdown:");
  
  set_rgbw_boot();
  
  SuplaDevice.setRGBWCallbacks(&get_rgbw_value, &set_rgbw_value);
  SuplaDevice.addRgbControllerAndDimmer();

  
  // ﻿Replace tyhe following with your GUID
  char GUID[SUPLA_GUID_SIZE] = ADD YOUR GUID HERE;
  // ﻿with GUID that you can retrieve from https://www.supla.org/arduino/get-guid

  // Ethernet MAC address
  uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

 
  /*
   * Having your device already registered at cloud.supla.org,
   * you want to change CHANNEL sequence or remove any of them,
   * then you must also remove the device itself from cloud.supla.org.
   * Otherwise you will get "Channel conflict!" error.
   */
                                           

 

  SuplaDevice.begin(GUID,                        // Global Unique Identifier 
                    mac,                         // Ethernet MAC address
                    supla_server,                // SUPLA server address
                    location_id,                 // Location ID 
                    supla_location_password);    // Location Password

}

void loop() {
  SuplaDevice.iterate();
}

// Supla.org ethernet layer
    int supla_arduino_tcp_read(void *buf, int count) {
        _supla_int_t size = client.available();
       
        if ( size > 0 ) {
            if ( size > count ) size = count;
            return client.read((uint8_t *)buf, size);
        };
    
        return -1;
    };
    
    int supla_arduino_tcp_write(void *buf, int count) {
        return client.write((const uint8_t *)buf, count);
    };
    
    bool supla_arduino_svr_connect(const char *server, int port) {
          return client.connect(server, 2015);
    }
    
    bool supla_arduino_svr_connected(void) {
          return client.connected();
    }
    
    void supla_arduino_svr_disconnect(void) {
         client.stop();
    }
    
    void supla_arduino_eth_setup(uint8_t mac[6], IPAddress *ip) {

       // Serial.println("WiFi init");
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
        //    Serial.print(".");
        }

        Serial.print("\nlocalIP: ");
        Serial.println(WiFi.localIP());
        Serial.print("subnetMask: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("gatewayIP: ");
        Serial.println(WiFi.gatewayIP());
    }

SuplaDeviceCallbacks supla_arduino_get_callbacks(void) {
          SuplaDeviceCallbacks cb;
          
          cb.tcp_read = &supla_arduino_tcp_read;
          cb.tcp_write = &supla_arduino_tcp_write;
          cb.eth_setup = &supla_arduino_eth_setup;
          cb.svr_connected = &supla_arduino_svr_connected;
          cb.svr_connect = &supla_arduino_svr_connect;
          cb.svr_disconnect = &supla_arduino_svr_disconnect;
          cb.get_temperature = NULL;
          cb.get_temperature_and_humidity = NULL;
          cb.get_rgbw_value = &get_rgbw_value;
          cb.set_rgbw_value = &set_rgbw_value;
          
          return cb;
}
