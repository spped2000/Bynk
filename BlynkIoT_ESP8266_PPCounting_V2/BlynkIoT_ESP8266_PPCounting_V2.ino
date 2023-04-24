#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial NodeSerial(D1, D2); // RX | TX

int Case, sensor1, sensor2, count1, lastcount1, count2, lastcount2;
int i, j,ioff,modecount,resetblynk=0;





/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************
  Blynk.Edgent implements:
  - Blynk.Inject - Dynamic WiFi credentials provisioning
  - Blynk.Air    - Over The Air firmware updates
  - Device state indication using a physical LED
  - Credentials reset using a physical Button
 *************************************************************/

/* Fill in information from your Blynk Template here */
/* Read more: https://bit.ly/BlynkInject */

#define BLYNK_TEMPLATE_ID "TMPL6gCG1bKUn"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "Y7QHdxIsDXrZy8V_tYSUcpZ8HNE4XVXP"

//====================================================================================
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
//---------------------------------------------
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const long utcOffsetInSeconds = 25200;
int n=0;


//----------------------------------------Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//---------------------------------------------
//---------------------------------------------
#define FIREBASE_HOST "https://test-tfs-f09fa-default-rtdb.asia-southeast1.firebasedatabase.app/"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "AIzaSyDZZlRS6fgLii26fJWUUlGHOLYMJkxIzOg" //Your Firebase Database Secret goes here


char ssid[] = "Jukree";
char pass[] = "00000000";
//=====================================================================================

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"

BlynkTimer timer;

//====================================================================================
// + date
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


//=======================================================================================
//===================================================================================================================************
//+ format
String branchId = "SH000";//"SH001-1"; //063Mega Bangna   062seacon
String cameraId = "5";

// type ไปกำหนด ตอนส่งค่า
int datasize = 5;
String off[50],datacollection[50];
String countoff[50];

//====================================================================================================================
// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;
//=======================================================================================

//33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333----timer

#if !defined(ESP8266)
  #error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         1
#define _TIMERINTERRUPT_LOGLEVEL_     1

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

#include "ESP8266TimerInterrupt.h"

#define BUILTIN_LED     2       // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED

volatile bool statusLed = false;
volatile uint32_t lastMillis = 0;
//33333333333333333333333333333333333333333333333333333333333333333333333

// for timer
int it,hrr,mnn,scc;
int offday,offmonth,offyear;

//33333333333333333333333333333333333333333333333333333333333333333333333

#define TIMER_INTERVAL_MS       1000

// Init ESP8266 timer 1
ESP8266Timer ITimer;

//=======================================================================

void IRAM_ATTR TimerHandler()
{
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(BUILTIN_LED, OUTPUT);
  }

  digitalWrite(BUILTIN_LED, statusLed);  //Toggle LED Pin
  statusLed = !statusLed;

#if (TIMER_INTERRUPT_DEBUG > 0)
 // Serial.println("Delta ms = " + String(millis() - lastMillis));
  lastMillis = millis();
  if(modecount==1){
    scc = scc+1;
    if(scc==60){
      mnn = mnn+1;
      scc = 0;
      if(mnn==60){
        hrr = hrr+1;
        mnn = 0;
      }
    }
  }
#endif
}
//33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333


// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  digitalWrite(D4, !digitalRead(D4));
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  //-----------------------------------##
   pinMode(D1, INPUT);
   pinMode(D2, OUTPUT); 

   NodeSerial.begin(57600);
  //-----------------------------------##
  Serial.begin(115200);

  
   
  pinMode(D4, OUTPUT);
  delay(100);

 // timer.setInterval(1000L, myTimerEvent);

  //BlynkEdgent.begin();

      //=======================================================================================
  Serial.println("Serial communication started\n\n");  
           
  WiFi.begin(ssid, pass);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(ssid);

  

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  BlynkEdgent.begin();
  
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address

  
  //----------------------------------------
  timeClient.begin();
  // + date
  timeClient.setTimeOffset(25200);
  //UMT+7 => 3600*7
  //----------------------------------------
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase

  Firebase.reconnectWiFi(true);
//=======================================================================================

  delay (1000);
  //33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333
  while (!Serial);

  delay(300);

  Serial.print(F("\nStarting TimerInterruptTest on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(lastMillis);

  }
  else
    Serial.println(F("Can't set ITimer correctly. Select another freq. or interval"));

//33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333


  
}

