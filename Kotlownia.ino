#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial         // Comment this out to disable prints and save space
#define ONEWIRE_PIN D6             //pin number where sensor is connected

const char* ssid = "SSID";
const char* password = "Password";
char auth[] = "5ecc693b3675476e81f2923c128b44be";
float temperature;
bool emailSendedToReceiver = false;

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
  sensors.requestTemperatures();              // Polls the sensors.
  temperature = sensors.getTempCByIndex(0);   // Stores temperature. Change to getTempCByIndex(0) for celcius.
  Blynk.virtualWrite(1, temperature);         // Send temperature to Blynk app virtual pin 1.
  SendEmailIfNeeded();
  Serial.print(temperature);
  Serial.println(F(" 'C"));
}

void SendEmailIfNeeded() {
  if(temperature <= 35.0 && !emailSendedToReceiver)
  {
    Blynk.email("email_address", "Alert Temperatury w zasobniku", "Temperatura w zasobniku spadła poniżej 32 stopni.");
    emailSendedToReceiver = true;
  }

  if(temperature >= 45.0 && emailSendedToReceiver)
  {
    Blynk.email("email_address", "Temperatura w zasobniku w normie", "Temperatura w zasobniku wróciła powyżej 42 stopni.");
    emailSendedToReceiver = false;
  }
}
