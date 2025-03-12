#include "common.h"

#include <SPI.h>
#include <WiFiNINA.h>
WiFiSSLClient client;
#include <ArduinoJson.h>

StaticJsonDocument<200> doc;


char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverbase[40] = "script.google.com";  
char server[40];

int stato = 0;
int status = WL_IDLE_STATUS;
String path = "";


void setup_wifi(){

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!\nSTOPPED");    
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);    
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.println("Connected to wifi");

}

boolean FIRST = true;
void go(int st) {
    stato = st;
    FIRST = true;
    delay(300);
}



void runcmd(String data) {
    if (FIRST) {
      Serial.println("Avvio...");
      FIRST = false;
    }

    path = Web_App_URL + data;
    strncpy(server, serverbase ,40);
    Serial.print("server: "); Serial.println(server);
    Serial.print("path: "); Serial.println(path);
    go(1);
}


void call() {
    if (FIRST) {
        Serial.println("Call HTTPS...");
        FIRST = false;
    }
    if (client.connect(server, 443)) {
        Serial.print("Connected to server: "); Serial.println(server);
        client.print("GET " + path + " HTTP/1.1\r\n");
        client.print("Host: " + String(server) + "\r\n");
        client.print("Connection: close\r\n\r\n");
        go(2);
    } else {
        Serial.println("Connection failed");
        go(10);
    }
}


int getRequestCode(String line) {
  int n = 0;
  char ll[100] = ""; 
    line.toCharArray(ll, line.length());
    int i = 0;
    char* token = strtok(ll, " ");
    while (token != NULL) {
        i++;        
        token = strtok(NULL, " ");
        if (i == 1) {  //qui ho il codice della risposta
          n = atoi(token);
        }        
    }  
    return n;
}

int requestCode;


void getHeader() {
  if (FIRST) {
    Serial.println("Analisi Header...");    
    FIRST = false;
  }

  if (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);     
    
    if (line.indexOf("HTTP/1.1") >= 0) {   
      requestCode = getRequestCode(line);
      
    } else if ((requestCode == 302) && (line.indexOf("Location:") >= 0)) {
      int pos = line.indexOf(" ");
      String newaddr = line.substring(pos+1);
      Serial.println("--------");
      Serial.println(newaddr);
      int s = line.indexOf("//")+2;
      int f = line.indexOf("/", s) + 1;
      String str = line.substring(s, f);
      str.toCharArray(server, str.length());
      Serial.print("server="); Serial.println(server);      
      path = line.substring(f-1);
      Serial.print("path="); Serial.println(path);      
      
    } else if (line.indexOf("Connection: close") >= 0){
      Serial.println("decide che fare...");
      Serial.println("code: " + String(requestCode));

      if ((requestCode == 302) || (requestCode == 307)) {
        //redirect  
        client.flush();
        go(1);
      } else if (requestCode == 200){

        //leggo i dati  
        go(3);
      } else {
        //non gestito
        client.flush();
      }
    }
  }
}

int n_linea;
String readdata() {
    if (FIRST) {
        Serial.println("Lettura dati");
        FIRST = false;
        n_linea=0;
    }

    if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.print("[!] LINEA" + String(n_linea) + ": ");
        Serial.println(line);
        if (n_linea == 3){
          DeserializationError error = deserializeJson(doc, line);
          if (error) {
              Serial.print("deserializeJson() failed: ");
              Serial.println(error.f_str());
              go(10);
          } else {
              const char* status = doc["status"];
              Serial.print("status: ");
              Serial.println(status);
              go(0);
          }
          return line;
        }
        n_linea++;
    }
    return "";
}

void error() {
    if (FIRST) {
        Serial.println("Error - stopped");
        FIRST = false;
    }
}


/*
void loop_switchcase() {
    switch (stato) {
        case 0:
            waitcmd();
            break;
        case 1:
            call();
            break;
        case 2:
            //leggo il codice della risposta e decido come procedere (200, 302, 404, 500...)
            getHeader();
            break;
        case 3:
            readdata();
            break;
        case 10:
            error();
            break;
    }

}
*/

String invia_messaggio(String data){ 

    String response="-1";

    runcmd(data);

    while (stato != 0){
      switch (stato) {

        case 1:
            call();
            break;
        case 2:
            //leggo il codice della risposta e decido come procedere (200, 302, 404, 500...)
            getHeader();
            break;
        case 3:
            response = readdata();
            break;
        case 10:
            error();
            break;
      }
    }
    return response;

}