void loop() {

  //=================================================================================
     String hr, mn, sc;
     String currentDate;
     String TimeNow;
     String strhrr,strmnn,strscc;

     int monthDay,currentMonth,currentYear;
     String strtimesecond;
//----------

        //++++++++++++++++++++++++++
 if(WiFi.RSSI()> 0 || WiFi.RSSI() < -82){
  modecount = 1;                                                       //offline 

   if(resetblynk == 2){
  resetblynk = resetblynk+1;
  }
  
//  while(WiFi.status() != WL_CONNECTED){
//    off[ioff] = data;
//    Serial.println(off[ioff]);
//    Serial.println("88888888888888888");
    


 //   ioff++;
 
//    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
 }//try to connect with wifi
 else{
  modecount = 2;                                                       //online

  // sendOffline();
  if(resetblynk < 2){
  resetblynk = resetblynk+1;
  }
 // Serial.print("resetblynk = ");
 // Serial.println(resetblynk);

  if(resetblynk <= 2){
  BlynkEdgent.run();  //------------------------------------------------------------------------------------------------- blynkIoT run
//  timer.run();      //-------------------------------------------------------------------------------------------------
  }
//    for(ioff=0;ioff<10;ioff++){
//      Serial.println(off[ioff]);
//     Serial.println("9999999999999999999999");

//      Firebase.setString(firebaseData,off[ioff],strcount);
//
    
 //   ioff = 0;
 
       timeClient.update();
   
    if (timeClient.getHours() < 10) {
      hr = "0" + String(timeClient.getHours());
      hrr = hr.toInt();
    }
    else {
      hr = String(timeClient.getHours());
      hrr = hr.toInt();
    }
    
    if (timeClient.getMinutes() < 10) {
      mn = "0" + String(timeClient.getMinutes());
      mnn = mn.toInt();
    }
    else {
      mn = String(timeClient.getMinutes());
      mnn = mn.toInt();

    }
  
    if (timeClient.getSeconds() < 10) {
      sc = "0" + String(timeClient.getSeconds());
      scc = sc.toInt();
    }
    else {
      sc = String(timeClient.getSeconds());
      scc = sc.toInt();
    }
    //*************************************************************************************************************************************************************************************
    strtimesecond = "Data/Status/SH000-5-Status/";//"Data/SH040-2-Status/";
   // scc = scc*2;
    Firebase.setInt(firebaseData,strtimesecond,scc);
    Serial.println(scc);
    //Serial.println(WiFi.RSSI());
    
    //***********************************************************************************************************************
    //--------------------------------------------------------------------------------
    // date
    
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime); 
    
    monthDay = ptm->tm_mday;
    offday=monthDay;
    currentMonth = ptm->tm_mon+1;
    offmonth = currentMonth;
    currentYear = ptm->tm_year+1900;
    offyear = currentYear;

    //--------------------------------------------------------------------------------
   }
   
// }
 //++++++++++++++++++++++++++
    
   
    //Serial.println(TimeNow);
    if(modecount==2) {                                              //online
     currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
     TimeNow = currentDate + " " + hr + ":" + mn + ":" + sc;
    }
    if(modecount==1){                                                                                 //offline
     currentDate = String(offyear) + "-" + String(offmonth) + "-" + String(offday);
     //---------------------------------------------
     //ใส่เลข 0 ข้างหน้าเลขที่ <10
     //---------------------------------------------
     if (hrr<10){
      strhrr = "0" +String(hrr);
     }
     else
      strhrr = hrr;
    if (mnn<10){
      strmnn = "0" +String(mnn);
     }
     else
      strmnn = mnn;
     if (scc<10){
      strscc = "0" +String(scc);
     }
     else
      strscc = scc;  
     
     TimeNow = currentDate + " " + strhrr + ":" + strmnn + ":" + strscc;
    }
    if(n==5){
      String collection = branchId + " " + cameraId + " " + "in" ;
//      Serial.print(hrr);
//      Serial.print(":");
//      Serial.print(mnn);
//      Serial.print(":");
//      Serial.println(scc);      
      Timein(collection,count1,TimeNow);
      n=0;
    }
//    if(n==3){
//      String collection1 = branchId + " " + cameraId + " " + "out" ;
//      Timeout(collection1,count2,TimeNow);
//      n=0;
//    }
    

//=================================================================================#######
//ใส่เงื่อนไข คนเข้า หรือ ออก
  //-----------------------------------------------------------------------##
 while (NodeSerial.available() > 0)
  {
    int i_data = NodeSerial.parseInt();
  //-----------------------------------------------------------------------##
    if (NodeSerial.read() >= 1 )
    {
        n=5; // มาจาก PeopleCount()เดิม      
    }

//=================================================================================#######

   }
}

