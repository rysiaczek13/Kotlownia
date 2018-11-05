#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <SimpleTimer.h>
#define BLYNK_PRINT Serial         // Comment this out to disable prints and save space
#include <BlynkSimpleEsp8266.h>

//pin number where sensor is connected
#define ONEWIRE_PIN D2

const char* ssid = "domeczek";
const char* password = "Olineczek2010";
char auth[] = "5ecc693b3675476e81f2923c128b44be";
float temperature;

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);
SimpleTimer timer;

void setup() {
  Serial.begin(115200);
  delay(10);

  sensors.begin();
  Blynk.begin(auth, ssid, password);
  
  timer.setInterval(2000L, sendTemps);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendTemps()
{
  sensors.requestTemperatures();                  // Polls the sensors.
  temperature = sensors.getTempCByIndex(0);   // Stores temperature. Change to getTempCByIndex(0) for celcius.
  Blynk.virtualWrite(1, temperature);         // Send temperature to Blynk app virtual pin 1.

  Serial.print(temperature);
  Serial.println(F(" 'C"));
}
