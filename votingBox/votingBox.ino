//=============================Settings=================================================
#define RF_RX 8
#define RF_TX 9

#define DEBUG 0

//=============================Libraries================================================
#include <SoftwareSerial.h>
SoftwareSerial RF(RF_RX, RF_TX); // (RX, TX)

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 16, 2);

#include <GyverTimer.h>
GTimer LCDTimer(MS);

#include <FS.h>
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncTCP.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
const char* ssid = "ESP";
const char* pass = "12345678";
AsyncWebServer server(80);
IPAddress apIP(192, 168, 1, 1);


#include <string.h>

//=============================Var======================================================
int totalYES = 0;
int totalNO = 0;
int val = 0;

//array
unsigned int point = 0;
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
boolean is = false;

//RFid temp data var
unsigned int seria1 = 0;
unsigned int no1 = 0;
unsigned int seria2 = 0;
unsigned int no2 = 0;

//----------------------------------вывод-переменных-на-сервер--------------------------------------------------------------------
String processor(const String& var) {
  if (var == "FIRSTVOIT") {
    return String(totalYES);
  }
  if (var == "SECONDVOIT") {
    return String(totalNO);
  }
  return String();
}
//------------------------------------------------------------------------------------------------------------
void matching(){
  for (int i = 0; i < arrSize; ++i) {
    if (voit[i][1] == 1) totalYES++;
    if (voit[i][2] == 1) totalNO++;
  }
}
void reboot(){
  for (int i = 0; i < arrSize; ++i) {
    voit[i][0] = 0;
    voit[i][1] = 0;
    voit[i][2] = 0;
    point = 0;
 }
}
void clearBufferArray1(){
  for (int i=0; i<count1;i++)
    { buffer1[i] = NULL;}                  // clear all index of array with command NULL
}

void clearBufferArray2(){
  for (int i=0; i<count2;i++)
    { buffer2[i] = NULL;}                  // clear all index of array with command NULL
}

void setup() {
  //=========================server=init======================================================================
  //(Soft)serial init.
  Serial.begin(9600);
  RF.begin(9600);
  
  WiFi.softAP(ssid, pass); //Добавьте(сотрите) параметр pass, если хотите, чтобы точка доступа была закрытой(открытой)
  //IP addres
  Serial.print("AP IP адрес: ");
  Serial.println(WiFi.softAPIP());
  // Выводим локальный IP
  Serial.println(WiFi.localIP());
    
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  //------------------------Server-pages-init----------------------------------------------------------------
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/about.html", String(), false, processor);
  });
  server.on("/contacts.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/contacts.html", String(), false, processor);
  });
  server.on("/firstvoit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(totalYES).c_str());
  });
  server.on("/secondvoit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(totalNO).c_str());
  });
  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/bootstrap.min.css", "text/css");
  });
  server.begin();
  //==========================================================================================================
  
  //timer init
  LCDTimer.setInterval(60000);

  //LCD init.
  lcd.init();
  lcd.backlight();
}

void loop() {
  //-----------------------------------------init-------------------------------------------------------

  no1 = 0;
  no2 = 0;
  seria1 = 0;
  seria2 = 0;

  if (LCDTimer.isReady()) {
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
  if (RF.available())              // if date is comming from softwareserial port ==> data is comming from SoftSerial shield
  {
    while (RF.available())         // reading data into char array
    {
      buffer1[count1++] = RF.read();   // writing data into array
      if (count1 == 64)break;
      delay(10);
    }
    Serial.write(buffer1, count1);           // if no data transmission ends, write buffer to hardware serial port
    //Переводим полученное от RFID из буфера 5-7 байты шестнацатеричного числа в десятичное число
    //Шестнадцатеричные числа представлены цифрами от 0 до 9 и буквами от A(10) до F(15)
    val = 0;
    for (int i = 5; i <= 6; i++) {
      val = val * 16 + (int) (buffer1[i] > '9' ? (buffer1[i] - 'A') + 10 : buffer1[i] - '0');
    }
    Serial.println("");
    Serial.print("Seria: ");
    Serial.println(val);
    //Переводим полученное от RFID из буфера 7-10 байты шестнацатеричного числа в десятичное число
    //Шестнадцатеричные числа представлены цифрами от 0 до 9 и буквами от A(10) до F(15)
    val = 0;
    for (int i = 7; i <= 10; i++) {
      val = val * 16 + (int) (buffer1[i] > '9' ? (buffer1[i] - 'A') + 10 : buffer1[i] - '0');
    }
    Serial.print("No: ");
    Serial.println(val);
    if (point == 0) {
      voit[point][0] = val;
      voit[point][1] = 1;
      voit[point][2] = 0;
      point++;
    } else {
      for (int i = 0; i < arrSize; ++i) {
        if (voit[i][0] == val) {
          is = true;
        }
      }
      if (!is) {
        voit[point][0] = val;
        voit[point][1] = 1;
        voit[point][2] = 0;
        point++;
      }
    }
    is = false;
    clearBufferArray1();              // call clearBufferArray function to clear the storaged data from the array
    count1 = 0;                       // set counter of while loop to zero
  }
  //-----------------------------------------second-rf--------------------------------------------------
  if (Serial.available())              // if date is comming from softwareserial port ==> data is comming from SoftSerial shield
  {
    while (Serial.available())         // reading data into char array
    {
      buffer2[count2++] = Serial.read();   // writing data into array
      if (count2 == 64)break;
      delay(10);
    }
    Serial.write(buffer2, count2);           // if no data transmission ends, write buffer to hardware serial port
    //Переводим полученное от RFID из буфера 5-7 байты шестнацатеричного числа в десятичное число
    //Шестнадцатеричные числа представлены цифрами от 0 до 9 и буквами от A(10) до F(15)
    val = 0;
    for (int i = 5; i <= 6; i++) {
      val = val * 16 + (int) (buffer2[i] > '9' ? (buffer2[i] - 'A') + 10 : buffer2[i] - '0');
    }
    Serial.println("");
    Serial.print("Seria: ");
    Serial.println(val);
    //Переводим полученное от RFID из буфера 7-10 байты шестнацатеричного числа в десятичное число
    //Шестнадцатеричные числа представлены цифрами от 0 до 9 и буквами от A(10) до F(15)
    val = 0;
    for (int i = 7; i <= 10; i++) {
      val = val * 16 + (int) (buffer2[i] > '9' ? (buffer2[i] - 'A') + 10 : buffer2[i] - '0');
    }
    Serial.print("No: ");
    Serial.println(val);
    if (point == 0) {
      voit[point][0] = val;
      voit[point][1] = 0;
      voit[point][2] = 1;
      point++;
    } else {
      for (int i = 0; i < arrSize; ++i) {
        if (voit[i][0] == val) {
          is = true;
        }
      }
      if (!is) {
        voit[point][0] = val;
        voit[point][1] = 0;
        voit[point][2] = 1;
        point++;
      }
    }

    clearBufferArray2();              // call clearBufferArray function to clear the storaged data from the array
    count2 = 0;                       // set counter of while loop to zero
  }
  //====================================================================================================

  for (int i = 0; i < arrSize; ++i) {
    Serial.println("---------------");
    Serial.print(voit[i][0]);
    Serial.print(" ");
    Serial.print(voit[i][1]);
    Serial.print(" ");
    Serial.print(voit[i][2]);
  }
  
  matching();
}
