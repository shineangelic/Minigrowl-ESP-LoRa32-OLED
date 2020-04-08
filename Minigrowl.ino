// 
/*
Created:	03/07/2018 20:07:15
Author:     CRONER\Ale
*/


#include <ArduinoJson.hpp>
#include <ArduinoJson.h> 
 
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplay.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include "SSD1306.h"
#include <WiFi.h>
#include <GrowlManager.h> 

/* Sketch to demonstrate basic SI odel functionality

Connections
===========
Connect 13 to TX of SI UART
Connect 15 to RX of SI UART

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16
*/

// WIFI_LoRa_32 ports
// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)
#define MAIN_LIGHTS 12//onboardLED
#define HEATER		25
#define OUTTAKE_FAN	13
#define INTAKE_FAN	2
#define OLED		16


#define LIGHT_SENSOR 33
#define DHTPIN 22 

//grow room service coordinator
GrowlManager gm = GrowlManager();

SSD1306  display(0x3c, 4, 15);
//WIFI
const char* ssid = "Cisco66778";
const char* password = "cimadaconegliano";


//const char* host = "192.168.0.54";
short pc;
 

/**************************************************************************/
/*
Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
	Serial.begin(57600);


	pinMode(MAIN_LIGHTS, OUTPUT);
	gm.initMainLights(MAIN_LIGHTS);

	pinMode(INTAKE_FAN, OUTPUT);
	gm.initIntakeFan(INTAKE_FAN);

	pinMode(OUTTAKE_FAN, OUTPUT);
	gm.initOuttakeFanPin(OUTTAKE_FAN);

	pinMode(HEATER, OUTPUT);
	gm.setHeaterPin(HEATER);

	gm.initLightSensor(LIGHT_SENSOR);
	pinMode(LIGHT_SENSOR, INPUT);

	gm.setDhtPin(DHTPIN);
	pinMode(DHTPIN, INPUT);

	gm.initChamber();
	delay(1000);//wait DHT

	//OLED
	pinMode(OLED, OUTPUT);
	digitalWrite(OLED, LOW);    // set GPIO16 low to reset OLED
	delay(50);
	digitalWrite(OLED, HIGH); // while OLED is running, must set GPIO16 in high
	display.init();
	display.setLogBuffer(5, 30);
	 

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	pc = 0;//program counter
}


void loop(void)
{
	gm.loop();
	delay(500);
	/*OLED Report*/
	
	Serial.print("Free Heap: ");
	Serial.println(ESP.getFreeHeap());
	// Display the results (total active energy in Wh)
	/*if (event.current) {
		//print DB debug
		Serial.println(siClient.debugDbContents().c_str());
		Serial.print("Free Heap: ");
		Serial.println(ESP.getFreeHeap());
	}*/

	//delay(100);

	//Serial.print(gm.reportStatus().c_str());
	drawText();

	//Serial.print("connecting to ");
	//Serial.println(host);

	// Use WiFiClient class to create TCP connections
	//doTestGet();

}
/*
void doTestGet() {
	WiFiClient client;
	const int httpPort = 8080;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return;
	}

	// We now create a URI for the request
	String url = "/command/download/";


	Serial.print("Requesting URL: ");
	Serial.println(url);

	// This will send the request to the server
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");
	unsigned long timeout = millis();
	while (client.available() == 0) {
		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			client.stop();
			return;
		}
	}

	// Read all the lines of the reply from server and print them to Serial
	while (client.available()) {
		String line = client.readStringUntil('\r');
		Serial.println(line);
		ARDUINOJSON_NAMESPACE::StaticJsonDocument<400> doc;
		ARDUINOJSON_NAMESPACE::DeserializationError err;
		err = ARDUINOJSON_NAMESPACE::deserializeJson(doc, line);
		if (err) {
			//Serial.print(F("deserializeJson() failed: "));
			//Serial.println(err.c_str());
			continue;
		}
		else {
			// Fetch values.
			//
			// Most of the time, you can rely on the implicit casts.
			// In other case, you can do doc["time"].as<long>();
			const char* sensor = doc["parameter"];
			Serial.print("SENSOR FROM JSON:");
			Serial.println(sensor);
		}
		break;
	}
}*/

void drawText() {
	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, gm.reportStatus().c_str());

	//drawPowerProgressBar();
	display.display();
	display.clear();
}