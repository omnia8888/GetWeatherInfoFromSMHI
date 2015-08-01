/*
 Name:		GetWeatherInfoFromSMHI.ino
 Created:	8/1/2015 4:49:34 PM
 Author:	Micke
*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   50

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 99); // IP address, may need to change depending on network
//char server[] = "www.google.com";
char server[] = "ws.smhivader.se";

EthernetClient client;

char HTTP_req[REQ_BUF_SZ] = { 0 }; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer
String httpResponseLine;

enum SMHIWeatherImgNum
{
	Sunny = 1, //Soligt
	PartlySunnny = 2 //Lattmolnighet = 2,
	PartlyCloudy = 3 //VaxlandeMolnighet = 3,
	Clear = 4, //Halvklart 
	MostlyCloudy = 5, //Molnigt
	Overcast = 6, //Mulet
	Fog = 7, //Dimma
	ChanceOfRain = 8, //Regnskurar
	ScatteredThunder = 9, //�skskurar
	ScatteredSnowAndRainShowers = 10, //Sn�blandat regn och sol
	PartlySnow = 11, //v�xlande sn� och sol.
	Rain = 12, //Regn
	ThunderStorm = 13, //�ska
	SnowyRain = 14, //Sn�blandat regn
	Snow = 15 //Sn�
};

void setup() {
	Serial.begin(9600);       // for debugging
	
	// disable Ethernet chip and SD Card
	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	//Run with fixed IP
	Ethernet.begin(mac, ip);  // initialize Ethernet device with Fixed IP

	// print your local IP address:
	Serial.print("My IP address: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print(".");
	}
	Serial.println();

	// give the Ethernet shield a second to initialize:
	delay(1000);
	Serial.println("connecting...");

	// if you get a connection, report back via serial:
	if (client.connect(server, 80)) {
		Serial.println("connected");
		// Make a HTTP request:
		
		//client.println("GET /search?q=arduino HTTP/1.1");
		//client.println("Host: www.google.com");
		//client.println("Connection: close");
		
		client.println("GET /embed/7/Kragsta-2699487/ HTTP/1.1");
		client.println("Host: ws.smhivader.se");
		client.println("Connection: close");
		client.println();
	}
	else {
		// kf you didn't get a connection to the server:
		Serial.println("connection failed");
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
	// if there are incoming bytes available 
	// from the server, read them and print them:
	
	if (client.available()) {
		char c = client.read();
		
		//Check for new line char
		// if you get a newline, clear the line:
		if (c == '\n') {
			//Serial.println(httpResponseLine);
			parseHttpResponseLine();
			httpResponseLine = "";
		}
		else {
			httpResponseLine += char(c);
		}

	}

	// if the server's disconnected, stop the client:
	if (!client.connected()) {
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();

		// do nothing forevermore:
		while (true);
	}
}

void parseHttpResponseLine()
{
	//Look for 
	//int searchStringIndex = httpResponseLine.indexOf("html");
	int searchStringIndex = httpResponseLine.indexOf("top-weather");
	//Serial.print("Search index = ");
	//Serial.println(searchStringIndex);
	//Check if response is found
	if (searchStringIndex > 0) {
		Serial.println("Parseresponse found!");
		Serial.print("Search index = ");
		Serial.println(searchStringIndex);
		//Serial.print("Response line =");
		//Serial.print(httpResponseLine);
		//Serial.println(".");
		int startIndexOfWeatherInfoDesc = searchStringIndex + 13;
		int endIndexOfWeatherInfoDesc = httpResponseLine.indexOf("</span>");
		String weatherInfo = httpResponseLine.substring(startIndexOfWeatherInfoDesc,endIndexOfWeatherInfoDesc);


		Serial.print("Weather is ");
		Serial.println(weatherInfo);
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();
	}
}
