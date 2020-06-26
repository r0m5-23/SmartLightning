# SmartLightning

Para facilitar a leitura deste ficheiro, usar o vscode (ou similar) e colocar em modo "__preview__".

## _2ª Fase_

A segunda fase consiste em fazer a leitura dos valores fornecidos pelos sensores e enviá-los para o The Things Network (TTN).<br>
O código completo encontra-se na pasta __SmartLightning_F2__.

## Connection

 ### Device Overview (TTN)

  ```plan
  Application ID: smartlighting-app
  Device EUI: 009472084173A903
  Application EUI: 70B3D57ED002E395
  App Key: 6A21B5D4855D9595F888E6096BC2444B
  ```
 Uma vez que o registo já foi feito na primeira fase, só temos que voltar a conectar o arduino ao TTN.

  ```c
    int connected = modem.joinOTAA(appEui, appKey);
  ```

## Reading and Sending Data

  ```c
    // Leitura do valor de luminosidade
    // Read the lux value
    lux = analogRead(lightSensor); 

    // Leitura do valor de tensão 
    // Read the voltage value
    analogValue = analogRead(ldrSensor); //(0-1023)
    voltage = analogValue * (5.0 / 1023.0); //(0V-5V)

    // Cálculo do valor da resistência 
    // Calculate the resistance value
    resLrd = ((10000.0 * 5.0)/voltage) - 10000.0; //Low (voltage>>>2,5) ; High (voltage<<<2.4)

    // Média de luminosidade e resistência por minuto (60000ms)
    // Average lux and resistance per minute (60000ms)
    if (millis() - time > 60000){
      luxValue = (float)lux * 0.6 + (float)luxValue * 0.4;
      lux = 0;
    
      voltageValue = (float)voltage * 0.6 + (float)voltageValue * 0.4;
      voltage = 0;

      // Menor Luminosidade, menor tensão, maior resitência! LED é ligado
      // Lower lux, lower voltage, higher resistance! LED ON
      if(voltageValue <= 2){
        digitalWrite(LED, HIGH);
        ledValue = 1;
        Serial.println("It's too dark, LED turn on!");

      // Maior Luminosidade, maior tensão, menor resitência! LED é desligado
      // Higher lux, higher voltage, lower resistance! LED OFF
      } else {
        digitalWrite(LED, LOW);
        ledValue = 0;
        Serial.println("It's too bright, LED turn off!");
      }

      // Redefinir o tempo 
      // Reset time
      time = millis();
    }

    // Enviar valores para o TTN de 10 em 10 minutos (600000ms)
    // Send values to TTN every 10 minutes (600000ms)
    if (millis() - timeUpload > 600000){

      // Processamento dos dados em hexadecimal
      // Hexadecimal data processing
      int valLux = (int)(luxValue * 10);
      txBuffer[0] = (valLux >> 8) & 0xff;
      txBuffer[1] = valLux & 0xff;

      int valVol = (int)(voltageValue * 10);
      txBuffer[2] = (valVol >> 8) & 0xff;
      txBuffer[3] = valVol & 0xff;

      txBuffer[4] = ledValue & 0xff;

      // Envio de dados
      // Sending data
      int err;
      modem.beginPacket();
      modem.write(txBuffer);
      err = modem.endPacket(true);

      // Redefinir o tempo de envio
      // Reset upload time
      timeUpload = millis();
    }
  ```

## _3ª Fase_

A terceira fase do código dá-se no The Things Network (TTN) e consiste em descodificar os valores recebidos pelo dispositivo.<br>
O código completo encontra-se na pasta __SmartLightning_F3__.

 ### Payload Formats (TTN)

  ```js
    function Decoder(bytes, port) {

      var decoded = {};

      if (port === 1){
        decoded.lux = ((bytes[0] << 8) | bytes[1]);
        decoded.vol = ((bytes[2] << 8) | bytes[3]);
        decoded.led = bytes[4]
        if(decoded.led == 1) decoded.Status = "LED ON";
        if(decoded.led === 0) decoded.Status = "LED OFF";
      }  

      return{
        field1: decoded.lux / 10,
        field2: decoded.vol / 10,
        field3: decoded.led,
        status: decoded.Status
      }   
    }
  ```

 ### Payload - Test (TTN)

  ```plan
  3A 98 00 1E 00
  ```

 __Result:__

  ```plan
  {
    "field1": 1500,
    "field2": 3,
    "field3": 0,
    "status": "LED OFF"
  }
  ```

## Trabalho Realizado por

> Renato Mendes - 65384