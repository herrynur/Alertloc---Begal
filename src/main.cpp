#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include <GyverOLED.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

GyverOLED<SSH1106_128x64> oled;
TinyGPSPlus gps;

void updateSerial();
void displayInfo();
void setup_wifi();

// gps
double latitude = gps.location.lat();
double longitude = gps.location.lng();
String _long;
String _lat;

// #define button1 4
#define button2 39
#define button3 33
// #define button4 25

// wifi setup
const char *ssid = "Asus_X01BDA";
const char *password = "heri123456";
String payload;
#define ledwifi 26

// time
#define NTP_OFFSET 19800
#define NTP_INTERVAL 60 * 1000
#define NTP_ADDRESS "1.asia.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 28200, 60000);

// send data
void sendAlert(String status, String _long, String _lat);
String formattedTime;

void ujibutton();

void setup()
{
  // pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  // pinMode(button4, INPUT);
  oled.init();
  oled.clear();
  delay(100);
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(ledwifi, OUTPUT);
  delay(3000);
  setup_wifi();
  digitalWrite(ledwifi, HIGH);
  delay(2000);
  timeClient.begin();
}

void loop()
{
  while (Serial2.available() > 0)
  {
    if (gps.encode(Serial2.read()))
      displayInfo();
  }
  // Serial.print("long: ");
  // Serial.print(_long);
  // Serial.print(" - lat: ");
  // Serial.println(_lat);

  // button action
  // time
  // displayInfo();
  timeClient.update();
  formattedTime = timeClient.getFormattedTime();

  if (gps.location.isValid())
  {
    oled.setScale(1);
    oled.setCursorXY(20, 10);
    oled.print("GPS Signal OK  ");
    oled.setScale(2);
    oled.setCursorXY(20, 30);
    oled.print(formattedTime);
    oled.update();
    // button2
    if (digitalRead(button2) == HIGH)
    {
      sendAlert("Tinggi", _long, _lat);
      Serial.println("button 2 pressed");
      delay(100);
    }

    // button3
    if (digitalRead(button3) == HIGH)
    {
      sendAlert("Sedang", _long, _lat);
      Serial.println("button 3 pressed");
      delay(100);
    }
  }
  else
  {
    oled.setScale(1);
    oled.setCursorXY(20, 10);
    oled.print("Waiting for GPS");
    oled.update();
    oled.setScale(2);
    oled.setCursorXY(20, 30);
    oled.print(formattedTime);
    oled.update();

    // button2
    if (digitalRead(button2) == HIGH)
    {
      sendAlert("Tinggi", "112.656227", "-7.153452");
      Serial.println("button 2 pressed");
      delay(100);
    }

    // button3
    if (digitalRead(button3) == HIGH)
    {
      sendAlert("Sedang", "112.656227", "-7.153452");
      Serial.println("button 3 pressed");
      delay(100);
    }
  }
}

void ujibutton()
{
  if (digitalRead(button2) == HIGH)
  {
    delay(50);
    if (digitalRead(button2) == HIGH)
    {
      delay(50);
      if (digitalRead(button2) == LOW)
      {
        // sendAlert("Tinggi", "112.656227", "-7.153452");
        Serial.println("button 2 pressed");
      }
    }
  }
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    oled.update();
    delay(2);
    oled.clear();
    oled.setScale(1);
    oled.setCursorXY(0, 10);
    oled.print("Connecting to WiFi..");
    oled.update();
  }
  oled.clear();
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendAlert(String status, String _long, String _lat)
{
  oled.setScale(1);
  oled.setCursorXY(30, 55);
  oled.print("Sending...");
  oled.update();
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String serverB = "http://heri.d3k4interior.com/alertloc.php";
    String serverPath = serverB + "?pesan=" + status + "&long=" + _long + "&lat=" + _lat;

    http.begin(serverPath.c_str());
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    oled.setCursorXY(30, 55);
    oled.print("           ");
    oled.update();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    Serial2.write(Serial.read());
  }
  while (Serial2.available())
  {
    Serial.write(Serial2.read());
  }
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print(F("Location: "));
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.println(gps.location.lng(), 6);
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    _long = String(longitude, 6);
    _lat = String(latitude, 6);
  }
  else
  {
    Serial.println(F("Location: Waiting"));
  }
}