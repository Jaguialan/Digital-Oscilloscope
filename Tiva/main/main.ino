#include <Arduino.h>
#include <Energia.h>

#include "Ethernet.h"
#include "webConfig.h"
#include <stdio.h>

/////////////////////////////////////////////////
////////////////// DEFINITIONS //////////////////
/////////////////////////////////////////////////

#define RATIO 0.012992

/////////////////////////////////////////////////
/////////////////// PROTOTYPES //////////////////
/////////////////////////////////////////////////

void printHTML();
void printCSS();
void printJs();
void printEthernetData();

EthernetServer server(80);

/////////////////////////////////////////////////
/////////////////// VARIABLES ///////////////////
/////////////////////////////////////////////////

float vpp = 10;
float vp = 5;
float freq = 1000;
float period = 1;

void setup()
{
  Serial.begin(115200);
  Serial6.begin(115200);

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
  {
#if PRINT_IP_CONFIG
    // if you get a client,
    Serial.print("new client on port "); // print a message out the serial port
    Serial.println(client.port());
#endif
    String currentLine = "";             // make a String to hold incoming data from the client
    boolean newConnection = true;        // flag for new connections
    unsigned long connectionActiveTimer; // will hold the connection start time

    digitalWrite(D1_LED, HIGH);

    while (client.connected())
    { // loop while the client's connected
      Serial6.flush();
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
#if PRINT_IP_CONFIG
        Serial.print(c);
#endif
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

void printCSS()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/css");
  client.println();
  client.println(webCSS);
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

void printHTML()
{
  uint8_t freqInt = 0;
  uint8_t freqDec = 0;
  uint8_t inByte;

  while (Serial6.available() > 0)
  {
    inByte = Serial6.read();
    if (inByte == 0b11111111)
    { // Header byte

      vpp = Serial6.read();
      freqInt = Serial6.read();
      freqDec = Serial6.read();
      Serial.print("Vpp: ");
      Serial.println(vpp);
      Serial.print("Freq: ");
      Serial.print(freqInt);
      Serial.print(".");
      Serial.println(freqDec);
      Serial6.flush();
      break;
    }
  }
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Refresh: 5");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"en-US\" encoding=\"UTF-16\">");
  client.println("");
  client.println("<head>");
  client.println("<title>Digital Oscilloscope</title>");
  client.println("<link rel=\"stylesheet\" href=\"styles.css\">");
  client.println("<script src=\"main.js\" defer></script>");
  client.println("</head>");
  client.println("");
  client.println("<body>");
  client.println("<section class=\"mainContent\">");
  client.println("<div class=\"header\">");
  client.println("<br>");
  client.println("<h4>Proyectos Experimentales II</h4>");
  client.println("<h1>OSCILOSCOPIO DIGITAL</h1>");
  client.println("<br>");
  client.println("<hr>");
  client.println("</div>");
  client.println("<div class=\"oscMainBody\">");
  client.println("<div class=\"oscValsLeft\">");
  client.println("");
  client.println("<h1 class=\"mainText\">VALORES</h1>");
  client.println("");
  client.println("<div class=\"dataField\">");
  client.println("<p style=\"display: inline-block\" id=\"Vpp\">");
  client.print("Vpeak-peak: ");
  client.println(vpp * RATIO);
  client.println("</p>");
  client.println("<select class=\"dropDown\" onchange=\"update_vpp(event)\">");
  client.println("<option>mVpp</option>");
  client.println("<option>Vpp</option>");
  client.println("</select>");
  client.println("</div>");
  client.println("");
  client.println("<div class=\"dataField\">");
  client.println("<p style=\"display: inline-block\" id=\"Vp\">");
  client.print("Vpeak: ");
  vp = vpp / 2;
  client.println(vp * RATIO);
  client.println("</p>");
  client.println("<select class=\"dropDown\" onchange=\"update_vp(event)\">");
  client.println("<option>mVp</option>");
  client.println("<option>Vp</option>");
  client.println("</select>");
  client.println("</div>");
  client.println("");
  client.println("<div class=\"dataField\">");
  client.println("<p style=\"display: inline-block\" id=\"Freq\">");
  client.print("Frequency: ");
  freq = freqInt + 0.01 * freqDec;
  client.println(freq);
  client.println("</p>");
  client.println("<select class=\"dropDown\" onchange=\"update_freq(event)\">");
  client.println("<option>Hz</option>");
  client.println("<option>kHz</option>");
  client.println("</select>");
  client.println("</div>");
  client.println("");
  client.println("<div class=\"dataField\">");
  client.println("<p style=\"display: inline-block\" id=\"Period\">");
  client.print("Period: ");
  period = 1 / freq;
  client.println(period);
  client.println("</p>");
  client.println("<select class=\"dropDown\" onchange=\"update_period(event)\">");
  client.println("<option>ms</option>");
  client.println("<option>s</option>");
  client.println("</select>");
  client.println("</div>");
  client.println("</div>");
  client.println("");
  client.println("<div class=\"oscValsCenter\">");
  client.println("<h1>ESCALA DE TIEMPO</h1>");
  client.println("<div>");
  client.println("<p style=\"display: inline-block\">");
  client.println("<button type=\"button\" class=\"buttonCase\" id=\"incTimeScale\">&#129045 s/div</button>");
  client.println("</p>");
  client.println("");
  client.println("</div>");
  client.println("<div>");
  client.println("<p style=\"display: inline-block\">");
  client.println("<button type=\"button\" class=\"buttonCase\" id=\"decTimeScale\">&#129047 s/div</button>");
  client.println("</p>");
  client.println("</div>");
  client.println("");
  client.println("</div>");
  client.println("<div class=\"oscValsRight\">");
  client.println("<h1>ESCALA DE VOLTAJE</h1>");
  client.println("<div>");
  client.println("<p style=\"display: inline-block\">");
  client.println("<button type=\"button\" class=\"buttonCase\" id=\"incVScale\">&#129045 V/div</button>");
  client.println("</p>");
  client.println("");
  client.println("</div>");
  client.println("<div>");
  client.println("<p style=\"display: inline-block\">");
  client.println("<button type=\"button\" class=\"buttonCase\" id=\"decVScale\">&#129047 V/div</button>");
  client.println("</p>");
  client.println("</div>");
  client.println("</div>");
  client.println("</div>");
  client.println("<div class=\"oscValsRight\">");
  client.println("<a style=\"border: 1px solid; padding: 4.5px; white\" onclick=\"update_vpp(event); update_vp(event); update_freq(event); update_period(event);\">Refrescar valores</a> <br>");
  client.println("</div>");
  client.println("<footer class=\"footer\">");
  client.println("<div>");
  client.println("<h6 id=\"update_all_vars\" onload=\"update_all()\">&copy Javier L&aacutezaro Fern&aacutendez &amp; Julia Uruel Sanz - MISEA 2022 - M&AacuteSTER EN");
  client.println("INGENIER&IacuteA DE SISTEMAS ELECTR&OacuteNICOS Y APLICACIONES</h6>");
  client.println("</div>");
  client.println("");
  client.println("<script>");
  client.println("function update_all(){");
  client.println("update_vpp();");
  client.println("update_vp();");
  client.println("update_freq();");
  client.println("update_period();");
  client.println("alert(\"no funsiona\");");
  client.println("}");
  client.println("</script>");
  client.println("</footer>");
  client.println("</section>");
  client.println("</body>");
  client.println("");
  client.println("</html>");
  Serial6.flush();
}
