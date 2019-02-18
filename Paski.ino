#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <SimpleTimer.h>
#include <EEPROM.h>

#define ONE_WIRE_BUS D7
#define heat D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int addr = 0;
int Button = 0;
int Temp = 0;
int Timer = 0;
float tempread = 0;
float offset = 0,3;

//char auth[] = "d342eb62c8cb453****************";
char auth[] = "a6e7aaebe53443************";

SimpleTimer timer;

void setup()
{
  Serial.begin(9600);
  pinMode(heat, OUTPUT);
  //Blynk.begin(auth, "Vodafone*********", "***********");
  Blynk.begin(auth, "Vodafone-************", "***********");
  digitalWrite(heat, HIGH);
  sensors.begin();
  EEPROM.begin(512);

  timer.setInterval(5000L, sendTemps); // Temperature sensor polling interval (5000L = 5 seconds)
}

void sendTemps()
{
  sensors.requestTemperatures(); // Polls the sensors

  tempread = sensors.getTempCByIndex(0); // Gets first probe on wire in lieu of by address

  Blynk.virtualWrite(0, tempread);
  Temp = EEPROM.read(address);

  if (Button == 1 &&  Timer == 1) {
    Serial.println("Entra");
    val = digitalRead(heat);
    // riscaldamento acceso
    if (val = LOW) {
      // temperatura superiore alla soglia spengo
      if (tempread >= Temp) {
        digitalWrite(heat, HIGH);
        Serial.println("Temperatura raggiunta, spengo");
      }
      // temperatura inferiore alla soglia resta acceso
    }
      
    // riscaldamento spento
    if (val = HIGH) {
      // temperatura inferiore alla soglia - offset: accendo
      if (tempread <= Temp - offset) {
        digitalWrite(heat, LOW);
        Serial.println("Temperatura - offeset non raggiunta, accendo");
      }
      // temperatura superiore alla soglia resta spento
    }
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
  // Salvo la temperatura in EEPROM
  EEPROM.write(addr, Temp);
  EEPROM.commit();
}

void loop()
{
  Blynk.run();
  timer.run();

}
