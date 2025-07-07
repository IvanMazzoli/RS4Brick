#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

// Setup RS485
#define RS485_RX_PIN D6
#define RS485_TX_PIN D5
#define RS485_CONTROL_PIN D7
SoftwareSerial rs485(RS485_RX_PIN, RS485_TX_PIN);

// RS485 methods
void rs485Send(const String &message);
void rs485Receive();

// UUID methods
String uuid;
String generateUUID();

void setup()
{

    // Enable serial debug
    Serial.begin(115200);

    // Setup RS485
    rs485.begin(9600);
    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, LOW);

    // Generate a UUID
    uuid = generateUUID();
    Serial.println("[SLAVE] RS4Brick Slave initialized");
    Serial.print("[SLAVE] UUID: ");
    Serial.println(uuid);
}

void loop()
{
    // Keep receiving data from the RS485 bus
    rs485Receive();
}

/*
 * Sends data via RS485
 */
void rs485Send(const String &message)
{
    // Enable RS485 transmission
    digitalWrite(RS485_CONTROL_PIN, HIGH);
    delay(2);

    // Transmit all the data tho the bus
    rs485.println(message);
    rs485.flush();

    // Go back to listening
    delay(2);
    digitalWrite(RS485_CONTROL_PIN, LOW);

    Serial.print("[SLAVE] Sent response: ");
    Serial.println(message);
}

/*
 * Receives data via RS485
 *
 * AVAILABLE COMMANDS:
 * WHO          Replies with the slave UUID
 */
void rs485Receive()
{

    // While there's data on the RS485 bus
    while (rs485.available())
    {

        // Read all incoming data until newline char
        String message = rs485.readStringUntil('\n');
        message.trim();

        // Check first if there's actually something
        if (message.length() > 0)
        {

            // Print the command to the serial monitor (debug)
            Serial.print("[SLAVE] Received command: ");
            Serial.println(message);

            // Discovery coomad > Reply with UUID 
            if (message.equalsIgnoreCase("WHO"))
            {
                String response = "UUID:" + uuid;
                rs485Send(response);
            }
        }
    }
}

/*
 * Generate a UUID based on the chip ID
 */
String generateUUID()
{
    uint32_t chipId = ESP.getChipId();
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "rs4b-%06X", chipId);
    return String(buffer);
}