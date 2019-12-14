#include <Wire.h>             // Using library Wire at version 1.0 in folder:
                              // C:\Users\<username>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.1\libraries\Wire
#include <Adafruit_GFX.h>     // https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h> // https://github.com/mcauser/Adafruit_SSD1306/tree/esp8266-64x48
#include <ESP8266WiFi.h>      // Using library ESP8266WiFi at version 1.0 in folder: C:\Users\<username>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.1\libraries\ESP8266WiFi 
#include <PubSubClient.h>     // http://downloads.arduino.cc/libraries/github.com/knolleary/PubSubClient-2.7.0.zip

const char* ssid = "your SSID"; // The name of your wifi network, For demo purpose, put our smartphone in "Tethering Mode" and use the name of that Access Point.
const char* password = "yourPassWord";
const char* mqttServer = "broker.hivemq.com"; // This is a free online MQTT Broker, no authentication needed. 
const int mqttPort = 1883; // Standard MQTT portnumber
const char* mqttUser = ""; // Leave as it is if you use hivemq.com
const char* mqttPassWord = ""; // Leave as it is if you use hivemq.com
char message[16];

WiFiClient espClient;
PubSubClient client(espClient);

//RGB
int redPin = D6;
int greenPin = D7;
int bluePin = D8;
int LEDcolor=0;

// SCL GPIO5, or D1 used for Oled
// SDA GPIO4, or D2 used for Oled
#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!"); // Don't change anything if you use the
#endif                                                      // Wemos 0.66 inch OLED Shield for the D1 Mini and the right library
                                                            // https://github.com/mcauser/Adafruit_SSD1306/tree/esp8266-64x48

#define pwmPin D5
//int preheatSec = 120;
int preheatSec = 120;
int prevVal = LOW;
unsigned long th, tl, h, l, co2 = 0;

void PWM_ISR() {
  long tt = millis();
  int val = digitalRead(pwmPin);
  
  if (val == HIGH) {    
    if (val != prevVal) {
      h = tt;
      tl = h - l;
      prevVal = val;
    }
  }  else {    
    if (val != prevVal) {
      l = tt;
      th = l - h;
      prevVal = val;
      co2 = 5000 * (th - 2) / (th + tl - 4);      
    }
  }
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Create a random client ID
   String clientId = "ESP8266Client-";
   clientId += String(random(0xffff), HEX);
   if (client.connect(clientId.c_str())) {
     Serial.println("connected");
     Serial.println (clientId.c_str());
   } else {
     Serial.print("failed, rc=");
     Serial.print(client.state());
     Serial.println(" try again in 5 seconds");
     delay(5000);
   }
 }
}

void displayPreheating(int secLeft) {
  display.setTextSize(1);    
  display.println(F("PREHEATING"));
  display.setTextSize(1);    
  display.println();    
  display.setTextSize(2 );          
  display.print(" ");
  display.print(secLeft);
  display.display();
}

void displayPPM(long co2) {
  display.setTextSize(1);    
  display.println(F("CO2  PPM"));;    
  display.setTextSize(1);    
  display.println();    
  display.setTextSize(2);    
  if (co2 < 1000) {
    display.print(" ");
  }
  display.print(co2);  
  display.display();
  Serial.println(co2);  
}

void pubMessage() {
      char message[16];    
      snprintf(message, sizeof(message), "%d", co2);
      client.publish("yourname/sensors/co2", message); // This is an example, You can use any Topic and subtopic
      delay(20000);
}

void callback(char* topic, byte* co2, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
  Serial.print((char)co2[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
}


void rgbLed() {
  if ((co2 > 350) && (co2 < 450)) {
      analogWrite(redPin, 0);analogWrite(greenPin, 100);analogWrite(bluePin, 0);Serial.println("Fresh air");
  }
  if ((co2 > 450) && (co2 < 700)) {
    analogWrite(redPin,34);analogWrite(greenPin,139);analogWrite(bluePin, 34);Serial.println("Normal level");
    }
  if ((co2 > 700) && (co2 <= 1000)) {
  analogWrite(redPin, 50);analogWrite(greenPin, 205);analogWrite(bluePin, 50);Serial.println("Permitted level");
  }
  if ((co2 > 1000) && (co2 < 2500)) {
    analogWrite(redPin, 255);analogWrite(greenPin,69);analogWrite(bluePin, 0);Serial.println("Getting sleepy");
  }
  if ((co2 > 2500) && (co2 < 5000)) {
    analogWrite(redPin, 255);analogWrite(greenPin,0);analogWrite(bluePin, 0);Serial.println("Not good for your health");
  }
}


void setup()   { 
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status()!= WL_CONNECTED) {
  delay(500);
  Serial.print("Connecting to WiFi...");  
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  if (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassWord )) {
    Serial.println("connected");
    } else {
    Serial.print("failed with state ");
    Serial.print(client.state());   
    delay(2000);
    }
  }
 
  pinMode(pwmPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pwmPin), PWM_ISR, CHANGE);  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.setTextColor(WHITE);
}

void loop() { 
  if (!client.connected()) {
   reconnect();
   }   
    display.clearDisplay(); 
    display.setCursor(0,0); 
    if (preheatSec >= 0) {
      displayPreheating(preheatSec);
      preheatSec--;
     }
    else {    
      displayPPM(co2);
      pubMessage();
      rgbLed();             
    }
}