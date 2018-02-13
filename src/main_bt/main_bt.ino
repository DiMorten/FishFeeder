#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
SoftwareSerial BT(10,11);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth
 
void setup()
{
  BT.begin(9600);       // Inicializamos el puerto serie BT que hemos creado
  Serial.begin(9600);   // Inicializamos  el puerto serie  
}

const byte bt_numChars = 32;
char bt_receivedChars[bt_numChars];   // an array to store the received data

boolean newData = false;

void loop()
{
  if(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
  {
    //Serial.write(BT.read());
    bt_recvWithEndMarker();
    bt_showNewData();
  }

  if(Serial.available())  // Si llega un dato por el monitor serial se envía al puerto BT
  {
     BT.write(Serial.read());
  }
}


void bt_recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    while (BT.available() > 0 && newData == false) {
        rc = BT.read();

        if (rc != endMarker) {
            bt_receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= bt_numChars) {
                ndx = bt_numChars - 1;
            }
        }
        else {
            bt_receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void bt_showNewData() {
    if (newData == true) {
        Serial.print("This just in ... \"");
        Serial.println(bt_receivedChars);
        Serial.print("\"");
        newData = false;
    }
}
