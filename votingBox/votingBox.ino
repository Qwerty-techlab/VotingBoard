//hours spend: 4

#define WIFI_RX 10
#define WIFI_TX 11
#define RF_ONE_RX 12
#define RF_TWO_RX 13
#define LED_ONE 
#define LED_TWO 

#include <string.h>

#include <SoftwareSerial.h>
SoftwareSerial wifi(WIFI_RX, WIFI_TX);// (RX, TX)
SoftwareSerial RFone(RF_ONE_RX);
SoftwareSerial RFtwo(RF_TWO_RX);

#include <RDM6300.h>
RDM6300<SoftwareSerial> rdmone(&RFone);
RDM6300<SoftwareSerial> rdmtwo(&RFtwo);

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
    unsigned long long lastID_one;
    unsigned long long lastID_two;
    
    if (rdmone.available()){
        voit[point][0] = rdmone.read();
        
        lastID_one = rdmone.read();
        
        for(int i = 0; i<=499; i++){
            if(lastID_one == voit[i][0]) break;
        }
        
        voit[point][1] = 1;
        voit[point][2] = 0;
        
        point++
    }

    if (rdmtwo.available()){
        voit[point][0] = rdmtwo.read();
        
        lastID_two = rdmtwo.read();
        
        for(int i = 0; i<=499; i++){
            if(lastID_two == voit[i][0]) break;
        }
        
        voit[poit][1] = 0;
        voit[point][2] = 1;
        
        point++
    }
    
    match();
    
}

void match(){
    for(int i = 0; i<=499; i++){      
        if(voit[i][1] == 1) totalYES++;
        if(voit[i][2] == 1) totalNO++;
    }
}
