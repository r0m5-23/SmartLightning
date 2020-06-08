#include <MKRWAN.h>

LoRaModem modem;

#include "arduino_secrets.h"

String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

//Pins
int lightSensor = A0;
int ldrSensor = A1;
int LED = 2;

//Variables
int analogValue = 0;
float voltage = 0.0;
float voltageValue = 0.0;
float resLrd = 0.0;
float resValue = 0.0;
float lux = 0.0;
float luxValue = 0.0;
uint8_t txBuffer[6];
unsigned long time;
unsigned long timeUpload;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(lightSensor, INPUT);
  pinMode(ldrSensor, INPUT);
  time = millis();
  timeUpload = millis();

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

void loop()
{
  lux = analogRead(lightSensor);

  analogValue = analogRead(ldrSensor); //(0-1023)
  voltage = analogValue * (5.0 / 1023.0); //(0V-5V)
  resLrd = ((10000.0 * 5.0)/voltage) - 10000.0; //Low (voltage>>>2,5) ; High (voltage<<<2.4)
  
  if (millis() - time > 60000){
    luxValue = (float)lux * 0.6 + (float)luxValue * 0.4;
    lux = 0;
    
    resValue = (float)resLrd * 0.6 + (float)resValue * 0.4;
    resLrd = 0;

    voltageValue = (float)lux * 0.6 + (float)luxValue * 0.4;
    voltage = 0;

    if(voltageValue <= 2){
      digitalWrite(LED, HIGH);
      Serial.println("It's too dark, LED turn on!");
    }else if (voltageValue >= 3 ){
      digitalWrite(LED, LOW);
      Serial.println("It's too bright, LED turn off!");
    }

    time = millis();
  }

  if (millis() - timeUpload > 600000)
  {

    // Data processing
    int valLux = (int)(luxValue * 10);
    txBuffer[0] = (valLux >> 8) & 0xff;
    txBuffer[1] = valLux & 0xff;

    int valVol = (int)(voltageValue * 10);
    txBuffer[2] = (valVol >> 8) & 0xff;
    txBuffer[3] = valVol & 0xff;

    int valRes = (int) (resValue * 10);
    txBuffer[4] = (valRes >> 8) & 0xff;
    txBuffer[5] = valRes & 0xff;

    //Serial.println(txBuffer[0], HEX);
    //Serial.println(txBuffer[1], HEX);
    //Serial.println(txBuffer[2], HEX);
    //Serial.println(txBuffer[3], HEX);
    //Serial.println(txBuffer[4], HEX);
    //Serial.println(txBuffer[5], HEX);

    // Sending data
    int err;
    modem.beginPacket();
    modem.write(txBuffer);
    err = modem.endPacket(true);
    if (err > 0)
    {
      Serial.println("Message sent correctly!");
    }
    else
    {
      Serial.println("Error sending message :(");
      Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
      Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
    }

    // Reset timer
    timeUpload = millis();
  }

  delay(50);
}