//
//void PeopleCount(){
//  if (count1 > lastcount1){
////    Serial.println("------------------------------------------------");
////    Serial.println("----+++++++++++++++---+++++++++--------++++++---");
////    Serial.println("----+++++++++++++++---++++++++++-------++++++---");
////    Serial.println("--------+++++++-------+++++++++++------++++++---");
////    Serial.println("--------+++++++-------++++++++++++-----++++++---");
////    Serial.println("--------+++++++-------++++++-++++++----++++++---");
////    Serial.println("--------+++++++-------++++++--++++++---++++++---");
////    Serial.println("--------+++++++-------++++++---++++++--++++++---");
////    Serial.println("--------+++++++-------++++++----++++++-++++++---");
////    Serial.println("--------+++++++-------++++++-----++++++++++++---");
////    Serial.println("----+++++++++++++++---++++++------+++++++++++---");
////    Serial.println("----+++++++++++++++---++++++-------++++++++++---");
////    Serial.println("------------------------------------------------");
//    Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++");
//    n=5;
//
//  }
////  else if (count2 < lastcount2){
////    Serial.println("-------------------------------------------------------------------");
////    Serial.println("-----++++++++++++-------++++++------++++++----+++++++++++++++++++--");
////    Serial.println("---++++++++++++++++-----++++++------++++++----+++++++++++++++++++--");
////    Serial.println("--++++++------++++++----++++++------++++++----+++++++++++++++++++--");
////    Serial.println("--++++++------++++++----++++++------++++++----------+++++++--------");
////    Serial.println("--++++++------++++++----++++++------++++++----------+++++++--------");
////    Serial.println("--++++++------++++++----++++++------++++++----------+++++++--------");
////    Serial.println("--++++++------++++++----++++++------++++++----------+++++++--------");
////    Serial.println("--++++++------++++++----++++++------++++++----------+++++++--------");
////    Serial.println("--++++++------++++++----++++++------++++++----------+++++++--------");
////    Serial.println("--+++++++++++++++++------++++++++++++++++-----------+++++++--------");
////    Serial.println("-----++++++++++++----------++++++++++++-------------+++++++--------");
////    Serial.println("-------------------------------------------------------------------");
////    n=3;
////  } 
////  if (count2 < 0)
////    count2 = 0;
//
//  lastcount1 = count1;
////  lastcount2 = count2;
////  Serial.println("--------------------------------------------");
////  Serial.print("People in the room: ");
////  Serial.println(count);
//Serial.print("Count In: ");
//Serial.println(count1);
////Serial.print("Count Out: ");
////Serial.println(count2);
//
////  Serial.println("--------------------------------------------");
//}

void Timein(String collection,int cou, String a){
 // int j;
    String strcount = String(cou);
  // *old*  String data = "Data/esp-02/IN/" + a;
   // String data = "Data/SH063/" + collection + " " + a;
   // ใช้ปกติ String data = "Data/SH040-2/" + a + " ";//******************************************************************************
    String data = "Data/SH000-5/" + a + " ";//******************************************************************************
    Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    Serial.println(a);
    Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//
  //++++++++++++++++++++++++++
  if(modecount==1){                                                               //offline
    countoff[ioff] = String(cou);
    off[ioff]=data;
    datacollection[ioff]=collection;
    ioff++;
  }
  if(modecount==2 && ioff != 0){                                                  //online
    for(j=0;j<ioff;j++){
     // Firebase.setString(firebaseData,off[j],countoff[j]);
      Firebase.setString(firebaseData,off[j],datacollection[j]);

    }
    ioff=0;
  }
  
 //++++++++++++++++++++++++++
   // Firebase.setString(firebaseData,data,strcount);
    Firebase.setString(firebaseData,data,collection);
}

////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void sendOffline(){
//  int j;
//
//  Serial.println("000000000000000000000000000000000000000000000000000000000000000000000000000000");
//  Serial.println(ioff);
//  if (ioff>0){
//    delay(5000);
//  }
//  for(j=0;j<ioff;j++){                                                        //online
//     // Firebase.setString(firebaseData,off[j],countoff[j]);
//      Firebase.setString(firebaseData,off[j],datacollection[j]);
//       Serial.print(j);
//       Serial.print("\t");
//       Serial.print(off[j]);
//       Serial.print("\t");
//       Serial.println(datacollection[j]);
//    }
//    ioff=0;
//}
////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//void Timeout(String collection,int cou, String a ){
//    String strcount = String(cou);
//    String data = "Data/SH062-out/" + collection + " " + a;
//    Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//    Serial.println(a);
//    Serial.println("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
////
//
//    
//  //  Firebase.setString(firebaseData,data,strcount);
//}
