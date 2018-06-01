/*
   Copyright (c) 2016, brunoartc
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/




#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUDP.h>
#include <WiFiManager.h>
#include <DNSServer.h>

Adafruit_SSD1306 display(3);
WiFiManager wifiManager;

const char *ssid = "", *password = "", *usr = "admin", *passwd = "admin";
int long prevmillis = 0;
bool stat = 0, bt = 0;
String logg, senha="";
int logn = 0;
int x = 0;

ESP8266WebServer server ( 80 );





const int luz = 5, btp = 4;

void loga(String x) {
  logn++;
  logg += "\n";
  logg += logn;
  int sec = millis() / 1000, minn = sec / 60, hr = minn / 60;
  logg += "(";
  logg += hr;
  logg += ":";
  logg += minn % 60;
  logg += ":";
  logg += sec % 60;
  logg += "): ";
  if (x != "botao") logg += server.client().remoteIP().toString();
  logg += "(";
  logg += x;
  logg += ")";
}

void ba() {
  if (digitalRead(btp) == 1) {
    if (!bt) {
      if (!stat) {
        stat = 1;
        loga("botao");
      }      else {
        stat = 0;
        loga("botao");
      };
    }
    bt = 1;
    delay( 5 );
  } else {
    bt = 0;
    delay( 5 );
  }
}

void sluz() {
  if (stat) {
    digitalWrite ( luz , 1 );
  }
  else {
    digitalWrite ( luz , 0 );
  }
  //Serial.println("LUZ");
}


void rfs() {
  loga("rfs");
  if (!server.authenticate(usr, passwd)) {
    return server.requestAuthentication();
  }

  char msg[400];
  snprintf ( msg, 400,
             "<html>\
    <head>\
      <meta http-equiv='refresh' content='2; /?'>\
      <title>ABDR</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <p>Acao conclida, voce sera redirecionado | Espere %d s</p>\
      <iframe src='http://free.timeanddate.com/clock/i5gd6hxo/n233' frameborder='0' width='116' height='18'></iframe>\
    </body>\
  </html>",
             (prevmillis - millis()) / 1000
           );
  server.send ( 200, "text/html", msg);
}



void handleRoot() {
  loga("handleRoot");
  if (!server.authenticate(usr, passwd)) {
    return server.requestAuthentication();
  }

Serial.println("Client Connect");
  //digitalWrite ( luz, 1 );
  char temp[600];
  int sec = millis() / 1000, min = sec / 60, hr = min / 60;
  if (server.authenticate(usr, passwd)) {
    snprintf ( temp, 600,
  
  "<html>\
    <head>\
      <meta http-equiv='refresh' content='5'/>\
      <title>ABDR</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>v0.02</h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <p>Light: %02d</p>\
      <p>D2 <a href=\"ON\"><button>ON</button></a> <a href=\"OFF\"><button>OFF</button></a></p>\
      <p>D2 <a href=\"LUZ\"><button>LUZ</button></a></p>\
      <img src=\"/test.svg\" />\
    </body>\
  </html>",
  
      hr, min % 60, sec % 60, analogRead(A0)
    );
  }
  server.send ( 200, "text/html", temp );
  Serial.println("Servidor Enviado e autenticado");
}

void handleNotFound() {
  loga("handleNotFound");
  digitalWrite ( luz, 1 );
  String message = "((>_<)) 404 File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  //digitalWrite ( luz, 0 );
}

void setup ( void ) {
  Serial.begin ( 115200 );

  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  Wire.begin(12,13);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.setCursor(0,0);
  
  //display.drawBitmap(32, 64,  ory, 64, 128, '0');

  
  while (x < 5) {

    senha += char(random(97, 122));
    x++;
  }
  display.setTextSize(5);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(":)");
  display.display();

  delay(200);

  display.setTextSize(2);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("github.com/");
  display.println("brunoartc");
  display.display();
  delay(1000);

  display.setTextSize(1);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Configuracao inicial:");
  display.println("Conecte ao quartoo");
  display.print("Senha= ");
  display.println(senha);
  display.println("Entre em 192.168.4.1");
  display.println("no seu computador");
  display.println("");
  display.print("          brunoartc");
  display.display();
  
  //wifiManager.resetSettings();
  const char* senhab = senha.c_str();
  WiFiManager wifiManager;
  wifiManager.autoConnect("quartoo",senhab);
  pinMode ( luz, OUTPUT );
  pinMode ( btp, INPUT );



  Serial.println ( "" );
  Serial.print ( "Conectado a: " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( MDNS.begin ( "abdr" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );


  server.on ( "/ON", []() {
    if (!server.authenticate(usr, passwd)) {
      return server.requestAuthentication();
    }
    rfs();
    stat = 0;
  } );


  server.on ( "/OFF", []() {
    if (!server.authenticate(usr, passwd)) {
      return server.requestAuthentication();
    }
    rfs();
    stat = 1;
  } );


  server.on ( "/LUZ", []() {
    loga("LUZ!!!!!!!!!!!!!!!!") ;
    if (!server.authenticate(usr, passwd)) {
      return server.requestAuthentication();
    }
    rfs();
    if (!stat) {
      stat = 1; Serial.println("aLuz Ligada");
    }
    else {
      stat = 0; Serial.println("aLuz DesLigada");
    };


  } );

  server.on ( "/logg", []() {
    if (!server.authenticate(usr, passwd)) {
      return server.requestAuthentication();
    }
    loga("logg '-'");
    server.send ( 200, "text/plain", logg );
  } );

  server.on ( "/test", []() {
    loga("test");
    server.send ( 404, "text/plain", "Confirmado" );
  } );


  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void info() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("CONECTADO A " + WiFi.SSID());
  display.println("Para controle acesse");
  display.println("abdr.local./ ou");
  display.println(WiFi.localIP());
  display.println("");
  display.println("");
  display.print("             ");
  display.println(WiFi.RSSI());
  display.display();
}



void loop ( void ) {
  if (Serial.available() > 0) {
    // read the incoming byte:
    char PCK = Serial.read();
    if (PCK == 'l') Serial.println(logg);
  }

  info();

  ba();

  server.handleClient();

  sluz();

}
