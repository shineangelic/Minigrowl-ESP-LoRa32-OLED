// 
/*
Created:	03/07/2018 20:07:15
Author:     CRONER\Ale
*/

#include <ArduinoJson.hpp>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT_U.h>
#include <DHT.h>
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplay.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
//#include <GrowlManager.h>
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
#define MAIN_LIGHTS 25//onboardLED
#define HEATER  12
#define FAN  13
#define OLED  16
#define FAN2  2
#define LIGHT_SENSOR 33
#define DHTPIN 22
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

GrowlManager gm = GrowlManager();

SSD1306  display(0x3c, 4, 15);
//WIFI
const char* ssid = "Cisco66778";
const char* password = "cimadaconegliano";

const char* host = "192.168.0.54";
const char* streamId = "....................";
const char* privateKey = "....................";

//LIGHTSENSOR
float Rsensor; //Resistance of sensor in K

/**************************************************************************/
/*
Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
	pinMode(MAIN_LIGHTS, OUTPUT);
	pinMode(FAN2, OUTPUT);
	pinMode(FAN, OUTPUT);
	pinMode(HEATER, OUTPUT);
	pinMode(LIGHT_SENSOR, INPUT);
	pinMode(OLED, OUTPUT);
	digitalWrite(OLED, LOW);    // set GPIO16 low to reset OLED
	delay(50);
	digitalWrite(OLED, HIGH); // while OLED is running, must set GPIO16 in high
	dht.begin();

	display.init();
	display.setLogBuffer(5, 30);
	Serial.begin(57600);

	displaySensorDetails();

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

/**************************************************************************/
/*
Displays some basic information on this sensor from the unified
sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{

	Serial.println("------------------------------------");

	Serial.println("------------------------------------");
	Serial.println("");
	delay(500);
}



void loop(void)
{

	digitalWrite(HEATER, LOW);    // set GPIO16 low to reset OLED
	digitalWrite(FAN2, HIGH);
	digitalWrite(FAN, HIGH);
	digitalWrite(MAIN_LIGHTS, HIGH);
	delay(100);

	digitalWrite(FAN, LOW);
	digitalWrite(HEATER, HIGH);
	delay(100);
	digitalWrite(FAN2, LOW);

	int sensorValue = analogRead(LIGHT_SENSOR);
	Rsensor = (float)(1023 - sensorValue) * 10 / sensorValue;
	Serial.println("the analog read data is ");
	Serial.println(sensorValue);
	Serial.println("the sensor resistance is ");
	Serial.println(Rsensor, DEC);

	delay(100);
	digitalWrite(FAN2, LOW);

	/*OLED Report*/
	drawText();

	// Display the results (total active energy in Wh)
	/*if (event.current) {
		//print DB debug
		Serial.println(siClient.debugDbContents().c_str());
		Serial.print("Free Heap: ");
		Serial.println(ESP.getFreeHeap());
	}*/

	//delay(100);
	delay(2000);

	float h = dht.readHumidity();
	float t = dht.readTemperature();
	float f = dht.readTemperature(true);

	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println(F("Failed to read from DHT sensor!"));
		return;
	}

	float hif = dht.computeHeatIndex(f, h);
	float hic = dht.computeHeatIndex(t, h, false);

	Serial.print(F("Humidity: "));
	Serial.print(h);
	Serial.print(F("%  Temperature: "));
	Serial.print(t);
	Serial.print(F("°C "));
	Serial.print(f);
	Serial.print(F("°F  Heat index: "));
	Serial.print(hic);
	Serial.print(F("°C "));
	Serial.print(hif);
	Serial.println(F("°F"));



	Serial.print("connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP connections
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
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(err.c_str());
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

}

void drawText() {
	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, "ssss");

	//drawPowerProgressBar();
	display.display();
	display.clear();
}

void drawPowerProgressBar() {
	float progress = 22 * 100;
	// draw the progress bar
	display.drawProgressBar(0, 52, 80, 8, progress);

	// draw the percentage as String
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.drawString(102, 50, String(23) + "W");
}