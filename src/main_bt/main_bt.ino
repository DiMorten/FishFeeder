#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
#include <DS3231.h>

SoftwareSerial BT(10,11);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth
DS3231  rtc(SDA, SCL);   // Init the DS3231 using the hardware interface



const byte bt_numChars = 32;
char bt_receivedChars[bt_numChars];   // an array to store the received data
boolean newData = false;


typedef struct {
  uint8_t h;
  uint8_t m;
}__attribute__ ((__packed__)) TimeValues;

typedef struct {
  TimeValues now;
  TimeValues alarm;
  char receivedChars[bt_numChars];
  uint8_t alarm_ring; //1 when alarm is ringing
}__attribute__ ((__packed__)) TimeData;

static TimeData td;
void setup()
{
  

  BT.begin(9600);       // Inicializamos el puerto serie BT que hemos creado
  Serial.begin(9600);   // Inicializamos  el puerto serie  
  pinMode(13, OUTPUT); //Establecer el pin 13 como salida
  rtc.begin();
  td.alarm.h=0;
  td.alarm.m=0;
  td.alarm_ring=0; 
}


void loop()
{
  if(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
  {
    //Serial.write(BT.read());
    bt_recvWithEndMarker();
    bt_showNewData();
    alarm_set(td);
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
            //bt_receivedChars[ndx] = rc;
            td.receivedChars[ndx] = rc;
            //Serial.println(ndx);
            ndx++;
            if (ndx >= bt_numChars) {
                ndx = bt_numChars - 1;
            }
        }
        else {
            //bt_receivedChars[ndx] = '\0'; // terminate the string
            td.receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
            Serial.println("newData");
        }
    }
}

void bt_showNewData() {
    if (newData == true) {
        Serial.print("This just in ... \"");
        //Serial.print(bt_receivedChars);
        Serial.print(td.receivedChars);
        Serial.println("\"");
        newData = false;
    }
}

void alarm_set(TimeData *td) {
  // td.receivedChars is of type 12h23m
  for(int i=0;i<=5;i++) {
    Serial.println(td->receivedChars[i]);
  }
  td->now.h=td->alarm.h;
  td->now.m=td->alarm.m;
}

