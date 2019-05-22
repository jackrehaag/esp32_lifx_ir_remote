#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <IRremote.h>
#include <string>
#include "secrets.h"

int RECV_PIN = 15;
int BUZZ_PIN = 14;
IRrecv irrecv(RECV_PIN);

int freq = 2000;
int channel = 0;
int resolution = 8;
bool BEEP = false;

decode_results results;

void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  };

  Serial.println("Connected to the WiFi network");
}

void setup() {
  Serial.begin(9600);
  delay(4000);
  connectToWifi();
  irrecv.enableIRIn();
  if (BEEP == true) {
    ledcSetup(channel, freq, resolution);
    ledcAttachPin(BUZZ_PIN, channel);
  }
}

void beep() {
  if (BEEP == true) {
    ledcWrite(channel, 10);
    ledcWriteTone(channel, 1000);
    delay(200);
    ledcWriteTone(channel, 0);
  }
}

void setBulbState(char* values) {
  HTTPClient http;
  const char* endpoint = stringTocharStar("https://api.lifx.com/v1/lights/" + LIFX_BULB_ID + "/state");
  http.begin(endpoint);
  http.addHeader("Authorization", "Bearer " + LIFX_TOKEN);
  Serial.println("Setting bulb state:" + String(values));
  Serial.println("status code: " + http.PUT(values));
  Serial.println("response: " + http.getString());
}

String getBulbStatus() {
  HTTPClient http;
  const String endpoint = String("https://api.lifx.com/v1/lights/") + LIFX_BULB_ID;
  http.begin(endpoint);
  http.addHeader("Authorization", "Bearer " + LIFX_TOKEN);
  Serial.println("Getting bulb information...");
  Serial.println("status code: " + http.GET());
  String response = http.getString();
  Serial.println("response: " + response);
  http.end();
  return response;
}

double getBrightnessLevel() {
  String bulbStatus = getBulbStatus();
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, bulbStatus);
  Serial.println(String("bulb info: " + bulbStatus));
  double brightnessLevel = doc[0]["brightness"];
  Serial.println(String("Current brightness level: ") + brightnessLevel);
  return brightnessLevel;
}

char* stringTocharStar(String str) {
  if (str.length() != 0) {
    char *p = const_cast<char*>(str.c_str());
    return p;
  }
}

void increaseBrightness() {
  double brightness = getBrightnessLevel();
  double newBrightnessLevel = brightness + 0.20;
  if (newBrightnessLevel > 1) {
    newBrightnessLevel = 1.0;
  }
  String newBulbState = String("{\"brightness\": ") + newBrightnessLevel + String("}");
  Serial.println("new bulb state: " + newBulbState);
  setBulbState(stringTocharStar(newBulbState));
}

void decreaseBrightness() {
  double brightness = getBrightnessLevel();
  double newBrightnessLevel = brightness - 0.20;
  if (newBrightnessLevel < 0) {
    newBrightnessLevel = 0;
  }
  String newBulbState = String("{\"brightness\": ") + newBrightnessLevel + String("}");
  Serial.println("new bulb state: " + newBulbState);
  setBulbState(stringTocharStar(newBulbState));
}

void turnEffectsOff() {
  HTTPClient http;
  const String endpoint = String("https://api.lifx.com/v1/lights/" + LIFX_BULB_ID + "/effects/off");
  http.begin(endpoint);
  http.addHeader("Authorization", "Bearer " + LIFX_TOKEN);
  http.POST("{\"power_off\": false}");
  Serial.println("Turning off bulb effects...");
  String response = http.getString();
  Serial.println("response: " + response);
  http.end();
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    switch (results.value) {
      case 16187647:
        beep();
        Serial.println("brighter");
        increaseBrightness();
        break;
      case 16220287:
        beep();
        Serial.println("dimmer");
        decreaseBrightness();
        break;
      case 16203967:
        beep();
        Serial.println("off");
        setBulbState("{\"power\":\"off\"}");
        break;
      case 16236607:
        beep();
        Serial.println("on");
        setBulbState("{\"power\":\"on\"}");
        break;
      case 16195807:
        beep();
        Serial.println("Red (R)");
        setBulbState("{\"power\":\"on\", \"color\": \"red\"}");
        break;
      case 16228447:
        beep();
        Serial.println("Green (G)");
        setBulbState("{\"power\":\"on\", \"color\": \"green\"}");
        break;
      case 16212127:
        beep();
        Serial.println("Dark blue (B)");
        setBulbState("{\"power\":\"on\", \"color\": \"blue\"}");
        break;
      case 16244767:
        beep();
        Serial.println("White (W)");
        setBulbState("{\"power\":\"on\", \"color\": \"white\"}");
        break;
      case 16191727:
        beep();
        Serial.println("Light red");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:0.0 saturation:0.8399999737739563 brightness:1.0\"}");
        break;
      case 16224367:
        beep();
        Serial.println("Light green");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:120.0 saturation:0.5099999904632568 brightness:1.0\"}");
        break;
      case 16208047:
        beep();
        Serial.println("Medium blue");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:234.85000610351562 saturation:0.8500000238418579 brightness:1.0\"}");
        break;
      case 16240687:
        beep();
        Serial.println("Flash");
        break;
      case 16199887:
        beep();
        Serial.println("Orange");
        setBulbState("{\"power\":\"on\", \"color\": \"orange\"}");
        break;
      case 16232527:
        beep();
        Serial.println("Light blue");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:191.5399932861328 saturation:0.7099999785423279 brightness:1.0\"}");
        break;
      case 16216207:
        beep();
        Serial.println("Purple");
        setBulbState("{\"power\":\"on\", \"color\": \"purple\"}");
        break;
      case 16248847:
        beep();
        Serial.println("Strobe");
        turnEffectsOff();
        break;
      case 16189687:
        beep();
        Serial.println("Light orange");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:35.58000183105469 saturation:0.7400000095367432 brightness:1.0\"}");
        break;
      case 16222327:
        beep();
        Serial.println("Turquoise");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:202.47000122070312 saturation:1.0 brightness:1.0\"}");
        break;
      case 16206007:
        beep();
        Serial.println("Light purple");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:279.8500061035156 saturation:0.6800000071525574 brightness:1.0\"}");
        break;
      case 16238647:
        beep();
        Serial.println("Fade");
        break;
      case 16197847:
        beep();
        Serial.println("Yellow");
        setBulbState("{\"power\":\"on\", \"color\": \"yellow\"}");
        break;
      case 16230487:
        beep();
        Serial.println("Dark turquoise");
        setBulbState("{\"power\":\"on\", \"color\": \"hue:213.8800048828125 saturation:1.0 brightness:1.0\"}");
        break;
      case 16214167:
        beep();
        Serial.println("Pink");
        setBulbState("{\"power\":\"on\", \"color\": \"pink\"}");
        break;
      case 16246807:
        beep();
        Serial.println("Smooth");
        break;
    }
    irrecv.resume();
  }
}
