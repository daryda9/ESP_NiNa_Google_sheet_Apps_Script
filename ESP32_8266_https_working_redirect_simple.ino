#include "wifi_manage.h"
#include <ArduinoJson.h>

StaticJsonDocument<200> response;

float temperatura = 30;
float umidita = 90;

void setup() {
  Serial.begin(115200);
  delay(1000);
  setup_wifi(); // Inizializzazione e connessione al wifi 
  delay(1000);
}

void loop() {

  String data = "?temperatura=";
  data += temperatura;
  data += "&umidita=";
  data += umidita;

  String risposta = invia_messaggio(data);    // in risposta, viene salvato il risultato della richiesta

  Serial.println("RESPONSE LOOP: " + risposta);

  DeserializationError error = deserializeJson(response, risposta);
  if (error) {
      Serial.print("deserializeJson() fallita: ");
      Serial.println(error.f_str());
  } else {
      const char* status = response["status"];
      Serial.print("status: ");
      Serial.println(status);

      float maxTemperatura = response["maxTemperatura"];
      Serial.print("maxTemperatura: ");
      Serial.println(maxTemperatura);
  }


  delay(10000);
}
