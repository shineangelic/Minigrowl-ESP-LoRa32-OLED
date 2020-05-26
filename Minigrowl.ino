/*

Minigrowl main sketch. It will create a GrowlManager object and cycle thru its loop.
The Growlmanager has a GrowlChamber, which contains the abstracion of real devices (actuators)
and sensors. The GrowlManager comunicates via JSON with the spring-boot server, its address has to be configured in this sketch
as well as devices Pin connections

Created:	03/07/2018 20:07:15
Author:     CRONER\Ale
*/
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplay.h>
#include "SSD1306.h"
#include <WiFi.h>
#include "time.h"
#include <GrowlManager.h>


#include <Private.h>
//UNCOMMENT and use yours
//const char* host = "https://yourhost";
//const int httpPort = 443;

//const char* ssid = "yourSSID";
//const char* password = "yourWifiPass";


/* 

Connections
===========
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
#define MAIN_LIGHTS 12
#define HEATER		2//onboardLED
#define OUTTAKE_FAN	13
#define INTAKE_FAN	25
#define OLED		16
//BME280 temperature sensor
#define SCL 17
#define SDA 21

#define LIGHT_SENSOR 33
#define DHTPIN 22 
#define ERRPIN 23//error LED

#define BOARD_ID 1 

//grow room service coordinator
GrowlManager gm = GrowlManager(BOARD_ID, 1);

//oled display, can be easily removed
SSD1306  display(0x3c, 4, 15);

const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

/**************************************************************************/
/*
Initialize the growchamber and setup devices PINs
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
	
	gm.setBME280Pin(SCL, SDA);

	pinMode(ERRPIN, OUTPUT);

	//call chamber delegate AFTER having set PINs
	gm.initChamber(host, httpPort, proto);
	delay(1000);//wait DHT

	//OLED
	pinMode(OLED, OUTPUT);
	digitalWrite(OLED, LOW);// set GPIO16 low to reset OLED
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

	// Init and get the time
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	printLocalTime();
}

void printLocalTime() {
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		Serial.println("Failed to obtain time");
		return;
	}
	time_t tnow;
	time(&tnow);
	//int ret = localtime_s(&when, &tnow);
	struct tm* when = localtime(&tnow);
	Serial.println(when, " %B %d %Y %H:%M:%S (%A)");
}


void loop(void)
{
	gm.loop();

	//Serial.print("Free Heap: ");
	//Serial.println(ESP.getFreeHeap());

	/*OLED Report*/
	drawText();
	delay(500);

	//Report error physically
	if (gm.hasChamberError())
		digitalWrite(ERRPIN, HIGH);
	else
		digitalWrite(ERRPIN, LOW);

}

void drawText() {
	display.clear();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, gm.reportStatus().c_str());

	display.display();
	display.clear();
}