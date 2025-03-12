#include "common.h"

#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecure.h>
  WiFiClientSecure client;
#endif

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

void setup_wifi(){

  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    delay(1000);
    if (connecting_process_timed_out == 0) {
      delay(1000);
        ESP.restart();

    }
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("------------");
}

String invia_messaggio(String data){
    if (WiFi.status() == WL_CONNECTED) {

    // Creazione URL per invio dati a Google Sheets.
    String Send_Data_URL = Web_App_URL; //+ "?";

    Send_Data_URL += data;

    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);

    // Initialize HTTPClient as "http".
      HTTPClient http;
    
    #ifdef ESP32
      http.begin(Send_Data_URL.c_str()); //ESP32
    #elif defined(ESP8266)
      client.setInsecure();
      http.begin(client, Send_Data_URL); //esp8266
    #endif

    // HTTP GET Request.
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setTimeout(10000); // Imposta il timeout a 10 secondi
    // Gets the HTTP status code.
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    // Getting response from google sheets.
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);    
    }
    
    http.end();
    
    Serial.println("-------------");
    return payload;
  }
  return "WIFI_NOT_CONNECTED";
}