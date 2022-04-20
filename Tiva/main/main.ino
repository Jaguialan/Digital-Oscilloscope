/*
  Internet connectivity example (web server)
 Derived from example Sketch by Hans Scharler (http://www.iamshadowlord.com)
*/

#include "Ethernet.h"
#include "webConfig.h"

uint16_t sine[640] = {2048,2068,2088,2108,2128,2148,2168,2188,2208,2228,2248,2268,2288,2308,2328,2348,2368,2388,2407,2427,2447,2467,2486,2506,2525,2545,2564,2584,2603,2623,2642,2661,2680,2699,2718,2737,2756,2775,2794,2812,2831,2850,2868,2886,2905,2923,2941,2959,2977,2995,3013,3030,3048,3065,3083,3100,3117,3134,3151,3168,3185,3202,3218,3235,3251,3267,3283,3299,3315,3331,3346,3362,3377,3392,3408,3423,3437,3452,3467,3481,3495,3509,3523,3537,3551,3565,3578,3591,3604,3617,3630,3643,3655,3668,3680,3692,3704,3716,3727,3739,3750,3761,3772,3783,3793,3804,3814,3824,3834,3844,3853,3863,3872,3881,3890,3898,3907,3915,3923,3931,3939,3947,3954,3961,3968,3975,3982,3988,3995,4001,4007,4013,4018,4023,4029,4034,4038,4043,4047,4052,4056,4059,4063,4067,4070,4073,4076,4078,4081,4083,4085,4087,4089,4090,4091,4093,4093,4094,4095,4095,4095,4095,4095,4094,4093,4093,4091,4090,4089,4087,4085,4083,4081,4078,4076,4073,4070,4067,4063,4059,4056,4052,4047,4043,4038,4034,4029,4023,4018,4013,4007,4001,3995,3988,3982,3975,3968,3961,3954,3947,3939,3931,3923,3915,3907,3898,3890,3881,3872,3863,3853,3844,3834,3824,3814,3804,3793,3783,3772,3761,3750,3739,3727,3716,3704,3692,3680,3668,3655,3643,3630,3617,3604,3591,3578,3565,3551,3537,3523,3509,3495,3481,3467,3452,3437,3423,3408,3392,3377,3362,3346,3331,3315,3299,3283,3267,3251,3235,3218,3202,3185,3168,3151,3134,3117,3100,3083,3065,3048,3030,3013,2995,2977,2959,2941,2923,2905,2886,2868,2850,2831,2812,2794,2775,2756,2737,2718,2699,2680,2661,2642,2623,2603,2584,2564,2545,2525,2506,2486,2467,2447,2427,2407,2388,2368,2348,2328,2308,2288,2268,2248,2228,2208,2188,2168,2148,2128,2108,2088,2068,2048,2027,2007,1987,1967,1947,1927,1907,1887,1867,1847,1827,1807,1787,1767,1747,1727,1707,1688,1668,1648,1628,1609,1589,1570,1550,1531,1511,1492,1472,1453,1434,1415,1396,1377,1358,1339,1320,1301,1283,1264,1245,1227,1209,1190,1172,1154,1136,1118,1100,1082,1065,1047,1030,1012,995,978,961,944,927,910,893,877,860,844,828,812,796,780,764,749,733,718,703,687,672,658,643,628,614,600,586,572,558,544,530,517,504,491,478,465,452,440,427,415,403,391,379,368,356,345,334,323,312,302,291,281,271,261,251,242,232,223,214,205,197,188,180,172,164,156,148,141,134,127,120,113,107,100,94,88,82,77,72,66,61,57,52,48,43,39,36,32,28,25,22,19,17,14,12,10,8,6,5,4,2,2,1,0,0,0,0,0,1,2,2,4,5,6,8,10,12,14,17,19,22,25,28,32,36,39,43,48,52,57,61,66,72,77,82,88,94,100,107,113,120,127,134,141,148,156,164,172,180,188,197,205,214,223,232,242,251,261,271,281,291,302,312,323,334,345,356,368,379,391,403,415,427,440,452,465,478,491,504,517,530,544,558,572,586,600,614,628,643,658,672,687,703,718,733,749,764,780,796,812,828,844,860,877,893,910,927,944,961,978,995,1012,1030,1047,1065,1082,1100,1118,1136,1154,1172,1190,1209,1227,1245,1264,1283,1301,1320,1339,1358,1377,1396,1415,1434,1453,1472,1492,1511,1531,1550,1570,1589,1609,1628,1648,1668,1688,1707,1727,1747,1767,1787,1807,1827,1847,1867,1887,1907,1927,1947,1967,1987,2007,2027};
uint16_t sineIndex = 0;

// Prototypes
void printHTML();
void printCSS();
void printJs();
void printEthernetData();
void updateHTML(bool, uint8_t);

EthernetServer server(80);

// Variables

uint16_t vpp = 10;    
uint16_t vp = 5;     
uint16_t freq = 1000;   
uint16_t period = 1; 

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
          delay(1);
          printHTML();
          delay(1);
        }
        else if (currentLine.endsWith("GET /styles.css "))
        {
          delay(1);
          printCSS();
          delay(1);
        }
        else if (currentLine.endsWith("GET /main.js "))
        {
          delay(1);
          printJs();
          delay(1);
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
  //client.println("Refresh: 10");
  //client.println("Connection: close"); 
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
