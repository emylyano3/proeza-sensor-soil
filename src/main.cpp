#include <ESPDomotic.h>
#include <string>

#define WATER_VALUE 670 
#define AIR_VALUE   1024
#define LOOP_DELAY  6000

#ifdef ESP01
// usable pins are GPIO2 and GPIO as output (both must be pulled up to boot normal )
// If using SERIAL_TX_ONLY over GPIO1, GPIO3 becomes available
const uint8_t SENSOR_PIN  = 3;
const uint8_t RELAY_PIN   = 2;
const uint8_t TX_PIN      = 1;
#elif NODEMCUV2
// usable pins D0,D1,D2,D5,D6,D7 (D10 is TX (GPIO1), D9 is RX (GPIO3), D3 is GPIO0, D4 is GPIO2, D8 is GPIO15)
const uint8_t ANALOG_SENSOR_PIN_1 = A0;
const uint8_t LED_PIN             = D0;
#endif

int channelStateMapper(int original);

Channel     _sensorChannel ("ss-1", "soil-sensor-1", ANALOG_SENSOR_PIN_1, INPUT, AIR_VALUE, true, 1000);

ESPDomotic  _domoticModule;

template <class T> void log (T text) {
  #ifdef LOGGING
  Serial.print(F("*SS: "));
  Serial.println(text);
  #endif
}

template <class T, class U> void log (T key, U value) {
  #ifdef LOGGING
  Serial.print(F("*SS: "));
  Serial.print(key);
  Serial.print(F(": "));
  Serial.println(value);
  #endif
}


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
  String ssid = "Sensor " + String(ESP.getChipId());
  _sensorChannel.setStateMapper(channelStateMapper);
  _domoticModule.setPortalSSID(ssid.c_str());
  #ifndef ESP01
  _domoticModule.setFeedbackPin(LED_PIN); // PIN 0 can be used as output
  #endif
  _domoticModule.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT);
  _domoticModule.setWifiConnectTimeout(WIFI_CONNECT_TIMEOUT);
  _domoticModule.setConfigFileSize(CONFIG_FILE_SIZE);
  _domoticModule.setModuleType("sensor");
  _domoticModule.addChannel(&_sensorChannel);
  _domoticModule.init();
}

int channelStateMapper(int original) {
  uint32_t mapped = map(original, AIR_VALUE, WATER_VALUE, 0, 100);
  return static_cast<int>(mapped);
}

void loop() {
  _domoticModule.loop();
}
