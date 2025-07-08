#include <Arduino.h>
#include <SoftwareSerial.h>

// Setup RS485
#define RS485_RX_PIN D6
#define RS485_TX_PIN D5
#define RS485_CONTROL_PIN D7
SoftwareSerial rs485(RS485_RX_PIN, RS485_TX_PIN);

// RS485 methods
void rs485Send(const String &message);
void rs485Receive();

void setup()
{

  // Enable serial debug
  Serial.begin(115200);

  // Setup RS485
  rs485.begin(9600);
  pinMode(RS485_CONTROL_PIN, OUTPUT);
  digitalWrite(RS485_CONTROL_PIN, LOW);

  // Setup done
  Serial.println("[MASTER] RS4Brick Master initialized");
}

void loop()
{

  // Start receiving data from slaves
  rs485Receive();

  // Transfer inputs from the console to RS485
  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0)
    {
      Serial.print("[MASTER] Sending RS485 message: ");
      Serial.println(input);
      rs485Send(input);
    }
  }
}

/*
 * Sends data via RS485
 *
 * AVAILABLE COMMANDS:
 * WHO      Starts slave discovery
 */
void rs485Send(const String &message)
{

  // Enable RS485 transmission
  digitalWrite(RS485_CONTROL_PIN, HIGH);
  delayMicroseconds(50);

  // Transmit all the data tho the bus
  rs485.println(message);
  rs485.flush();

  // Go back to listening
  delay(1);
  digitalWrite(RS485_CONTROL_PIN, LOW);

  Serial.println("[MASTER] RS485 message sent");
}

/*
 * Receives data via RS485
 */
void rs485Receive()
{

  // While there's data on the RS485 bus
  while (rs485.available())
  {

    // Read all incoming data until newline char
    String message = rs485.readStringUntil('\n');
    message.trim();

    // Print the command to the serial monitor (debug)
    if (message.length() > 0)
    {
      Serial.print("[MASTER] RS485 message received: ");
      Serial.println(message);
    }
  }
}
