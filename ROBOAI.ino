#include <ArduinoJson.h>
 #include <SPI.h>
#include <Wire.h>
#ifdef ESP32
  #include <WiFi.h>
#else
#include <ESP8266HTTPClient.h>

  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
// Replace with your network credentials
const char* ssid = "RD";
const char* password = "adam5557";

// Initialize Telegram BOT
#define BOTtoken "6804671478:AAEpnLKG0rycSRlnXduK6RDx7u_8PUNmE28"  

#define CHAT_ID "6951537336"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


void setup() {
  Serial.begin(115200);
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");   
    client.setTrustAnchors(&cert);
  #endif

  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());
}
void hasilAI(String data, String chat_id){
  Serial.println(data);
  bot.sendMessage(chat_id, data);
}

String urlEncode(const String &value) {
  String encodedValue = "";

  for (char c : value) {
    if (isAlphaNumeric(c) || c == '-' || c == '.' || c == '_' || c == '~') {
      encodedValue += c;
    } else if (c == ' ') {
      encodedValue += '+';
    } else {
      encodedValue += '%' + String(c, HEX);
    }
  }

  return encodedValue;
}

void respond(String data, String chat_id){
  
    if (WiFi.status() == WL_CONNECTED){
      String encoded = urlEncode(data);
      String apiUrl = "http://149.28.136.43:3000/openai?q=" + encoded;
       WiFiClient wifiClient;
    HTTPClient http;
    http.setTimeout(20000);  
    http.begin(wifiClient, apiUrl); 
    http.addHeader("Content-Type", "application/json");
    
    int httpCode = http.GET();
            delay(5000);
    if (httpCode == HTTP_CODE_OK) {  
  DynamicJsonDocument jsonDocument(1024);
      DeserializationError error = deserializeJson(jsonDocument, http.getString());

      if (error) {
        Serial.println(error.c_str());
      } else {
        const char* responseData = jsonDocument["data"];
        String responseDataS = String(responseData); 
        hasilAI(responseDataS, chat_id);
      }
    }else{
      Serial.println(httpCode);
    }
    }
  }
  
void responseAI(String data, String chat_id){
 Serial.println(data); 
    respond(data, chat_id);  
  }


void process(){
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);

    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Tidak terdeteksi User di JSON", "");
    }
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
String welcome = "Selamat datang di NMCU, " + from_name + ".\n";
      welcome += "Gunakan  /ai untuk berinteraksi dengan robot.\n\n";
      bot.sendMessage(chat_id, welcome, "");
    }
if (text) {
   String content = text;
          bot.sendMessage(chat_id, "Processing!");
          responseAI(content, chat_id);  
          delay(5000);
}else{
    bot.sendMessage(chat_id, "MASUKKAN FORMAT DENGAN BENAR!");
}
}
}
  }


void loop() {
  process();
}
