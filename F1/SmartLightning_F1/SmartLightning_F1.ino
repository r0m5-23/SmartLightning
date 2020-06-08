#include <MKRWAN.h>

LoRaModem modem;

#include "arduino_secrets.h"

String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

uint8_t txBuffer[1];

void setup() {
  
  Serial.begin(115200);
  while (!Serial);
  
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }
  modem.minPollInterval(60);
  //Só permite enviar uma mensagem a cada 2min
}

void loop() {
  Serial.println();
  Serial.println("Enter a message to send to network");
  Serial.println("(make sure that end-of-line 'NL' is enabled)");

  while (!Serial.available());
  
  char msg = "123";
  
  Serial.println();
  Serial.print("Sending: " + msg);
  Serial.println();

  txBuffer[0]= msg & 0xFF;
  
  int err;
  modem.beginPacket();
  modem.write(txBuffer);
  err = modem.endPacket(true);
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
    Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  }
  delay(1000);
}
