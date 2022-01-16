//=============================Settings=================================================
#define RF_ONE_RX 8
#define RF_ONE_TX 9
#define RF_TWO_RX 10
#define RF_TWO_TX 11

#define LED_ONE 6
#define LED_TWO 7

#define DEBUG 0
//======================================================================================

//=============================Libraries================================================
#include <SoftwareSerial.h>
SoftwareSerial RFone(RF_ONE_RX, RF_ONE_TX); // (RX, TX)
SoftwareSerial RFtwo(RF_TWO_RX, RF_TWO_TX);

//======================================================================================
int totalYES = 0;
int totalNO = 0;

unsigned int point = 0;
const unsigned int arrSize = 4;
unsigned long int voit[arrSize][4];
/*
  |0 |1  |2  |3 |
  |No|Ser|Yes|No|
*/

unsigned char buffer[64]; // buffer array for data recieve over serial port
int count = 0;            // counter for buffer array

unsigned int seria1 = 0;
unsigned int no1 = 0;
unsigned int seria2 = 0;
unsigned int no2 = 0;



void setup() {
  Serial.begin(9600);
  RFone.begin(9600);
  RFtwo.begin(9600);

  pinMode(LED_ONE, OUTPUT);
  digitalWrite(LED_ONE, LOW);
  pinMode(LED_TWO, OUTPUT);
  digitalWrite(LED_TWO, LOW);
}

void loop() {

  bool is = false;
  no1 = 0;
  no2 = 0;
  seria1 = 0;
  seria2 = 0;

  //=========================================RFid=======================================================
  if (RFone.available())
  {
    while (RFone.available())
    {
      buffer[count++] = RFone.read();
      if (count == 64)break;
      delay(10);
    }

    for (int i = 5; i <= 6; i++) {
      seria1 = seria1 * 16 + (int) (buffer[i] > '9' ? (buffer[i] - 'A') + 10 : buffer[i] - '0');
    }
    for (int i = 7; i <= 10; i++) {
      no1 = no1 * 16 + (int) (buffer[i] > '9' ? (buffer[i] - 'A') + 10 : buffer[i] - '0');
    }

    for (int i = 0; i <= point; ++i) { //check card
      if (seria1 == voit[i][1] && no1 == voit[i][0]) {
        voit[i][2] = 1;
        voit[i][3] = 0;
      } else {
        voit[point][0] = no1;
        voit[point][1] = seria1;
        voit[point][2] = 1;
        voit[point][3] = 0;
        point++;
      }
    }

    clearBufferArray();
    count = 0;
    Serial.print(seria1 + " " + no1);
  }
  //-----------------second-RF----------------------------------------------------------------------------
  if (RFtwo.available())
  {
    while (RFtwo.available())
    {
      buffer[count++] = RFtwo.read();
      if (count == 64)break;
      delay(10);
    }

    for (int i = 5; i <= 6; i++) {
      seria2 = seria2 * 16 + (int) (buffer[i] > '9' ? (buffer[i] - 'A') + 10 : buffer[i] - '0');
    }
    for (int i = 7; i <= 10; i++) {
      no2 = no2 * 16 + (int) (buffer[i] > '9' ? (buffer[i] - 'A') + 10 : buffer[i] - '0');
    }

    for (int i = 0; i <= point; ++i) {
      if (seria2 == voit[i][1] && no2 == voit[i][0]) {
        voit[i][2] = 0;
        voit[i][3] = 1;
      } else {
        voit[point][0] = no2;
        voit[point][1] = seria2;
        voit[point][2] = 0;
        voit[point][3] = 1;
        point++;
      }
    }

    clearBufferArray();
    count = 0;
    Serial.print(seria2 + " " + no2);
  }
  //=========================================RFid=======================================================

  match();
#if(DEBUG == 1)
  debug();
#endif
}

void match() {                         //function to match voits
  for (int i = 0; i <= arrSize; i++) {
    if (voit[i][2] == 1) totalYES++;
    if (voit[i][3] == 1) totalNO++;
  }
}

void reset() {                         //function to total reset
  point = 0;
  totalNO = 0;
  totalYES = 0;

  for (int i = 0; i <= arrSize; ++i) {
    voit[i][0] = 0;
    voit[i][2] = 0;
    voit[i][2] = 0;
    voit[i][3] = 0;
  }
}

void clearBufferArray() {               // function to clear buffer array
  for (int i = 0; i < count; i++)
  {
    buffer[i] = NULL;
  }
}

void debug() {
Serial.println("----------------Debug----------------");
 for (int i = 0; i <= point; ++i) {
    Serial.print(voit[i][0]);
    Serial.print(" ");
    Serial.print(voit[i][1]);
    Serial.print(" ");
    Serial.print(voit[i][2]);
    Serial.print(" ");
    Serial.println(voit[i][3]);

  }
Serial.println("----------------Debug----------------");
}
