//hours spend: 3

#define WIFI_RX 10
#define WIFI_TX 11
#define RF_ONE_RX 12
#define RF_TWO_RX 13
#define LED_ONE 
#define LED_TWO 

#include <string.h>

#include <rdm6300.h>
Rdm6300 rdmone;
Rdm6300 rdmtwo;

#include <SoftwareSerial.h>
SoftwareSerial wifi(WIFI_RX, WIFI_TX);// (RX, TX)
SoftwareSerial RFone(RF_ONE_RX, RF_ONE_TX);
SoftwareSerial RFtwo(RF_TWO_RX, RF_TWO_TX);

File keys;

int totalYES = 0;
int totalNO = 0;
unsigned long long int voit[500][3];
int point = 0;

void setup(){
    Serial.begin(9600);

    rdmone.begin(RF_ONE_RX);
    rdmtwo.begin(RF_TWO_RX);

    pinMode(LED_ONE, OUTPUT);
    digitalWrite(LED_ONE, LOW);
    pinMode(LED_TWO, OUTPUT);
    digitalWrite(LED_TWO, LOW);
}
void loop(){
    if (rdmone.update()){
        voit[point][0] = rdmone.get_tag_id();
        voit[poit][1] = 1;
        voit[point][2] = 0;
        point++
    }

    if (rdmtwo.update()){
        voit[point][0] = rdmtwo.get_tag_id();
        voit[poit][1] = 0;
        voit[point][2] = 1;
        point++
    }
    
    for(int i = 0; i<=499; i++){      
        if(voit[i][1] == 1) totalYES++;
        if(voit[i][2] == 1) totalNO++;
    }
}