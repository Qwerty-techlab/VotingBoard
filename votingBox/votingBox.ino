//=============================Settings=================================================
#define RF_ONE_RX 8
#define RF_ONE_TX 9
#define RF_TWO_RX 10
#define RF_TWO_TX 11

#define LED_ONE 6
#define LED_TWO 7

#define BTN_PIN 3

#define DEBUG 0
//======================================================================================

//=============================Libraries================================================
#include <SoftwareSerial.h>
SoftwareSerial RFone(RF_ONE_RX, RF_ONE_TX); // (RX, TX)
SoftwareSerial RFtwo(RF_TWO_RX, RF_TWO_TX);

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 16, 2);

#include <GyverButton.h>
GButton resetbtn(BTN_PIN);

#include <GyverTimer.h>
GTimer LCDTimer(MS);

#include <Arduino.h>

#include <ESP8266WiFi.h>

#include <Hash.h>

#include <ESPAsyncTCP.h>

#include <ESPAsyncWebServer.h>

#include <string.h>

//=============================Var======================================================
//wifi 
const char ssidSTA[] = ""; // your ssid
const char passSTA[] = ""; // your password
const char ssidAP[] = "ESP_AP";
const char passAP[] = "";
IPAddress apIP(192, 168, 1, 1);

int totalYES = 0;
int totalNO = 0;
int val = 0;

//array
unsigned int posit = 0;
const unsigned int arrSize = 4;
unsigned int voit[arrSize][3];
/*
  |0  |1  |2  |
  |---|---|---|
  |Ser|Yes|No |
*/
unsigned char buffer1[64]; // buffer array for data recieve over serial port
unsigned char buffer2[64]; // buffer array for data recieve over serial port
int count1 = 0;            // counter for buffer array
int count2 = 0; 

//RFid temp data var
unsigned int seria1 = 0;
unsigned int no1 = 0;
unsigned int seria2 = 0;
unsigned int no2 = 0;

// =================================HTML=страницы================================================================================

//EXAMPLE
/*const char #NAME#[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
   <html>
     <head></head>
     <bode></body>
   </html>)rawliteral";  
*/
//----------------------------------Index-----------------------------------------------------------------------------------------
const char index[] PROGMEM = R"rawliteral(
  )rawliteral";
//================================================================================================================================



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
String processor(const String& var){
  if(var == ""){
    return String();
  }
  return String();
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


String outputState() {
    if (digitalRead(output)) {
        return "checked";
    }
    else {
        return "";
    }
    return "";
}

void setup() {
  //server init
  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(true);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssidAP, passAP);
  WiFi.begin(ssidSTA,passSTA);
  IPAddress staIp = WiFi.localIP();
  Serial.printf("\nDevice connected to router, ip: %u.%u.%u.%u\n",staIp[0],staIp[1],staIp[2],staIp[3]);
  Serial.printf("\nEnable AP, ssid: %s, ip: %u.%u.%u.%u\n",ssidAP,apIP[0],apIP[1],apIP[2],apIP[3]);


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temperature).c_str());
  });
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
      String inputMessage;
      String inputParam;
      // получаем значение input1 <ESP_IP>/update?state=<inputMessage>
      if (request->hasParam(PARAM_INPUT_1)) {
          inputMessage = request->getParam(PARAM_INPUT_1)->value();
          inputParam = PARAM_INPUT_1;
          digitalWrite(output, inputMessage.toInt());
          ledState = !ledState;
      }
      else {
          inputMessage = "No message sent";
          inputParam = "none";
      }
      Serial.println(inputMessage);
      request->send(200, "text/plain", "OK");
      });

  // Отправляем запрос GET на <ESP_IP>/state
  server.on("/state", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(200, "text/plain", String(digitalRead(output)).c_str());
      });
  
  server.begin();

  //(Soft)serial init.
  Serial.begin(9600);
  RFone.begin(9600);
  RFtwo.begin(9600);

  //timer init
  LCDTimer.setInterval(60000);
  
  //led init.
  pinMode(LED_ONE, OUTPUT);
  digitalWrite(LED_ONE, LOW);
  pinMode(LED_TWO, OUTPUT);
  digitalWrite(LED_TWO, LOW);

  //LCD init.
  lcd.init();
  lcd.backlight();
  
  //hello message
  lcd.setCursor(0, 4);
  lcd.print("VotingBox");
  lcd.setCursor(1, 4);
  lcd.print("V0.1");
  delay(500);
}
void loop() {
  //-----------------------------------------init-------------------------------------------------------
  resetbtn.tick();
  
  no1 = 0;
  no2 = 0;
  seria1 = 0;
  seria2 = 0;

  if(LCDTimer.isReady()){
    lcd.setCursor(0, 0);
    lcd.print("YES");
    lcd.setCursor(1, 0);
    lcd.print(totalYES + "   ");

    lcd.setCursor(0, 13);
    lcd.print("NO");
    lcd.setCursor(1, 13);
    lcd.print(totalNO + "   ");
  }
  //=========================================RFid=======================================================
  //-----------------------------------------first-rf---------------------------------------------------
  
  //-----------------------------------------second-rf--------------------------------------------------
  
  //====================================================================================================
  
  //reboot
  if(resetbtn.isTriple()) reboot();
}

void match(){
  for(int i = 0; i<arrSize; ++i){
    if(voit[i][1] == 1) totalYES++;
    if(voit[i][2] == 1) totalNO++;
  }
}
void reboot(){
  for(int i = 0; i<arrSize; ++i){
    voit[i][0] = 0;
    voit[i][1] = 0;
    voit[i][2] = 0;
    posit = 0;
  }
}
