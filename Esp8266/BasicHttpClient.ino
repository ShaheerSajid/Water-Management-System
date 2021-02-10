#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;


int y;
String x = "";
String uid = "";
bool session = true;
uint64_t t1 = 0;
unsigned long t2 = 0;
uint64_t t4 = 0;
float t3 = 0;
HTTPClient http;


String wow(String l)
{
  String payload = "";
  http.begin("http://192.168.137.248/jillani/");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(l);
  payload = http.getString();
  http.end();
  return payload;
}
void setup() {

  Serial.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Shaheer", "12345678");
  while (!(WiFiMulti.run() == WL_CONNECTED))
  {
    Serial.print('N');
    delay(500);
  }
  Serial.print('C');
  delay(500);

}

void loop() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    if (Serial.available())
    {
      x = "";
      y = Serial.read();

      if (y == '?')
      {
        while (Serial.available())
        {
          y = Serial.read();
          if(y == '\n') break;
          x += String(y, HEX);
          t3 = 0;
        }
        uid = x;
        String r = "returnQuantity=" + x + "&key=7yjgDSkpgk07";
        //Serial.println(r);
        String t = wow(r);
        delay(1000);
        Serial.print(t);
        session = false;
        t4 = millis();
      }
      else if (y == 'F')
      {
        while (Serial.available())
        {
          y = Serial.read();
          x += String(char(y));
        }
        if (x == "Start")
        {
          t1 = millis();
          session = true;
        }
        if (x == "Stop") {
          x = "";
          while (!Serial.available())
            {
            String z = "";
            }
            while (Serial.available())
            {
            y = Serial.read();
            x += String(char(y));
            }
            //Serial.print(x); 

          t4 = millis();
          session = false;
        }
      }
    }
    if (millis() - t4 > 15000  && session == false)
    {
      Serial.print('O');
      session = true;
      String t = wow("updateQuantity="+uid+"&usage="+x+"&key=7yjgDSkpgk07");
     // Serial.println(t);
    }
  }
  else
  {
    while (!(WiFiMulti.run() == WL_CONNECTED))
    {
      Serial.print('N');
      delay(500);
    }
    Serial.print('C');
    delay(500);
  }
}

