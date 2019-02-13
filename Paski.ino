#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <SimpleTimer.h>
#define ONE_WIRE_BUS D7
#define heat D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int Button = 0;
int Temp = 0;
int Timer = 0;
float tempread = 0;


//char auth[] = "d342eb62c8cb45388f45a2286466f7d4";
char auth[] = "a6e7aaebe53443c6ba43a9ae7e9d6dd2";

SimpleTimer timer;

void setup()
{
  Serial.begin(9600);
  pinMode(heat, OUTPUT);
  //Blynk.begin(auth, "Vodafone-33779956", "azj7i569969laue");
  Blynk.begin(auth, "Vodafone-skynet", "Rickyale.73");
  digitalWrite(heat, HIGH);
  sensors.begin();

  timer.setInterval(5000L, sendTemps); // Temperature sensor polling interval (5000L = 5 seconds)
}

void sendTemps()
{
  sensors.requestTemperatures(); // Polls the sensors

  tempread = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address

  Blynk.virtualWrite(0, tempread);

  if (Button == 1 &&  Timer == 1 && tempread < Temp) {
    digitalWrite(heat, LOW);
    Serial.println("Entra");
  }
  else {
    digitalWrite(heat, HIGH);
  }

}

BLYNK_WRITE(V1)
{
  // You'll get HIGH/1 when Button is ON and LOW/0 when OFF
  Serial.print("Button: ");
  Serial.println(param.asStr());
  Button = param.asInt();
}

BLYNK_WRITE(V2)
{
  // You'll get HIGH/1 at startTime and LOW/0 at stopTime.
  // this method will be triggered every day
  // until you remove widget or stop project or
  // clean stop/start fields of widget
  Serial.print("Timer: ");
  Serial.println(param.asStr());
  Timer = param.asInt();
  if (Timer == 0) {
    Blynk.virtualWrite(1, 0); 
  }
}

BLYNK_WRITE(V3)
{
  // You'll get the target temp value from the slider
  Serial.print("Slider: ");
  Serial.println(param.asStr());
  Temp = param.asInt();
}

void loop()
{
  Blynk.run();
  timer.run();

}
