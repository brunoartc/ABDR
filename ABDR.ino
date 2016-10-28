/*
 * Copyright (c) 2016, brunoartc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "hidden", *password = "hidden", *usr = "hidden", *passwd = "hidden", *ausr = "hidden", *apasswd = "hidden";
int long prevmillis = 0;
bool stat=0;

ESP8266WebServer server ( 300 );

const int luz = 5;

void sluz() {
  if (stat) {digitalWrite ( luz , 1 );}
  else {digitalWrite ( luz , 0 );}
  Serial.println("LUZ");
}


void rfs(const char* xas, const char* xmsg) { 
  
  if (!server.authenticate(usr, passwd) && !server.authenticate(ausr, apasswd)){
      return server.requestAuthentication();
  }
  
  char msg[400];
        snprintf ( msg, 400,
  "<html>\
    <head>\
      <meta http-equiv='rfs' content='2; %d'>\
      <title>ABDR</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <p>%d %d s</p>\
    </body>\
  </html>",
      xas, xmsg, (prevmillis-millis())/1000
    );
    server.send ( 200,"text/html", msg);  
}

void handleRoot() {
  
  if (!server.authenticate(usr, passwd) && !server.authenticate(ausr, apasswd)){
      return server.requestAuthentication();
  }
  
  Serial.println("Client Connect");
	//digitalWrite ( luz, 1 );
	char temp[600];
	int sec = millis() / 1000, min = sec / 60, hr = min / 60;
  if (!server.authenticate(ausr, apasswd)) {
  	snprintf ( temp, 600,
  
  "<html>\
    <head>\
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
    
  } else {
    snprintf ( temp, 600,
  
    "<html>\
    <head>\
      <title>ABDR</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      </style>\
    </head>\
    <body>\
      <h1>v0.02a</h1>\
      <p>Uptime: %02d:%02d:%02d</p>\
      <p>LightLv: %02d</p>\
      <p>D2 <a href=\"aLUZ\"><button>LUZ</button></a>\
    </body>\
  </html>",
  
      hr, min % 60, sec % 60, analogRead(A0)
      );
  }
	server.send ( 200, "text/html", temp );
	Serial.println("Servidor Enviado e autenticado");
}

void handleNotFound() {
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
	pinMode ( luz, OUTPUT );
  int Llv=analogRead(A0);
	digitalWrite ( luz, 0 );
  delay(2);
  if (Llv-analogRead(A0)>0){
    digitalWrite ( luz, 1 );
    Llv=1;
  } else {
    Llv=0;
  }
	Serial.begin ( 115200 );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
  
    digitalWrite ( luz, 1 );
  	delay ( 250 );
  	Serial.print ( "." );
    digitalWrite ( luz, 0 );
    delay ( 250 );
     
	}
 
  digitalWrite ( luz, Llv );

	Serial.println ( "" );
	Serial.print ( "Conectado a: " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( MDNS.begin ( "ABDR" ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on ( "/", handleRoot );

  
	server.on ( "/test.svg", drawGraph );

 
	server.on ( "/aLUZ", []() {
    //server custom authentication
    if( !server.authenticate(ausr, apasswd) ){
      return server.requestAuthentication();
    }
  
    if (millis()>prevmillis){
      
      if (!stat){
        stat=1; Serial.println("aLuz Ligada"); handleRoot();
      }
      else{
        stat=0; Serial.println("aLuz DesLigada"); handleRoot();
      }
      prevmillis=millis()+60000;
    } else{
    }
    
	} );

  
  server.on ( "/ON", []() {
    stat=0;
    rfs("/?", "Voce sera redirecionado");
  } );

  
  server.on ( "/OFF", []() {
    stat=1;
    rfs("/?", "Voce sera redirecionado");
  } );

  
  server.on ( "/LUZ", []() {   
    if (!stat){
        stat=1; Serial.println("aLuz Ligada");
      }
      else{
        stat=0; Serial.println("aLuz DesLigada");
      };
      
      rfs("/?", "Voce sera redirecionado");
      
  } );

  
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );
}



void loop ( void ) {
	server.handleClient();
  sluz();
}


void drawGraph() {
	String out = "";
	char temp[100];
	out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
 	out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
 	out += "<g stroke=\"black\">\n";
 	int y = rand() % 130;
 	for (int x = 10; x < 390; x+= 10) {
 		int y2 = rand() % 130;
 		sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
 		out += temp;
 		y = y2;
 	}
	out += "</g>\n</svg>\n";

	server.send ( 200, "image/svg+xml", out);
}

