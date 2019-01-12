/*********
Paski.ino
*********/

// Including the ESP8266 WiFi library
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network details
const char* ssid = "***********";
const char* password = "***********";

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String HvacState = "HIGH";

#define ONE_WIRE_BUS D1      // Data wire GPIO pin to use. GPIO5 (D1)
const uint16_t Switch = D2;  // ESP8266 GPIO pin to use. GPIO4 (D2).


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];

// Web Server on port 80
WiFiServer server(80);

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  
  pinMode(Switch, OUTPUT);
  digitalWrite(Switch, HIGH);
  
  DS18B20.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(5000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

void getTemperature() {
  float tempC;
  float tempF;
  do {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the Rele on and off
            if (header.indexOf("GET /1/on") >= 0) {

              Serial.println("AC is on");
              HvacState = "on";
              Serial.println("Accendo...");
              digitalWrite(Switch, LOW);
              
            } else if (header.indexOf("GET /1/off") >= 0) {
              
              Serial.println("AC is off");
              HvacState = "off";
              Serial.println("Spengo...");
              digitalWrite(Switch, HIGH);
                                
           }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}");
            client.println(".button3 {background-color: #0000FF;}");
            client.println(".button4 {background-color: #CC0000;}");
            client.println(".button5 {background-color: #0066CC;}</style></head>");

                        
            // Web Page Heading
            client.println("<body><h1>Riscaldamento di casa Gaeta</h1>");

            getTemperature();
            client.println("<p>Mo' ci stanno in casa </p>");
            client.println("<p><a href=\"/2/on\"><button class=\"button button5\">");
            client.println(temperatureCString);
            client.println("</button></a></p>");

            client.println("<p>'O riscaldamento sta' </p>");
                                                            
            // Display current state, and ON/OFF button  
            if (HvacState=="off") {
              client.println("<p><a href=\"/1/on\"><button class=\"button button2\">SPENT</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button\">APPICCIATO</button></a></p>");
            } 
               
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}   
