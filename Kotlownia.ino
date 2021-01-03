#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <SimpleTimer.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#define BLYNK_PRINT Serial  // Comment this out to disable prints and save space
#define ONEWIRE_PIN D6      //pin number where sensor is connected
#define RELAY_PIN D7        //pin number where relay is connected
#define OLED_RESET -1

Adafruit_SSD1306 display(OLED_RESET);

const char* ssid = "SSID";
const char* password = "password";
char auth[] = "5ecc693b3675476e81f2923c128b44be";

float maxTemp = 45.0;
float histereza = 7.0;
float minTemp = maxTemp-histereza;
float tempZasobnika;
float tempPowrotuCWU;
float tempSolarna;

bool emailSendedToReceiver = false;

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

// Addresses of 3 DS18B20s
uint8_t sensorZasobnika[8] = { 0x28, 0x55, 0x6A, 0x56, 0xB5, 0x01, 0x3C, 0xF6 };
uint8_t sensorPowrotuCWU[8] = { 0x28, 0x17, 0x4F, 0x56, 0xB5, 0x01, 0x3C, 0xC7 };
uint8_t sensorSolarny[8] = { 0x28, 0xFF, 0x57, 0x09, 0xA3, 0x16, 0x03, 0xA9 };

SimpleTimer timer;

void setup() {
  Serial.begin(115200);
  sensors.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  delay(10);

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
  tempZasobnika = getTemperature(sensorZasobnika);
  tempPowrotuCWU = getTemperature(sensorPowrotuCWU);
  tempSolarna = getTemperature(sensorSolarny);
  setRelay(tempZasobnika);
  Blynk.virtualWrite(1, tempZasobnika);         // Send temperature to Blynk app virtual pin 1.
  Blynk.virtualWrite(2, tempPowrotuCWU);         // Send temperature to Blynk app virtual pin 2.
  Blynk.virtualWrite(3, tempSolarna);         // Send temperature to Blynk app virtual pin 3.
  SendEmailIfNeeded(tempZasobnika);
  displayTemp(tempZasobnika);
  // Serial.print("Temperatura zasobnika: ");
  // Serial.print(tempZasobnika);
  // Serial.println(F(" 'C "));

  // Serial.print("Temperatura powrotu CWU: ");
  // Serial.print(tempPowrotuCWU);
  // Serial.println(F(" 'C "));

  // Serial.print("Temperatura solarna: ");
  // Serial.print(tempSolarna);
  // Serial.println(F(" 'C "));
}

void SendEmailIfNeeded(float temp) {
  if(temp <= (minTemp - 5) && !emailSendedToReceiver)
  {
    Blynk.email("email_address", "Alert Temperatury w zasobniku", "Temperatura w zasobniku spadła poniżej 32 stopni.");
    emailSendedToReceiver = true;
  }

  if(temp >= maxTemp && emailSendedToReceiver)
  {
    Blynk.email("email_address", "Temperatura w zasobniku w normie", "Temperatura w zasobniku wróciła powyżej 42 stopni.");
    emailSendedToReceiver = false;
  }
}

void setRelay(float temp) {
  if(temp >= maxTemp)
  {
    // Serial.print("R1: ");
    // Serial.println(temp);
    digitalWrite(RELAY_PIN, HIGH);
  }

  if(temp <= minTemp)
  {
    // Serial.print("R2: ");
    // Serial.println(temp);
    digitalWrite(RELAY_PIN, LOW);
  }
}

float getTemperature(DeviceAddress deviceAddress)
{
  return sensors.getTempC(deviceAddress);
}

void displayTemp(float temp) 
{
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(5,30);
  display.println(temp);
  display.display();
}