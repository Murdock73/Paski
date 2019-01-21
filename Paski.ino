#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <SimpleTimer.h>
#define ONE_WIRE_BUS D7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char auth[] = "a6e7aaebe53443c6ba4*****************";

SimpleTimer timer;

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, "***************", "***************");

  sensors.begin();

  timer.setInterval(5000L, sendTemps); // Temperature sensor polling interval (5000L = 5 seconds)
}

void sendTemps()
{
  sensors.requestTemperatures(); // Polls the sensors

  float tempread = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address

  Blynk.virtualWrite(4, tempread);
}

void loop()
{
  Blynk.run();
  timer.run();
}
