# Smart Light

Para facilitar a leitura deste ficheiro, usar o vscode (ou similar) e colocar em modo "__preview__".

## _1ª Fase_

A primeira fase do código consiste em registar o dispositivo, conectá-lo ao The Things Network (TTN) e testar o envio de dados.<br>
O código completo encontra-se nos ficheiros `SmartLightning(F1).ino` e `arduino_secrets.h`.

## Registration

  ```c
    Serial.println(modem.deviceEUI());
  ```
 __Result:__

  ```plan
  Your device EUI is: 009472084173A903
  ```

 ### Device Overview (TTN)

  ```plan
  Application ID: smartlighting-app
  Device EUI: 009472084173A903
  Application EUI: 70B3D57ED002E395
  App Key: 6A21B5D4855D9595F888E6096BC2444B
  ```
 
 ### Example code (TTN)

  ```js
    const char *appEui = "70B3D57ED002E395";
    const char *appKey = "6A21B5D4855D9595F888E6096BC2444B";
  ```

## Connection

  ```c
    int connected = modem.joinOTAA(appEui, appKey);
  ```

## Envio de dados para o servidor

  ```c
    char msg = "123";
    txBuffer[0]= msg & 0xFF;
    modem.beginPacket();
    modem.print(txBuffer);
  ```


 ### Payload Formats (TTN)

  ```js
    function Decoder(bytes, port) {
        var decoded = {};

        if (port === 1) decoded.msg = bytes[0];

        return decoded;
    }
  ```

 ### Payload - Test (TTN)

  ```plan
  7B
  ```

 __Result:__

  ```plan
  {
    "msg": 123
  }
  ```

## Trabalho Realizado por

> Renato Mendes - 65384