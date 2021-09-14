/**
 * @author Sv443 / Sven Fehler
 * @license MIT
 * 
 * ESP-Notifier
 * WiFi connected switch based on ESP8266 - for using with the desktop client: https://github.com/Sv443/Node-Notifier
 * 
 * For installation instructions, please visit this page: https://github.com/Sv443/ESP-Notifier#installation
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ArduinoJson.h>

#include "config.h"



/*
     ____       _   _   _
    / ___|  ___| |_| |_(_)_ __   __ _ ___   _
    \___ \ / _ \ __| __| | '_ \ / _` / __| (_)
     ___) |  __/ |_| |_| | | | | (_| \__ \  _
    |____/ \___|\__|\__|_|_| |_|\__, |___/ (_)
                                |___/
*/

// Full URL to the server running Node-Notifier
// Expected format: "http://IP_ADDRESS:PORT" (without trailing slash)
#define SERVER_URL "http://192.168.178.45:8042"

// Builtin LED feedback
#define LED_FEEDBACK_ENABLED true

// Pin of button (active high)
#define BUTTON_PIN 14

// BAUD rate (bytes/s) of the serial monitor (CTRL + SHIFT + M) in Arduino IDE
#define SERIAL_BAUD_RATE 9600

// Notification settings
// If you need finer control, modify the getPostData() function to your liking
#define NOTIFICATION_TITLE "TITLE"
#define NOTIFICATION_MESSAGE "MESSAGE"
#define NOTIFICATION_ICON "assets/test.png"

// If you need to send a lot of data, increase INTERNAL_JSON_MEM_SIZE below

/* End of settings */






/* !!!! Internal settings, I don't recommend touching these unless necessary or you know what you're doing */

// ArduinoJson allocated memory size - to calculate use https://arduinojson.org/v6/assistant/
#define INTERNAL_JSON_MEM_SIZE 1000

/* End of internal settings */





/**
 * @struct HttpResult The result of an HTTP request
 * @property status The HTTP status code
 * @property response The raw response text as a string
 */
struct HttpResult {
  int status;
  String response;
};

/**
 * @struct JsonParseRes The result of parsing a JSON string
 * @property error Whether there was an error
 * @property message A message from the server
 * @property errorMsg An error message (if error=true)
 */
struct JsonParseRes {
  bool error;
  String message;
  String errorMsg;
};

/** Whether a request is currently processing (to debounce the button) */
bool processing = false;

/**
 * Gets called once at startup to initialize everything
 */
void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.println();

  Serial.println("ESP-Notifier v1.0.0 by Sv443 - https://github.com/Sv443/ESP-Notifier");


	pinMode(BUILTIN_LED, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);

	digitalWrite(BUILTIN_LED, HIGH); // ESP's builtin LED is active low for some reason

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
}

/**
 * Gets called on every clock cycle of the microcontroller
 */
void loop()
{
	int pressed = digitalRead(BUTTON_PIN);

	Serial.print("Pressed: ");
	Serial.println(pressed);

	if(pressed == HIGH && !processing)
	{
		processing = true;

    String url = SERVER_URL;
    url += "/send";

    String data = getPostData();

		HttpResult result = sendPOST(url, data);

		if(result.status >= 200 && result.status < 300)
		{
      JsonParseRes parsed = parseJson(result.response);

      if(parsed.error == true)
      {
        Serial.print("Error while parsing POST request - status: ");
        Serial.println(result.status);
        Serial.println(parsed.errorMsg);
        Serial.println();
        processing = false;
        errLed();
      }
      else
      {
        Serial.println("Successfully sent POST request to server");
  			processing = false;
  			okLed();
      }
		}
		else
		{
			Serial.print("Error while sending POST request - status: ");
			Serial.println(result.status);
			processing = false; 
			errLed();
		}
	}

  	delay(100);
}

/**
 * Returns the data that gets sent to the server  
 * Feel free to modify this to your liking
 */
String getPostData()
{
  // StaticJsonDocument allocates in stack, DynamicJsonDocument allocates in heap
  // TODO: find out which is better here
  DynamicJsonDocument doc(INTERNAL_JSON_MEM_SIZE);

  String result = "";

  doc["title"] = NOTIFICATION_TITLE;
  doc["message"] = NOTIFICATION_MESSAGE;
  doc["icon"] = NOTIFICATION_ICON;

  serializeJson(doc, result);

  return result;
}

JsonParseRes parseJson(String jsonString)
{
  unsigned int jsonLength = jsonString.length() + 1;

  DynamicJsonDocument doc(INTERNAL_JSON_MEM_SIZE);
  char json[jsonLength];
  jsonString.toCharArray(json, jsonLength);

  Serial.println();
  Serial.println("JSON");
  Serial.println(json);

  DeserializationError error = deserializeJson(doc, json);

  JsonParseRes result = { false, "", "" };

  if(error)
  {
    result.error = true;
    result.errorMsg = error.f_str();
  }
  else
  {
    bool err = doc["error"];
    String msg = doc["message"];

    result.error = err;
    result.message = msg;
  }

  return result;
}

/**
 * Sends a GET request
 */
HttpResult sendGET(String url)
{
	WiFiClient client;
	HTTPClient http;

	HttpResult result;

	Serial.println("Sending HTTP GET request to " + url);

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

/**
 * Sends a POST request
 */
HttpResult sendPOST(String url, String data)
{
  WiFiClient client;
  HTTPClient http;

  HttpResult result;

  Serial.println("Sending HTTP POST request to " + url);

  if(http.begin(client, url))
  {
    http.addHeader("Content-Type", "application/json; charset=UTF-8");
    int status = http.POST(data);

    if(status >= 200 && status < 300)
    {
      String payload = http.getString();

      http.end();

      result.status = status;
      result.response = "TODO";
      //result.response = parseJson(payload);
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

/**
 * Flashes the builtin LED to signify a successful request
 */
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

/**
 * Flashes the builtin LED to signify a failed request or error
 */
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
