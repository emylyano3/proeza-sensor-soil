#include "main.h"
#include <ESPDomotic.h>
#include <Logger.h>

#define WATER_VALUE 660
#define AIR_VALUE   1024

#ifdef ESP01
// usable pins are GPIO2 and GPIO as output (both must be pulled up to boot normal )
// If using SERIAL_TX_ONLY over GPIO1, GPIO3 becomes available
const uint8_t SENSOR_PIN  = 3;
const uint8_t RELAY_PIN   = 2;
const uint8_t TX_PIN      = 1;
#elif NODEMCUV2
// usable pins D0,D1,D2,D5,D6,D7 (D10 is TX (GPIO1), D9 is RX (GPIO3), D3 is GPIO0, D4 is GPIO2, D8 is GPIO15)
const uint8_t ANALOG_SENSOR_PIN_1 = A0;
#endif

int channelStateMapper(int original);

ESPDomotic*  domoticModule  = new ESPDomotic();
Channel*     channel        = new Channel("ss-1", "soilsens-01", ANALOG_SENSOR_PIN_1, INPUT, true, 600000); //10 min

void setup() {
#ifdef ESP01
  //to avoid using pin 0 as input
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY, TX_PIN);
#else
  Serial.begin(115200);
#endif
  delay(500);
  Serial.println();
  log(F("Starting module"));
  channel->setState(LOW); 
  channel->setEnabled(true);
  channel->setChannelStateMapCallback(channelStateMapper);
  domoticModule->setModuleType("sensor");
  domoticModule->setWifiConnectTimeout(20);
  domoticModule->setConfigPortalTimeout(120);
  domoticModule->setPortalSSID("proeza-soilsense");
  domoticModule->addChannel(channel);
  domoticModule->saveConfigCallback();
  domoticModule->init();
  channel->startTimer();
}

int channelStateMapper(int original) {
  return map(original, AIR_VALUE, WATER_VALUE, 0, 100);
}

void loop() {
  domoticModule->cycle();
}
