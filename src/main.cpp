/*
this code is intended for the ESP32
9/18/2019 works at the office on the WEMOS LOLIN32

Attaches to NPT time server to set clock.
Controls blower motor
sends emails and texts
 A simple web server that serves a webpage showing start time, run time, remaining times

 This sketch will print the IP address of your WiFi connection (once connected)
 to the Serial monitor. From there, you can open that address in a web browser.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 *  LED_BUILTIN attached to pin 5 on the Wemos, pin 13 on the Feather
 *  Blower Motor solid state relay attached to pin 33.
 *  Start button is attached to pin 32.
 */

#include <time.h> // ESP32 native time library, does graceful NTP server synchronization
// removed the lib and commented this line #include <TimeLib.h> // TimeLib.h is the Arduino time library. We use it because it is better than time.h for tracking current time and reacting to it.

#include <WiFi.h>
#include "WiFiInfo.h" // stroes the SSID and PASSWORD in seperate file so it will not be uploaded to github
                      // this file can be included in a folder by the same name in the libraries folder
#include <WiFiMulti.h> // Connect to the best AP based on a given list

#include "Gsender_32.h"  // supoprt for smtp mail module. This library must be placed in the library folder

#include <U8x8lib.h> // for the Heltec WiFi LoRa 32 builtin OLED
// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

WiFiMulti wifiMulti;  // create and instance of WiFiMulti called wifiMulti

boolean connectioWasAlive = true;

struct tm tmstruct;

#define TOUCH_PIN T5 // ESP32 Pin D12
int touch_value = 100; 
bool touch_state = false; // set state for touch sensor

WiFiServer server(80);  // set port for WiFiServer

// Variable to store the HTTP request
String header;

// Auxiliary variables to store the current output state
String led_state = "off"; // set state for the built in led
String blower_state = "off"; // set state for the blowerMotor

// constants won't change:
String machineNumber = "AirSampler1"; // using a constant String
String sketchName = "AirSamplerPIO"; // set the sketch name
const int led = LED_BUILTIN; //LED on pin 5 for the Wemos, pin 13 on the Feather.
const int blowerMotor = 33; // Solid state relay for the bolower motor is connected to this ping
const int startButton = 32; // Start button connected to this pin

int startTime;

const	int chipid = ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).

long timezone = -8; // Pacific time zone
byte daysavetime = 0; // 1 specifies daylight savings time 0 specifies standard time

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

const long interval = 1000;           // interval at which to blink (milliseconds)

String address[] = {"dsloan@ibeinc.com", "8187267067@txt.att.net"}; // emails to send to

void connectWiFi(){

  wifiMulti.addAP(ssid_from_AP_1, your_password_for_AP_1); // this is defined in the the WiFiInfo.h file
  wifiMulti.addAP(ssid_from_AP_2, your_password_for_AP_2); // this is defined in the the WiFiInfo.h file
  wifiMulti.addAP(ssid_from_AP_3, your_password_for_AP_3); // this is defined in the the WiFiInfo.h file

  Serial.print("Connecting Wifi to ");
  Serial.print(ssid_from_AP_1);
  Serial.print(", "),   
  Serial.print(ssid_from_AP_2);   
  Serial.print(", or "),   
  Serial.println(ssid_from_AP_3);   
  Serial.print("Looking for WiFi ");
  u8x8.clearDisplay();
  u8x8.drawString(0, 0, "Looking for WiFi");
  u8x8.setCursor(0,2);

  while(wifiMulti.run() != WL_CONNECTED){ // Wait for the Wi-Fi to connect: scan for Wi-Fi networks,
                                          // and connect to the strongest of the networks above
    Serial.print(".");
    u8x8.print(".");
    delay(250);
  }
  digitalWrite(led,LOW); // turn off LED after WiFi is connected
  Serial.println("");
  Serial.printf("WiFi connected to %s\n", WiFi.SSID().c_str());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // send the ip address assigned by the DHCP server
  
  String WiFilocalIP = WiFi.localIP().toString();
  //u8x8.setFont(u8x8_font_courB24_3x4_n);
  u8x8.clearDisplay();
  u8x8.draw1x2String(0, 0, (char *) WiFilocalIP.c_str()); // display the ip address assigned by the DHCP server on the OLED
  Serial.println(WiFilocalIP); // send the ip address assigned by the DHCP server

  Serial.print("Signal strength: ");
  Serial.println(WiFi.RSSI());
}

