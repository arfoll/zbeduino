#include <EspMQTTClient.h>
#include <mcp_can.h>
#include <SPI.h>

#include "zbeduino_g11.h"

// Create a zbeduino_secrets.h file with your SSID & passwd
#include "zbeduino_secrets.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

EspMQTTClient client(
    SECRET_SSID,
    SECRET_PASS,
    "192.168.88.48",  // MQTT Broker server ip
    "",   // Can be omitted if not needed
    "",   // Can be omitted if not needed
    "zbeduino",      // Client name that uniquely identify your device
    1883
);

typedef struct {
    unsigned char value_pos; // position of value in 8 byte array
    unsigned char bitmask; // bitmask to detect value
    unsigned char emit_char; // character to print
    char pretty_name[8];
} zbe_key_configuration;

zbe_key_configuration zbe_keys[] =
{
    { BYTE_BUTTON_SET4, ZBE_MAP, 'm', "MAP"},
    { BYTE_BUTTON_SET3, 0x01, 'n', "NAV"},
    { BYTE_BUTTON_SET3, 0x08, 'e', "MEDIA"}
};

long unsigned int rxId;
unsigned long long rxInt;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgStringTemp[10];
char msgString[128];

// Pins used for a KS0411
#define CAN0_INT 0                              // Set INT to pin 8 on 'normal' UNO
MCP_CAN CAN0(15);                               // Set CS to pin 10 on 'normal' UNO

void setup()
{
    Serial.begin(115200);

    // Optional functionalities of EspMQTTClient
    //client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater(OTA_USER, OTA_PASS); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
    client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
    //client.enableLastWillMessage("zbeduino/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true

    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s, with
    // Filters for extended CAN IDs (MCP_STDEXT)
    if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
        Serial.println("MCP2515 Initialized Successfully!");
    else
        Serial.println("Error Initializing MCP2515...");

    // Configure INT pin
    pinMode(CAN0_INT, INPUT);

    // Init both mask & filters
    // I don't understand these - https://forum.arduino.cc/t/mcp2515-can-filtering/506401/8
    CAN0.init_Mask(0,0,0x0250);
    CAN0.init_Filt(0, 0, 0x025B);

    // Change to normal mode to allow transmission
    CAN0.setMode(MCP_NORMAL);

    Serial.println("ZBEduino finished init...");
}

void onConnectionEstablished() {
    client.publish("zbeduino/online", "Hello World!");
}

void loop()
{
    // If CAN0_INT pin is low, read receive buffer
    if (!digitalRead(CAN0_INT)) {
        // Read data: len = data length, buf = data byte(s)
        CAN0.readMsgBuf(&rxId, &len, rxBuf);

#if 0
        // Determine if ID is standard (11 bits) or extended (29 bits)
        if ((rxId & 0x80000000) == 0x80000000)
            sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
        else
            sprintf(msgString, "Standard ID: 0x%.3lX  DLC: %1d  Data:", rxId, len);

        Serial.print(msgString);
#endif

        // Determine if message is a data frame / 0x25B and has len of 8
        if (rxId == 0x25B && len == 8) {
            // null terminate string
            msgString[0] = 0;
            for (byte i = 0; i<len; i++){
                sprintf(msgStringTemp, " 0x%.2X", rxBuf[i]);
                strcat(msgString, msgStringTemp);
            }
            Serial.printf("MSG: %s\r\n", msgString);

#if 1
            for (int i = 0; i < (sizeof(zbe_keys) / sizeof(zbe_key_configuration)); i++) {
                if (rxBuf[zbe_keys[i].value_pos] & zbe_keys[i].bitmask) {
                    Serial.printf("%s detected, will emit %c\n", zbe_keys[i].pretty_name, zbe_keys[i].emit_char);
                    client.publish("zbeduino/emit", String(zbe_keys[i].emit_char));
                }
            }
#else

            if (rxBuf[BYTE_BUTTON_SET4] != 0xF8) {
                if (rxBuf[BYTE_BUTTON_SET4] & ZBE_MAP) {
                    client.publish("zbeduino/emit", "m");
                }
            }
            else if (rxBuf[BYTE_BUTTON_SET3] != 0xC0) {
                if (rxBuf[BYTE_BUTTON_SET3] & 0x01) {
                    client.publish("zbeduino/emit", "e");
                }
                if (rxBuf[BYTE_BUTTON_SET3] & 0x08) {
                    client.publish("zbeduino/emit", "n");
                }
            }
#endif


            // Lib handles error in case you do this before connection is alive
            client.publish("zbeduino/payload", msgString);
            //client.publish("zbeduino/emit", tempint);
        }
    }
    // Keep MQTT client happy
    client.loop();
}

