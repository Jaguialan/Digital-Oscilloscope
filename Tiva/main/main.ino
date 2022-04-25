/*
  Internet connectivity example (web server)
 Derived from example Sketch by Hans Scharler (http://www.iamshadowlord.com)
*/

#include "Ethernet.h"
#include "webConfig.h"
#include <stdio.h>

// Prototypes
void printHTML();
void printCSS();
void printJs();
void printEthernetData();

EthernetServer server(80);

// Variables

float vpp = 10;
float vp = 5;
float freq = 1000;
float period = 1;

void setup()
{
  Serial.begin(115200);

  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);
  pinMode(PUSH1, INPUT_PULLUP); // released = HIGH, pressed = LOW
  pinMode(PUSH2, INPUT_PULLUP);

  Serial.println("Connecting to Ethernet....");
  // IPAddress ip = IPAddress(192,168,1,129);
  // IPAddress dns = IPAddress(192,168,1,1);
  // IPAddress gw = IPAddress(192,168,1,1);
  // IPAddress mask = IPAddress(255,255,255,0);

  Ethernet.begin(0);
  //  Ethernet.begin(0, ip, dns, gw);

  server.begin();

  printEthernetData();
}

EthernetClient client;

void loop()
{
  client = server.available();

  if (client)
  {                                      // if you get a client,
    Serial.print("new client on port "); // print a message out the serial port
    Serial.println(client.port());
    String currentLine = "";             // make a String to hold incoming data from the client
    boolean newConnection = true;        // flag for new connections
    unsigned long connectionActiveTimer; // will hold the connection start time

    digitalWrite(D1_LED, HIGH);

    while (client.connected())
    { // loop while the client's connected

      digitalWrite(D2_LED, HIGH);

      if (newConnection)
      {                                   // it's a new connection, so
        connectionActiveTimer = millis(); // log when the connection started
        newConnection = false;            // not a new connection anymore
      }
      if (!newConnection && connectionActiveTimer + 1000 < millis())
      {
        // if this while loop is still active 1000ms after a web client connected, something is wrong
        break; // leave the while loop, something bad happened
      }

      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        // This lockup is because the recv function is blocking.
        Serial.print(c);
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
        if (currentLine.endsWith("GET / "))
        {
          Serial.println(webHTML);
          printHTML();
        }
        else if (currentLine.endsWith("GET /styles.css "))
        {
          printCSS();
        }
        else if (currentLine.endsWith("GET /main.js "))
        {
          printJs();
        }
        else if (currentLine.endsWith("GET /vpp_units "))
        {
          client.println(vpp);
        }
        else if (currentLine.endsWith("GET /vp_units "))
        {
          client.println(vp);
        }
        else if (currentLine.endsWith("GET /freq_units "))
        {
          client.println(freq);
        }
        else if (currentLine.endsWith("GET /period_units "))
        {
          client.println(period);
        }
      }
    }
    // close the connection:
    digitalWrite(D2_LED, LOW);
    client.stop();
    // Serial.println("client disonnected");
  }
}

void printHTML()
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Refresh: 5");
  // client.println("Connection: close");
  client.println();
  client.println(webHTML);

  // break out of the while loop:
}

void printCSS()
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/css");
  client.println();
  client.println(webCSS);

  // break out of the while loop:
}

void printJs()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/js");
  client.println();
  client.println(webJs);
}

void printEthernetData()
{
  // print your IP address:
  Serial.println();
  Serial.println("IP Address Information:");
  IPAddress ip = Ethernet.localIP();
  Serial.print("IP Address:\t");
  Serial.println(ip);

  // print your MAC address:

  IPAddress subnet = Ethernet.subnetMask();
  Serial.print("NetMask:\t");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = Ethernet.gatewayIP();
  Serial.print("Gateway:\t");
  Serial.println(gateway);

  // print your gateway address:
  IPAddress dns = Ethernet.dnsServerIP();
  Serial.print("DNS:\t\t");
  Serial.println(dns);
}
