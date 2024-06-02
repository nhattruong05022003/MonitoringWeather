#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include <FS.h> // File System
#include <SPIFFS.h> // SPI Flash System 
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


// JSON configuration file
#define JSON_CONFIG_FILE "/telegram_config.json"

// Wifi
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Telegram bot
char BOTtoken[100] = "BOT_TOKEN"; // your Bot Token (Get from Botfather)
char CHAT_ID[50] = "CHAT_ID";

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 3000;
unsigned long lastTimeBotRan;

// DHT11
DHT dht(19, DHT11);
float humidity = 0;
float temperature = 0;

float real_humidity = 0;
float real_temperature = 0;

// Weather API
String url = "https://api.openweathermap.org/data/2.5/weather?";
char apiKey[50] = "OPENWEATHER_API_KEY";

char lat[20] = "YOUR_LATITUDE";
char lon[20] = "YOUR_LONGTITUDE";

// WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
WiFiManager wm;
int timeout = 120; // seconds to run for
bool connected = false;
bool shouldSaveConfig = false;

String mesg = "";
HTTPClient http;

int t1 = 0;
// Save config in json format
void saveConfigFile(){
  Serial.println("Saving configuation...");
  // Create json document
  StaticJsonDocument<512> json;
  json["BOTtoken"] = BOTtoken;
  json["CHAT_ID"] = CHAT_ID;
  json["apiKey"] = apiKey;
  json["lat"] = lat;
  json["lon"] = lon;

  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if(!configFile){
    // Error file did not open
    Serial.println("Failed to open config file for writing !");
  }

  // Serialize JSON data to write to file
  serializeJsonPretty(json, Serial);
  if(serializeJson(json, configFile) == 0){
    // Error writing file
    Serial.println("Failed to write to file !");
  }

  configFile.close();
}

// Load existing configuration file
bool loadConfigFile(){
  // Uncommet if we need to format filesystem
  // SPIFFS.format();

  // Read
  Serial.println("Mounting File System ... ");

  // Make need to make it begin(true) first time you are using SPIFFS
  if(SPIFFS.begin(false) || SPIFFS.begin(true)){
    Serial.println("mounted file system");
    if(SPIFFS.exists(JSON_CONFIG_FILE)){
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
      if(configFile){
        Serial.println("Opened configuration file");
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);
        serializeJsonPretty(json, Serial);
        if(!error){
          Serial.println("Parsing JSON");

          strcpy(BOTtoken, json["BOTtoken"]);
          strcpy(CHAT_ID, json["CHAT_ID"]);
          strcpy(apiKey, json["apiKey"]);
          strcpy(lat, json["lat"]);
          strcpy(lon, json["lon"]);

          return true;
        }
        else{
          Serial.println("Failed to load json config");
        }
      }
    }
  }
  else{
    // Error mounting file system
    Serial.println("Failed to mount FS");
  }
  return false;
}