void setTimeFromNTP(){
 //show the local time
  //tmstruct.tm_year = 0;
  Serial.printf("\nTime before contacting the time server: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
	
  //set the local time from the time Server
  Serial.println("Contacting Time Server");
  configTime(3600*timezone, daysavetime*3600, "north-america.pool.ntp.org", "time.nist.gov", ""); // get date and time from time server
  
  getLocalTime(&tmstruct, 5000);  // I think this reterives the time from the ESP32
  if(!getLocalTime(&tmstruct)){
    Serial.println("Failed to obtain time");}
  Serial.printf("Time after contacting the time server: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
  Serial.println("");
}

void sendStartConfirmation(){
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  String subject = machineNumber; // subject line of the text
  String body = "Starting " + WiFi.localIP().toString();  // This is the cotents for the text message. Must use the .toString() or it crashes

    if (gsender->Subject(subject)->Send(2, address, body)) {
        Serial.println("Message sent.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender->getError());
    }
}  

void setup()
{
  Serial.begin(115200); // set up the serial port
  while (!Serial); // wait for the serial port to initialize

  u8x8.begin(); // start the builtin OLED
  u8x8.setFont(u8x8_font_chroma48medium8_r); // set the font on the OLED
  u8x8.drawString(0, 1, "LoRa Receiver"); // display string on the OLED

  btStop();   // turn off the blue tooth module to save power

  pinMode(led, OUTPUT);      // set the LED pin mode
  digitalWrite(led, HIGH);  // turn on the LED until the WiFi is connected  

  pinMode(blowerMotor, OUTPUT);    // set the blowerMotor pin mode
  digitalWrite(blowerMotor, LOW);  // turn off the the blowerMotor

  pinMode(startButton, INPUT);    // set the startButton pin mode
 
  Serial.println();
  Serial.println("ESP32 Touch Test T5 ESP32 Pin D12");
  Serial.print("Sketch is ");
  Serial.println(sketchName);
  Serial.println(machineNumber.c_str());
  Serial.println();
  Serial.printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));//print High 2 bytes of the chip id
	Serial.printf("%08X\n",(uint32_t)chipid);//print Low 4bytes of the chip id.
  Serial.println(chipid); // print out the 

  WiFi.setHostname(machineNumber.c_str()); // needed to set hostName on router

  Serial.setDebugOutput(true); // sends additional info to the serial port

  connectWiFi();
  Serial.println();

  setTimeFromNTP();
 
  //sendStartConfirmation();

  server.begin();
}

void loop(){

  // Health check. If we've lost our wifi connection, restart
  //  the ESP to (hopefully) reconnect without user noticing.
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi dropped; rebooting");
    delay(100);
    ESP.restart();
  }

//  time_t now;
//  Serial.println(time(&now));

  touch_value = touchRead(TOUCH_PIN);  // get value using T5 

  if (touch_value < 50 and touch_state == false){
    Serial.print("Touching. The time is");
    getLocalTime(&tmstruct, 5000);  // I think this reterives the time from the ESP32
    if(!getLocalTime(&tmstruct)){
      Serial.println("Failed to obtain time");}
    Serial.printf(": %d-%02d-%02d %02d:%02d:%02d Standard Time\n",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday,tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
    
    char strd[13];
    sprintf( strd, "%d-%02d-%02d",(tmstruct.tm_year)+1900,( tmstruct.tm_mon)+1, tmstruct.tm_mday);
    Serial.print( strd );
    u8x8.drawString(0, 3, strd);
    char strt[15];
    sprintf( strt, "%02d:%02d:%02d", tmstruct.tm_hour , tmstruct.tm_min, tmstruct.tm_sec);
    Serial.print( strt );
    u8x8.draw1x2String(0, 5, strt);

//    Serial.println(millis()/1000);
    touch_state = true;
  }
  else if(touch_value > 50 and touch_state == true ){
    Serial.print("Not touching. The value is ");
    Serial.println(touch_value);
    touch_state = false;
  }
  
  if (startButton == true){
    digitalWrite(blowerMotor, LOW);  // turn on the the blowerMotor
    //! this needs to bet set to now   startTime = ;
    Serial.println();
  }


  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write ("This is what we got from the client ");
        Serial.write(c);                    // print it out the serial monitor
        header += c;                        // add the character read to the header string
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("Blower is on");
              blower_state = "on";
              digitalWrite(blowerMotor, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("Blower is off");
              blower_state = "off";
              digitalWrite(blowerMotor, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("LED on");
              led_state = "on";
              digitalWrite(led, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("LED off");
              led_state = "off";
              digitalWrite(led, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.print("<title>IBE ESP32</title>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<body><h1>" + machineNumber + "</h1>");
            
            // Display current state, and ON/OFF buttons for Blower motor  
            client.println("<p>Blower motor " + blower_state + "</p>");
            // If the blower_state is off, it displays the ON button       
            if (blower_state=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for LED  
            client.println("<p>LED " + led_state + "</p>");
            // If the output27State is off, it displays the ON button       
            if (led_state=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop

            //client.print("<h1> Click <a href=\"/H\">here</a> to turn the LED on<br>");
            //client.print("Click <a href=\"/L\">here</a> to turn the LED off.</h1><br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}