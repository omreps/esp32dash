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
#include <driver/adc.h>

#define BUILTIN_LED_PIN 2
#define WIFI_TRY_COUNT 30
#define MAX_DASH_ITEMS_COUNT 18

#define WIFI 0
#define MILLIS 1
#define HALL 3
//<----- define your types here

WebServer server(80);

int wifiIP[5];
int wifiNetMask[5];
int wifiGateway[5];

char wifiSSID[33];
char wifiPASS[65];

typedef struct
{
	char name[20];
	unsigned short iconUnicode;
	unsigned short type;
	double value;
} DashItems;

DashItems dashItem[MAX_DASH_ITEMS_COUNT];
unsigned short dashItemsCount;

bool esp32Started;

// the setup function runs once when you press reset or power the board
void setup() 
{
	esp32Started = false;

	Serial.begin(115200);
	Serial.println("\n\n\n---> ESP starting begin<---");

	if (!WiFiInit())
		return;

	if (!FSInit())
		return;

	if (!ServerInit())
		return;

	if (!DashItemsInit())
		return;

	esp32Started = true;

	Serial.println("\n\n\n---> Done: ESP32 Started <---");
}

// the loop function runs over and over again until power down or reset
void loop() 
{

	//Update data

	//WiFi signal level
	dashItem[0].value = getWifiSignalLevel();
	
	//Millis
	dashItem[1].value = millis();

	//Built in Hall sensor
	dashItem[2].value = hallRead();

	//handle server requests
	server.handleClient();

	delay(1);
}


//----------------------------------------------------------- WI-FI INIT -----------------------------------------
bool WiFiInit()
{
	strcpy(wifiSSID, "OMEGA");				//<-----Set your Wi-Fi network SSID
	strcpy(wifiPASS, "essy123essy");		//<-----Set your Wi-Fi network PASS

	Serial.println("Enabling Wi-Fi...");

	WiFi.onEvent(WiFiEvent);
	
	IPAddress local_IP(192, 168, 1, 117);	//<-----Set any unoccupied IP 
	IPAddress gateway(192, 168, 1, 1);		//<-----Set your network gateway
	IPAddress subnet(255, 255, 255, 0);		//<-----Set your network subnet mask

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

int getWifiSignalLevel()
{
	int dBm, quality;
	dBm = WiFi.RSSI();
	if (dBm <= -100)
		quality = 0;
	else if (dBm >= -50)
		quality = 100;
	else
		quality = 2 * (dBm + 100);
	return quality;
}
//----------------------------------------------------------- WI-FI INIT -----------------------------------------

//----------------------------------------------------------- HANDLE FILE SYSTEM ---------------------------------
bool FSInit()
{
	if (SPIFFS.begin(false, "/spiffs", 100) == true)
	{
		Serial.println("---> Done: FileSystem inint <---");
		return true;
	}
	else
	{
		Serial.println("---> Failed: FileSystem inint <---");
		return false;
	}
}
//----------------------------------------------------------- HANDLE FILE SYSTEM ---------------------------------

//----------------------------------------------------------- HANDLE HTTP SERVER REQ -----------------------------
String getContentType(String filename) 
{
	if (server.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path) 
{
	Serial.println("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.htm";
	String contentType = getContentType(path);
	if (SPIFFS.exists(path)) 
	{
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

bool ServerInit()
{
	//SERVER INIT HTTP
	server.onNotFound([]() 
	{
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});
	server.on("/", HTTP_GET, [](){if (!handleFileRead("/"))server.send(404, "text/plain", "FileNotFound");});  //open home page
	server.begin();

	Serial.println("---> Done: HTTP server started <---");

	return true;
}
//----------------------------------------------------------- HANDLE HTTP SERVER REQ -----------------------------

//----------------------------------------------------------- HANDLE dashItems --------------------------------------
bool DashItemsInit()
{
	dashItemsCount = 0;

	//WiFi signal level
	dashItem[dashItemsCount].type = WIFI;
	strcpy(dashItem[dashItemsCount].name, "Wi-Fi");
	dashItem[dashItemsCount].iconUnicode, 0xf1eb;
	dashItem[dashItemsCount].value = getWifiSignalLevel();
	dashItemsCount++;

	//Millis
	dashItem[dashItemsCount].type = MILLIS;
	strcpy(dashItem[dashItemsCount].name, "Millis");
	dashItem[dashItemsCount].iconUnicode, 0xf017;
	dashItem[dashItemsCount].value = millis();
	dashItemsCount++;

	//Built in Hall sensor
	dashItem[dashItemsCount].type = HALL;
	strcpy(dashItem[dashItemsCount].name, "Hall");
	dashItem[dashItemsCount].iconUnicode, 0xf076;
	adc1_config_width(ADC_WIDTH_12Bit);
	dashItem[dashItemsCount].value = hallRead();
	dashItemsCount++;

	//.....
	//<----- Add your dashboard items (sensors, variables, output state and so on) initialization here
	//.....

	Serial.println("---> Done: Dash Items init <---");

	return true;
}
//----------------------------------------------------------- HANDLE dashItems --------------------------------------