void saveConfigCallback(){
  // Callback notifying us of the need to save configuration
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager){
  // Called when config mode launched
  Serial.println("Entered Configuration Mode");
  Serial.print("Config SSID : ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.print("Config IP Address : ");
  Serial.println(WiFi.softAPIP());
}



void handleNewMessages(int numNewMessages) {
  // Serial.println("handleNewMessages");
  // Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/sensor to request data from sensor \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/sensor") {
      mesg = "";
      
      http.begin(url + "lat=" + lat +"&lon=" + lon + "&appid=" + apiKey);

      int httpCode = http.GET();
      if(httpCode > 0){
        String JSON_Data = http.getString();
        // Serial.println(JSON_Data);

        DynamicJsonDocument doc(2048);
        deserializeJson(doc, JSON_Data);
        JsonObject obj = doc.as<JsonObject>();

        real_temperature = obj["main"]["temp"].as<float>() - 273.15;
        real_humidity = obj["main"]["humidity"].as<float>();

        Serial.print("Real temperature : ");
        Serial.println(real_temperature);
        Serial.print("Real humidity : ");
        Serial.println(real_humidity);

        mesg += "Nhiet do ngoai troi : ";
        mesg += String(real_temperature) + "°C\n";
        mesg += "Do am ngoai troi : ";
        mesg += String(real_humidity) + "%\n";
      }
      else{
        Serial.println("Error!");
      }
      http.end();
      
      humidity = dht.readHumidity();
      temperature = dht.readTemperature();

      Serial.print("Temperature : ");
      Serial.println(temperature);
      Serial.print("Humidity : ");
      Serial.println(humidity);
      mesg += "Nhiet do trong phong : ";
      mesg += String(temperature) + "°C\n";
      mesg += "Do am trong phong : ";
      mesg += String(humidity) + "%\n";
      bot.sendMessage(chat_id, mesg, "");
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  // Supress Debug information
  wm.setDebugOutput(false); // ko in cac dong debug
  // Remove any previous network settings
  // wm.resetSettings();

  // Doc file json
  bool forceConfig = false; // change to true when testing to force configuration every time we run
  bool spiffsSetup = loadConfigFile();
  if(!spiffsSetup){
    Serial.println("Forcing config mode as there is no saved config !");
    forceConfig = true;
  }
  // Set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);
  // Set callback that gets called when connecting to previous wifi fails and enter access mode
  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalTimeout(timeout);

  // Define a text box, 50 characters maximum
  WiFiManagerParameter text_box_bottoken("BOTToken", "Enter Telegram Bot Token here", BOTtoken, 100);
  WiFiManagerParameter text_box_chatid("ChatID", "Enter your Telegram chat id here", CHAT_ID, 50);
  WiFiManagerParameter text_box_api("APIKey", "Enter your API Key here", apiKey, 50);
  WiFiManagerParameter text_box_lat("Latitude", "Enter your latitude here", lat, 20);
  WiFiManagerParameter text_box_lon("Longtitude", "Enter your longtitude here", lon, 20);

  // Add custom parameter
  wm.addParameter(&text_box_bottoken);
  wm.addParameter(&text_box_chatid);
  wm.addParameter(&text_box_api);
  wm.addParameter(&text_box_lat);
  wm.addParameter(&text_box_lon);

  // DHT11
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  dht.begin();
  delay(2000);
  Serial.print("DHT11 connected !");
  Serial.println();

  // // Turn off watchdog reset
  // TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE; // Unlock timer config.
  // TIMERG1.wdt_feed = 1; // Reset feed count.
  // TIMERG1.wdt_config0.en = 0; // Disable timer.
  // TIMERG1.wdt_wprotect = 0; // Lock timer config.

  // TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
  // TIMERG0.wdt_feed = 1;
  // TIMERG0.wdt_config0.en = 0;
  // TIMERG0.wdt_wprotect = 0;

  // Serial.println("Turn off Watchdog reset !");

  
  // Set up Wifi and Telegram Bot
  if(!wm.autoConnect("WifiManager")){
    Serial.println("Failed to connect and hit timeout");
    delay(1000);
    ESP.restart();
    delay(5000);
  }

  Serial.print("BOTtoken : ");
  Serial.println(text_box_bottoken.getValue());
  strncpy(BOTtoken, text_box_bottoken.getValue(), sizeof(BOTtoken));
  Serial.print("CHAT_ID : ");
  Serial.println(text_box_chatid.getValue());
  strncpy(CHAT_ID, text_box_chatid.getValue(), sizeof(CHAT_ID));
  Serial.print("apiKey : ");
  Serial.println(text_box_api.getValue());
  strncpy(apiKey, text_box_api.getValue(), sizeof(apiKey));
  Serial.print("Latitude : ");
  Serial.println(text_box_lat.getValue());
  strncpy(lat, text_box_lat.getValue(), sizeof(lat));
  Serial.print("Longtitude : ");
  Serial.println(text_box_lon.getValue());
  strncpy(lon, text_box_lon.getValue(), sizeof(lon));

  Serial.println("Connected!");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  connected = true;

  if(shouldSaveConfig){
    saveConfigFile();
  }
  // Dummy run
  http.begin(url + "lat=" + lat +"&lon=" + lon + "&appid=" + apiKey);
  int httpCode = http.GET();
  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() > lastTimeBotRan + botRequestDelay) && connected)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    // Serial.println(numNewMessages);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  if(millis() - t1 > 150000 && t1 != 0){
    ESP.restart();
    t1 = millis();
  }
  else if(t1 == 0){
    t1 = millis();
  }
}
