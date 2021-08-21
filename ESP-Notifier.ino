/**
 * @author Sv443 / Sven Fehler
 * @license MIT
 * 
 * Use this sketch with the desktop client: https://github.com/Sv443/Node-Notifier
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#include "config.h"

// Builtin LED feedback
#define LED_FEEDBACK_ENABLED true

// Pin of button (active high)
#define BUTTON_PIN 14

// BAUD rate (bytes/s) of the serial monitor (CTRL + SHIFT + M) in Arduino IDE
#define SERIAL_BAUD_RATE 9600


struct HttpResult {
  int status;
  String response;
};


void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.println();

	pinMode(BUILTIN_LED, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);

	digitalWrite(BUILTIN_LED, HIGH);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	Serial.print("Connecting...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
}

/** Whether a request is currently processing */
bool processing = false;

void loop()
{
  int pressed = digitalRead(BUTTON_PIN);

  Serial.print("Pressed: ");
  Serial.println(pressed);

	if(pressed == HIGH && !processing)
	{
		processing = true;

		// TODO: change to POST, send to local IP
		HttpResult result = sendGET("http://192.168.178.45:8076/joke/Programming?format=txt&lang=en");

		if(result.status >= 200 && result.status < 300)
		{
			Serial.println(result.response);

			processing = false;
			okLed();
		}
		else
		{
			Serial.print("Error while sending request - status: ");
			Serial.println(result.status);
			processing = false; 
			errLed();
		}
	}

  delay(100);
}

HttpResult sendGET(String url)
{
	WiFiClient client;
	HTTPClient http;

	HttpResult result;

	Serial.println("Sending HTTP request to " + url);

	if(http.begin(client, url))
	{
		int status = http.GET();

		if(status >= 200 && status < 300)
		{
			String payload = http.getString();
			
			http.end();

			result.status = status;
			result.response = payload;
		}
		else
		{
			result.status = status;
			result.response = "";
		}
	}
	else
	{
		result.status = 0;
		result.response = "";
	}

	return result;
}

void okLed()
{
	if(LED_FEEDBACK_ENABLED)
	{
		digitalWrite(BUILTIN_LED, LOW);
		delay(100);
		digitalWrite(BUILTIN_LED, HIGH);
		delay(100);
		digitalWrite(BUILTIN_LED, LOW);
		delay(100);
		digitalWrite(BUILTIN_LED, HIGH);
		delay(100);
		digitalWrite(BUILTIN_LED, LOW);
		delay(100);
		digitalWrite(BUILTIN_LED, HIGH);
	}
}

void errLed()
{
	if(LED_FEEDBACK_ENABLED)
	{
		digitalWrite(BUILTIN_LED, LOW);
		delay(1000);
		digitalWrite(BUILTIN_LED, HIGH);
		delay(1000);
		digitalWrite(BUILTIN_LED, LOW);
		delay(1000);
		digitalWrite(BUILTIN_LED, HIGH);
	}
}
