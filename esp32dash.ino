/*
 Name:				esp32dash.ino
 Created:			6/21/2018
 Target hardware:	ESP32
 Author:			0megaIT
*/

#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <FS.h>

#define BUILTIN_LED_PIN 2
#define WIFI_TRY_COUNT 30

int wifiIP[5];
int wifiNetMask[5];
int wifiGateway[5];

char wifiSSID[33];
char wifiPASS[65];

unsigned short deviceCount;
bool deviceStarted;

// the setup function runs once when you press reset or power the board
void setup() 
{
	deviceStarted = false;

	deviceCount = 0;

	Serial.begin(115200);
	Serial.println("\n\n\n---> ESP starting begin<---");

	if (!FSInit())
		return;

	if (!WiFiInit())
		return;

	deviceStarted = true;
}

// the loop function runs over and over again until power down or reset
void loop() 
{
  
}

//----------------------------------------------------------- WI-FI INIT -----------------------------------------
bool WiFiInit()
{
	strcpy(wifiSSID, "OMEGA");				//<---Set your Wi-Fi network SSID
	strcpy(wifiPASS, "essy123essy");		//<---Set your Wi-Fi network PASS

	Serial.println("Enabling Wi-Fi...");

	WiFi.onEvent(WiFiEvent);
	
	IPAddress local_IP(192, 168, 1, 117);	//<---Set any unoccupied IP 
	IPAddress gateway(192, 168, 1, 1);		//<---Set your network gateway
	IPAddress subnet(255, 255, 255, 0);		//<---Set your network subnet mask

	IPAddress dns(8, 8, 8, 8); //Google Public DNS

	Serial.println("Network settings: ");
	
	local_IP = IPAddress(local_IP[0], local_IP[1], local_IP[2], local_IP[3]);
	Serial.print("IP: ");
	Serial.println(local_IP);
	
	gateway = IPAddress(gateway[0], gateway[1], gateway[2], gateway[3]);
	Serial.print("Gateway: ");
	Serial.println(gateway);

	subnet = IPAddress(subnet[0], subnet[1], subnet[2], subnet[3]);
	Serial.print("Subnet: ");
	Serial.println(subnet);

	WiFi.config(local_IP, gateway, subnet, dns);

	WiFi.mode(WIFI_STA);

	Serial.printf("Connecting to %s\n", wifiSSID);

	WiFi.begin(wifiSSID, wifiPASS);

	for (int i = 0; i < WIFI_TRY_COUNT; i++)
	{
		Serial.print(".");
		delay(500);
		if (WiFi.status() == WL_CONNECTED)
		{
			Serial.println("\nConnected!");
			break;
		}
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		Serial.println("---> Done: Wi-Fi init <---");
		BlinkWiFiClientStarted();
		return true;
	}
	else
	{
		Serial.println("Can't connect as a Wi-Fi client!");
		BlinkWiFiClientFailed();
		return false;
	}
	
}

void BlinkWiFiClientStarted()
{
	if (BUILTIN_LED_PIN >= 0)
	{
		pinMode(BUILTIN_LED_PIN, OUTPUT);

		for (int i = 0; i < 2; i++)
		{
			digitalWrite(BUILTIN_LED_PIN, HIGH);
			delay(500);
			digitalWrite(BUILTIN_LED_PIN, LOW);
			delay(500);
		}
	}
}

void BlinkWiFiClientFailed()
{
	if (BUILTIN_LED_PIN >= 0)
	{
		pinMode(BUILTIN_LED_PIN, OUTPUT);

		for (int i = 0; i < 5; i++)
		{
			digitalWrite(BUILTIN_LED_PIN, HIGH);
			delay(100);
			digitalWrite(BUILTIN_LED_PIN, LOW);
			delay(100);
		}
	}
}

void WiFiEvent(WiFiEvent_t event)
{
	if (event == SYSTEM_EVENT_STA_GOT_IP)
	{
		Serial.printf("\nGot IP: %s\r\n", WiFi.localIP().toString().c_str());
	}
	else if (event == SYSTEM_EVENT_STA_DISCONNECTED)
	{
		Serial.printf("\nDisconnected from Wi-Fi!\n");
	}
	else
	{
		Serial.printf("\nWi-Fi event! Code: %d\r\n", event);
	}
}
//----------------------------------------------------------- WI-FI INIT -----------------------------------------

//----------------------------------------------------------- HANDLE FILE SYSTEM ---------------------------------

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) 
{
	Serial.printf("Listing directory: %s\n", dirname);

	File root = fs.open(dirname);
	if (!root) 
	{
		Serial.println("Failed to open directory");
		return;
	}
	
	if (!root.isDirectory()) 
	{
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();

	while (file) 
	{
		if (file.isDirectory()) 
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels) 
			{
				listDir(fs, file.name(), levels - 1);
			}
		}
		else 
		{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

bool FSInit()
{
	if (SPIFFS.begin(false, "/spiffs", 100) == true)
	{
		Serial.println("---> Done: FileSystem inint <---");
		listDir(SPIFFS, "/", 0);
		return true;
	}
	else
	{
		Serial.println("---> Failed: FileSystem inint <---");
		return false;
	}
}

//----------------------------------------------------------- HANDLE FILE SYSTEM ---------------------------------
