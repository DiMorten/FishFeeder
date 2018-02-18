#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
#include <DS3231.h>
#include <Servo.h>

SoftwareSerial BT(10,11);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth
DS3231  rtc(SDA, SCL);   // Init the DS3231 using the hardware interface
Servo myservo;  // create servo object to control a servo



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
  char receivedChars_h[3];
  char receivedChars_m[3];
  uint8_t date_was_set_flag;
  uint8_t dow_value;
  uint8_t day_alarm;
  uint8_t alarm_ring; //1 when alarm is ringing
}__attribute__ ((__packed__)) TimeData;

static TimeData td;
Time t;
void setup()
{
  
  myservo.attach(A4);  // attaches the servo on pin 9 to the servo object
  BT.begin(9600);       // Inicializamos el puerto serie BT que hemos creado
  Serial.begin(9600);   // Inicializamos  el puerto serie  
  pinMode(13, OUTPUT); //Establecer el pin 13 como salida
  rtc.begin();
  td.alarm.h=0;
  td.alarm.m=0;
  td.alarm_ring=0; 
  td.date_was_set_flag=0;
  td.day_alarm=0;
  Serial.print(rtc.getTimeStr());
  Serial.print(" ");
  
}


void loop()
{
  if(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
  {
    //Serial.write(BT.read());
    bt_recvWithEndMarker();
    bt_showNewData();
    alarm_set(&td);
    Serial.println("td.alarm_ring");
    Serial.println(td.alarm_ring);    
  }

  if(Serial.available())  // Si llega un dato por el monitor serial se envía al puerto BT
  {
     BT.write(Serial.read());
  }

    if(td.alarm_ring==1) {
      delay(1000);
      rtc_check(&td);
    }
    
}


void bt_recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    while (BT.available() > 0 && newData == false) {
        rc = BT.read();

        if (rc != endMarker) {
            td.receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= bt_numChars) {
                ndx = bt_numChars - 1;
            }
        }
        else {
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
        Serial.print(td.receivedChars);
        Serial.println("\"");
        newData = false;
    }
}

void alarm_set(TimeData *td) {
  Serial.println("here");
  Serial.println(td->receivedChars[0]);
  // td.receivedChars is of type 12h23m
  if(td->receivedChars[0]=='r') {
    td->alarm_ring=1;
    int i=1;
    int i_h=0;
    int i_m=0;
    Serial.println("hour:");
    while(td->receivedChars[i]!='h' && i<=3) {
      Serial.println("rec");      
      
      Serial.println(td->receivedChars[i]);
      td->receivedChars_h[i_h]=td->receivedChars[i];
      Serial.println("iter");      
      Serial.println(i);
      Serial.println(i_h);
      Serial.println("end iter");

      i++;
      i_h++;
    }
    i++;
    Serial.println("min:");
    while(td->receivedChars[i]!='m' && i<=6) {
      Serial.println("rec");      
      
      Serial.println(td->receivedChars[i]);
      td->receivedChars_m[i_m]=td->receivedChars[i];
      Serial.println("iter");      
      Serial.println(i);
      Serial.println(i_m);
      Serial.println("end iter");

      i++;
      i_m++;
    }/*
    /*

    
    i++;
    while(td->receivedChars[i]!='m') {
      td->receivedChars_m[i_m]=td->receivedChars[i];
      i++;
      i_m++;
    }*/
    Serial.println(i);
    Serial.println(i_h);
    Serial.println(i_m);
    
    
  }
  else if(td->receivedChars[0]=='d') {
    td->date_was_set_flag=1;
    td->dow_value=atoi(td->receivedChars[1]);
    if(td->dow_value==0) {
      td->day_alarm=1;
    }
  }
    
  int h_val = atoi(&td->receivedChars_h[0]);  
  int m_val = atoi(&td->receivedChars_m[0]);  
  

  td->alarm.h=(uint8_t)h_val;
  td->alarm.m=(uint8_t)m_val;
  Serial.println(td->alarm.h);
  Serial.println(td->alarm.m);  
}

void rtc_check(TimeData *td) {
  
    t=rtc.getTime();
    if(t.dow==td->dow_value-1) {
      td->day_alarm=1;
    }
    //Serial.println(t.hour, DEC);
    //Serial.println(t.min, DEC);
    if(t.hour==td->alarm.h && t.min==td->alarm.m && td->day_alarm!=0) {
      myservo.write(160);
      digitalWrite(13, HIGH);
      Serial.println("LED encendido");
      delay(1000);
      myservo.write(20);
      digitalWrite(13, LOW);
      Serial.println("LED apagado");      
      td->alarm_ring=0;
      td->day_alarm=0;
    }
    
}

