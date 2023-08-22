
#ifndef mqtt_h
#define mqtt_h

#include <Arduino.h>


#define Serial Serial3

//----------------------------------MQTT SETUP---------------------------------------------------

#include <SPI.h>
#include "ethernetDefines.h"
#include <PubSubClient.h>

#define SSpin PIN_PB5
#define MOSIpin PIN_PC0
#define MISOpin PIN_PC1
#define SCKpin PIN_PC2

#define macNumber 1 //pick a mac from the table... 1-20

// comment out to use DHCP, otherwise use addresses below...
#define FIXED_IP

// comment out to use host vs ip address, specifiy below...
#define MQTT_IP

// MQTT Reconnect minimum interval
#define MQTT_INTERVAL 5000UL
// Interval for no power report
#define NO_POWER_INTERVAL 30000UL


#ifdef FIXED_IP
const IPAddress ip2(192, 168, 77, 69);
const IPAddress dns2(8, 8, 8, 8);
const IPAddress gateway2(192,168,77,1);
#endif

//MQTT server settings
#ifdef MQTT_IP
const IPAddress mqttServer(192, 168, 77, 1);
#else
const char mqttServer[] = "www.mqtt.com";
#endif
const uint16_t mqttPort = 1883;

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void mqttSetup();
void mqttReport();
void noPower();
void mqttLoop();


#endif