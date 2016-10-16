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


const char* ssid = "hidden";
const char* password = "hidden";
const char* usr = "hidden";
const char* passwd = "hidden";
bool stat=0;

ESP8266WebServer server ( 300 );

const int luz = 5;

void handleRoot() {
  Serial.println("Client Connect");
  while(!server.authenticate(usr, passwd)){
    return server.requestAuthentication();
    delay(1);
    server.send ( 200, "text/plain", " Senha ou usuario incorreto >.< " );
  }
	//digitalWrite ( luz, 1 );
	char temp[600];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 600,

"<html>\
  <head>\
    <title>Quarto</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>v0.01</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>GPIO2 <a href=\"ON\"><button>ON</button></a> <a href=\"OFF\"><button>OFF</button></a></p>\
    <p>GPIO3 <a href=\"LUZ\"><button>LUZ</button></a> <a href=\"OFF\"><button>OFF</button></a></p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

		hr, min % 60, sec % 60
	);
	server.send ( 200, "text/html", temp );
	Serial.println("Server Sent, Sucesfull auth");
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
	digitalWrite ( luz, 0 );
	Serial.begin ( 115200 );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		digitalWrite(luz,1);
		delay ( 500 );
		Serial.print ( "." );
    digitalWrite( luz,0);
	}

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( MDNS.begin ( "ABDR" ) ) {
		Serial.println ( "MDNS responder started" );
	}

	server.on ( "/", handleRoot );

  
	server.on ( "/test.svg", drawGraph );

 
	server.on ( "/test", []() {
		server.send ( 200, "text/plain", "this works as well" );
	} );

  
  server.on ( "/ON", []() {
    handleRoot();
    stat=0;
  } );

  
  server.on ( "/OFF", []() {
    handleRoot();
    stat=1;
  } );

  
  server.on ( "/LUZ", []() {
    while(!server.authenticate(usr, passwd)){
      return server.requestAuthentication();
      delay(1);
      server.send ( 200, "text/plain", " Senha ou usuario incorreto >.< " );
    }
  
    handleRoot();
    
    if (!stat){stat=1;Serial.println("Luz Ligada");}
    else {stat=0;Serial.println("Luz DesLigada");}
  } );

  
	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );
}

void loop ( void ) {
	server.handleClient();
  if (stat) {digitalWrite ( luz , 1 );}
  else {digitalWrite ( luz , 0 );}
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

