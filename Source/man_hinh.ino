#include <SPI.h>
#include "FirebaseESP32.h"
#include <ArduinoJson.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "WiFi.h"
#include <Wire.h>
#include "Goodix.h"
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     4000
#define INT_PIN 2
#define RST_PIN 15
#define LILYGO_LILYPI false  
#define RA8875_CS 5
#define RA8875_RESET 4
#define BUZZER 26
volatile boolean heartBeatDetected = false;
#define FIREBASE_HOST "graduation-thesis-5d064-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "evfq0rEhnU4FawHHnf8JHCK0Qk3Hx3bdhZAru1Mc"
FirebaseData firebaseData;
String path = "/";
FirebaseJson json;
uint16_t Columns,Row;
uint16_t Min_Columns,Max_Columns,Min_Row,Max_Row;
char temp;
int flag_Display;
int flag_DT;
int flag_Delete;
int switch_session;
int update_Infor;
int flag_BP;
bool flag_Caps;
int flag_AddText;
int flag_Wifi;
int flag_Enter_Pass;
int Display_UP;
int save_pass;
int flag_Display_Pass;
int flag_connect;
int connected_Wifi;
int flag_bpnhap ;
int exit_for;
int update_thongtin = 0;
int flag_max30;
int flag_send;
int find_ssid;
int n;
int flag_do;
int save_ssid;
int flag_Display_ssid;
int flag_Enter_SSID;
int flag_level;
int dem;
int flag_get_;
int flag_enter_login;
int sellect_login;
int save_login;
int flag_Display_login;
int send_information;
char name_doctor[30];
char ID_patient[7];
String S_name_doctor;
String S_ID_patient;
bool Level_1;
bool Level_2;
bool Level_3;
bool Level_4;
int Level;
int Set_Level ;
char SSID_[30][20];
char SSID_Temp[20];
char password[20];
char buff2[15];
char buff3[30];
String buff4 ;
int16_t contact;
uint8_t rawdata[40]; 
float Spo2_ = 0, Bpm_ = 0, Spo2_Temp = 0, Bpm_Temp = 0;
char Spo2_String[] = "   ";
char Bpm_String[] = "";
char string[30] = "Information ";
char string0[30] = "Patient";
char string1[15] = "Name  : ";
char string2[15] = "Age   : ";
char string3[15] = "weight: ";
char string4[15] = "Sex   : ";
char string5[15] = "SPO2";
char string6[15] = "BPM";
char string7[30] = "Update Information";
char string8[15] = "Mode  :";
String ID_User ="181423";
char NamePatient[30] ;
char age[30];
char Weight[30];
char Sex[30];
String status_;
const char chu_In[] ="QWERTYUIOPASDFGHJKLZXCVBNM ,." ;
const char chu_Thuong[] ="qwertyuiopasdfghjklzxcvbnm ,." ;
const char so[] ="#d&_-123?@()=+456!*':%/789 ,.0" ;
char chu[30];
Goodix touch = Goodix();
PulseOximeter pox;
void onBeatDetected()
{
    Serial.println("Beat!");
    flag_do = 1;
}
uint32_t tsLastReport = 0;
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
void handleTouch(int8_t contacts, GTPoint* points) {
    Columns = points->x;
    Row = points->y;
}
void touchStart() {
  unsigned short configInfo;
  touch.begin(INT_PIN, RST_PIN, GOODIX_I2C_ADDR_BA);
  Wire.beginTransmission(touch.i2cAddr);
  configInfo = touch.configCheck(LILYGO_LILYPI);
}
int flag_dao;
int temp_cycle_pulse;
int flag_change;
int flag_soxung;
int flag_off;
volatile bool state = false;
volatile bool state_dir = false;
const int stepPin = 12; //PUL
const int dirPin = 14; // DIR
const int enPin = 27; //ena 14
volatile int cycle_pulse;
int flag_settimer;
volatile int Time_bop;
volatile int interruptCounter = 0;
hw_timer_t* timer = NULL; //khơi tạo timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; 
void IRAM_ATTR onTimer(){   
  portENTER_CRITICAL_ISR(&timerMux); //vào chế độ tránh xung đột
  if(flag_dao == 0)
  {
    if(dem == 1)
    {
      if(flag_off == 0)
      {
        temp_cycle_pulse = cycle_pulse;
      }
    }
    state = !state;
    digitalWrite(stepPin, state);
    interruptCounter++;
    if(interruptCounter/2 > temp_cycle_pulse){
      interruptCounter = 0;
      timerAlarmWrite(timer,Time_bop, true);
      timerAlarmEnable(timer);
      flag_dao = 1;
      state_dir = ! state_dir;
    }
  } 
  else
  {
    if(state_dir == LOW)
    {
      if(flag_change == 1)
      {
        temp_cycle_pulse = cycle_pulse;
        flag_change = 0;
        flag_off = 0;
      }
    }
    digitalWrite(dirPin, state_dir);
    flag_dao = 0; 
    timerAlarmWrite(timer,600, true);
    timerAlarmEnable(timer);
  }
  portEXIT_CRITICAL_ISR(&timerMux); // thoát 
}
void get_SSID(int index,uint16_t Min_Columns_, uint16_t Max_Columns_, uint16_t Min_Row_, uint16_t Max_Row_){
    if(Min_Columns_ < Columns && Columns < Max_Columns_ && Min_Row_ < Row && Row < Max_Row_)
    {
      flag_Display = 1;
      flag_Enter_Pass = 1;
      Min_Columns = 0;
      Max_Columns = 0;
      Min_Row = 0;
      Max_Row = 0;
      Columns = 0;
      Row = 0;
      copy(SSID_[index],SSID_Temp);
      tft.fillScreen(RA8875_BLACK);
    }
}
void enter_Pass(char SSID_[]){
   tft.fillRect(100,5,600,290,RA8875_WHITE);
   if(flag_connect == 0){
      tft.textEnlarge(2);
      tft.textTransparent(RA8875_BLACK);
      tft.textSetCursor(130,5);
      tft.textWrite(SSID_);
      tft.textSetCursor(130,45);
      tft.textWrite("Secure");
      tft.textSetCursor(130,105);
      tft.textWrite("Enter the network key");
      tft.drawRect(133,155,494,50,RA8875_BLACK);
      tft.drawRect(135,225,150,50,RA8875_BLACK);
      tft.textSetCursor(140,220);
      tft.textWrite("Cancel");
      tft.drawRect(475,225,170,50,RA8875_BLACK);
      tft.textSetCursor(480,220);
      tft.textWrite("Connect");
      tft.textEnlarge(2);
      tft.textTransparent(RA8875_BLACK);
      tft.textSetCursor(137,157);
      tft.textWrite(password);
   }
   else
   {
      int i;
      tft.textEnlarge(2);
      tft.textTransparent(RA8875_BLACK);
      tft.textTransparent(RA8875_BLACK);
      tft.textSetCursor(130,5);
      tft.textWrite(SSID_);
      tft.textSetCursor(130,45);
      tft.textWrite("Secure");
      tft.textSetCursor(130,100);
      tft.textWrite("Connecting.......");
      tft.drawRect(130,220,150,50,RA8875_BLACK);
      tft.textSetCursor(133,220);
      tft.textWrite("Cancel");
      WiFi.begin(SSID_Temp,password);
      for(int a = 0; a<20; a++){
        delay(500);
        if(WiFi.status() == WL_CONNECTED)
        {
          i = 1;
          break;
        } 
        else
          i = 0; 
      }
      xoa_toan(password);
      tft.textEnlarge(2);
      tft.textColor(RA8875_BLACK, RA8875_WHITE);
      tft.textSetCursor(153,150);
      tft.textWrite("                   ");
      if(i == 0) 
      {
        tft.textSetCursor(133,150);
        tft.textWrite("Connect Failed");
        connected_Wifi = 0;
      }
      else if(i == 1)
      {
        tft.textSetCursor(133,150);
        tft.textWrite("Connect success");
        connected_Wifi = 1;
      }
      flag_connect = 0;
   }
}
void connect_Wifi(){
  if(flag_Wifi == 1){
    if(connected_Wifi == 0){
      for (int i = 0; i < n; ++i) {
        Serial.println("dang thu hien");
        set_cursor(610,790,(i+1)*58,(i+1)*58+50);
        get_SSID(i,Min_Columns,Max_Columns,Min_Row,Max_Row);
        
        if(exit_for == 1) 
        {
            exit_for = 0;
            break;
        }
      }
    }
    else
    {
      if(550 < Columns && Columns < 800 && 58 < Row && Row < 108)
      {
        flag_Display = 1;
        connected_Wifi = 0;
        Columns = 0;
        Row = 0;
        xoa_toan(password);
        tft.fillScreen(RA8875_BLACK);
      }
    }
  }
}
void wifi_(){
    if(flag_Enter_Pass == 0){
      if(flag_Enter_SSID == 1)
      {
        if(flag_BP == 0)
        {
          tft.fillRect(100,5,600,290,RA8875_WHITE);
          tft.textEnlarge(2);
          tft.textTransparent(RA8875_BLACK);
          tft.textSetCursor(130,5);
          tft.textWrite("Add network");
          tft.textSetCursor(130,45);
          tft.textWrite("Secure");
          tft.textSetCursor(130,105);
          tft.textWrite("Enter network name");
          tft.drawRect(133,155,494,50,RA8875_BLACK);
          tft.drawRect(135,225,150,50,RA8875_BLACK);
          tft.textSetCursor(140,220);
          tft.textWrite("Cancel");
          tft.drawRect(475,225,170,50,RA8875_BLACK);
          tft.textSetCursor(480,220);
          tft.textWrite("Connect");
          tft.textEnlarge(2);
          tft.textTransparent(RA8875_BLACK);
          tft.textSetCursor(137,157);
          tft.textWrite(SSID_Temp);
        }
        else
        {
          if(flag_AddText == 1)
          {
            if(save_ssid == 1)
            {
                if(flag_Delete == 1)
                {
                    xoa(SSID_Temp);
                    flag_Delete = 0;
                }  
                else
                    them(SSID_Temp,temp);
            }
            flag_AddText = 0;
          }
          if(flag_bpnhap == 1)
          {
            flag_bpnhap = 0;
            flag_get_ = 0;
            BAN_PHIM();
          }
          if(flag_Display_ssid == 1)
          {
            tft.fillRect(100,5,600,290,RA8875_WHITE);
            tft.textEnlarge(2);
            tft.textTransparent(RA8875_BLACK);
            tft.textSetCursor(130,5);
            tft.textWrite("Add network");
            tft.textSetCursor(130,45);
            tft.textWrite("Secure");
            tft.textSetCursor(130,105);
            tft.textWrite("Enter network name");
            tft.drawRect(133,155,494,50,RA8875_BLACK);
            tft.drawRect(135,225,150,50,RA8875_BLACK);
            tft.textSetCursor(140,220);
            tft.textWrite("Cancel");
            tft.drawRect(475,225,170,50,RA8875_BLACK);
            tft.textSetCursor(480,220);
            tft.textWrite("Connect");
            tft.textEnlarge(2);
            tft.textTransparent(RA8875_BLACK);
            tft.textSetCursor(137,157);
            tft.textWrite(SSID_Temp);
            flag_Display_ssid = 0;
          }
          tft.textEnlarge(2);
          tft.textColor(RA8875_BLACK, RA8875_WHITE);
          tft.textSetCursor(137,156);
          tft.textWrite("                  ");
          tft.textTransparent(RA8875_BLACK);
          tft.textSetCursor(137,157);
          tft.textWrite(SSID_Temp);
          tft.textWrite("_");
        }
      }
      else
      {
        tft.textEnlarge(2);
        tft.textTransparent(RA8875_WHITE);
        tft.textSetCursor(0,0);
        tft.textWrite("<- WI-FI");
        tft.drawRect(695,0,100,40,RA8875_WHITE);
        if(flag_Wifi == 1){
          tft.drawRect(460,0,230,40,RA8875_WHITE);
          tft.textEnlarge(1);
          tft.textSetCursor(462,0);
          tft.textWrite("+ Add network");
          tft.textEnlarge(2);
          tft.fillRect(697,2,96,36,RA8875_BLUE);
          tft.fillRect(745,2,48,36,RA8875_WHITE);
          if(connected_Wifi == 0)
          {
            WiFi.disconnect();
            n = WiFi.scanNetworks();
            if(n == 0)
            {
              tft.textSetCursor(50,100);
              tft.textWrite("Not found WI-FI.");
            }
            else if(n>7) n = 7;
              for (int i = 0; i < n; ++i) {
                tft.drawRect(610,(i+1)*58,180,50,RA8875_WHITE);
                tft.textSetCursor(615,(i+1)*58 + 1 );
                tft.textWrite("Connect");
                tft.textSetCursor(10, (i+1)*58);
                tft.textColor(RA8875_WHITE, RA8875_BLACK);
                tft.textWrite("                       ");
                tft.textSetCursor(10, (i+1)*58);
                sprintf(buff2,"%.0d",(i+1));
                tft.textWrite(buff2);
                tft.textWrite(": ");
                buff4 = WiFi.SSID(i);
                buff4.toCharArray(buff3,(buff4.length()+1));
                copy(buff3,SSID_[i]);
                xoa_toan(buff3);
                tft.textWrite(SSID_[i]);
                delay(100);
              }  
          }
          else
          {
                tft.textSetCursor(10, 58);
                tft.textWrite(SSID_Temp);
                tft.drawRect(550,58,250,50,RA8875_WHITE);
                tft.textSetCursor(555,59);
                tft.textWrite("Disconnect");
          }
        }
        else{
          tft.fillRect(697,2,96,36,RA8875_BLACK);
          tft.fillRect(697,2,48,36,RA8875_WHITE);
          tft.textSetCursor(50,100);
          tft.textWrite("WI-FI is off.");
          tft.textSetCursor(50,145);
          tft.textWrite("Turn on WI-FI to connect");
          tft.textSetCursor(50,190);
          tft.textWrite("to an available WI-FI network.");
        }
      }
  } 
  else{
        if(flag_BP == 1)
        {
          if(flag_AddText == 1)
          {
            if(save_pass == 1)
            {
                if(flag_Delete == 1)
                {
                    xoa(password);
                    flag_Delete = 0;
                }  
                else
                    them(password,temp);
            }
            flag_AddText = 0;
          }
          if(flag_bpnhap == 1)
          {
            flag_bpnhap = 0;
            flag_get_ = 0;
            BAN_PHIM();
          }
          if(flag_Display_Pass == 1)
          {
            enter_Pass(SSID_Temp);
            flag_Display_Pass = 0;
          }
          tft.textEnlarge(2);
          tft.textColor(RA8875_BLACK, RA8875_WHITE);
          tft.textSetCursor(137,156);
          tft.textWrite("                  ");
          tft.textTransparent(RA8875_BLACK);
          tft.textSetCursor(137,157);
          tft.textWrite(password);
          tft.textWrite("_");
        }
        else
        {
          enter_Pass(SSID_Temp);
        }
  }
}

void get_sensor(){
      pox.update();
      Spo2_ =  pox.getSpO2();
      Bpm_ = pox.getHeartRate();
}
void display_sensor(){
      pox.update();
      if(Spo2_ > 100) 
          Spo2_ = 99;
      if(Bpm_ > 110) 
          Bpm_ = 110;
      sprintf(Spo2_String,"%.0f",Spo2_);
      sprintf(Bpm_String,"%.0f",Bpm_);
      if(Spo2_ == 0)
        tft.fillRect(601,(400-(Spo2_Temp*3)-3),78,((Spo2_Temp*3)),RA8875_BLACK);
      else{
        tft.fillRect(601,(400-(Spo2_Temp*3)-3),78,((Spo2_Temp*3)),RA8875_BLACK);
        tft.fillRect(601,(400-(Spo2_*3)-3),78,((Spo2_*3)),RA8875_BLUE);        
      } 
      Spo2_Temp = Spo2_;
      tft.textEnlarge(1);
      tft.textColor(RA8875_WHITE, RA8875_BLUE);
      tft.textSetCursor(610, 200);
      tft.textWrite("    ");
      tft.textTransparent(RA8875_RED);
      tft.textSetCursor(610, 200);
      tft.textWrite(Spo2_String);
      tft.textWrite("%"); 
      if(Bpm_ == 0)
        tft.fillRect(701,(400-(Bpm_Temp*2.7)-3),78,((Bpm_Temp*2.7)),RA8875_BLACK);
      else{
        tft.fillRect(701,(400-(Bpm_Temp*2.7)-3),78,((Bpm_Temp*2.7)),RA8875_BLACK);
        tft.fillRect(701,(400-(Bpm_*2.7)-3),78,((Bpm_*2.7)),RA8875_BLUE);
      }
      Bpm_Temp = Bpm_;
      tft.textColor(RA8875_WHITE, RA8875_BLUE);
      tft.textSetCursor(710, 200);
      tft.textWrite("    ");
      tft.textTransparent(RA8875_RED);
      tft.textSetCursor(710, 200);
      tft.textWrite(Bpm_String);
      SendData();
      pox.update();
}
void change_Infor()
{
  pox.update();
  tft.textEnlarge(2);
  if(update_Infor == 1)
  {
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textSetCursor(185, 96);
    tft.textWrite("                    ");
    tft.textTransparent(RA8875_WHITE);
    tft.textSetCursor(185, 96);
    tft.textWrite(NamePatient);
  } 
  else if(update_Infor == 2)
  {
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textSetCursor(185, 148);
    tft.textWrite("                    ");
    tft.textTransparent(RA8875_WHITE);
    tft.textSetCursor(185, 148);
    tft.textWrite(age);
  }
  else if(update_Infor == 3)
  {
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textSetCursor(185, 200);
    tft.textWrite("                    ");
    tft.textTransparent(RA8875_WHITE);
    tft.textSetCursor(185, 200);
    tft.textWrite(Weight);
  }
  else if(update_Infor == 4)
  {
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textSetCursor(185, 252);
    tft.textWrite("                    ");
    tft.textTransparent(RA8875_WHITE);
    tft.textSetCursor(185, 252);
    tft.textWrite(Sex);
  }
  tft.textWrite("_");
  pox.update();
}
void Infor_Patient(){
  pox.update();
  tft.textEnlarge(2);
  tft.textSetCursor(180, 95);
  tft.textWrite(NamePatient);
  tft.textSetCursor(180, 140);
  tft.textWrite(age);
  tft.textSetCursor(180, 185);
  tft.textWrite(Weight);
  tft.textSetCursor(180, 230);
  tft.textWrite(Sex);
  pox.update();
}
void log_in()
{
  tft.textEnlarge(1);
  tft.textColor(RA8875_WHITE, RA8875_WHITE);
  tft.textSetCursor(135, 82);
  tft.textWrite("                            ");
  tft.textSetCursor(135, 172);
  tft.textWrite("                            ");
  tft.textTransparent(RA8875_BLACK);
  tft.textSetCursor(135, 82);
  tft.textWrite(name_doctor);
  tft.textSetCursor(135, 172);
  tft.textWrite(ID_patient);
  tft.fillRect(100,5,600,290,RA8875_WHITE);
  tft.textEnlarge(2);
  tft.textTransparent(RA8875_BLACK);
  tft.textSetCursor(250,5);
  tft.textWrite("LOG IN ACCOUNT");
  tft.textSetCursor(130,45);
  tft.textEnlarge(1);
  tft.textWrite("Enter name doctor");
  tft.drawRect(133,80,494,40,RA8875_BLACK);
  tft.textSetCursor(130,135);
  tft.textWrite("Enter ID patient");
  tft.textTransparent(RA8875_BLACK);
  tft.textSetCursor(135, 82);
  tft.textWrite(name_doctor);
  tft.textSetCursor(135, 172);
  tft.textWrite(ID_patient);
  tft.drawRect(133,170,494,40,RA8875_BLACK);
  tft.drawRect(315,230,170,40,RA8875_BLACK);
  tft.textSetCursor(340,225);
  tft.textEnlarge(2);
  tft.textWrite("LOGIN");
}
void enter_login()
{
    //pox.update();
    if(flag_BP == 0)
    {
      log_in();
    }
    else
    {
      if(flag_AddText == 1)
      {
        if(sellect_login == 1)
        {
          if(save_login == 1)
          {
              if(flag_Delete == 1)
              {
                  xoa(name_doctor);
                  flag_Delete = 0;
              }  
              else
                  them(name_doctor,temp);
          }
        }
        else if(sellect_login == 2)
        {
          if(save_login == 1)
          {
              if(flag_Delete == 1)
              {
                  xoa(ID_patient);
                  flag_Delete = 0;
              }  
              else
                  them(ID_patient,temp);
          }
        }
        flag_AddText = 0;
      }
      if(flag_bpnhap == 1)
      {
        flag_bpnhap = 0;
        flag_get_ = 0;
        BAN_PHIM();
      }
      if(flag_Display_login == 1)
      {
        log_in();
        flag_Display_login = 0;
      }
      tft.textEnlarge(1);
      tft.textColor(RA8875_WHITE, RA8875_WHITE);
      tft.textSetCursor(135, 82);
      tft.textWrite("                            ");
      tft.textSetCursor(135, 172);
      tft.textWrite("                            ");
      if(sellect_login == 1)
      {
      tft.textTransparent(RA8875_BLACK);
      tft.textSetCursor(135, 82);
      tft.textWrite(name_doctor);
      tft.textWrite("_");
      tft.textSetCursor(135, 172);
      tft.textWrite(ID_patient);
      } else if(sellect_login == 2)
      {
      tft.textTransparent(RA8875_BLACK);
      tft.textSetCursor(135, 82);
      tft.textWrite(name_doctor);
      tft.textSetCursor(135, 172);
      tft.textWrite(ID_patient);
      tft.textWrite("_");
      }
   }
}
void GIAO_DIEN_1(){
  pox.update();
  tft.textSetCursor(1, 1);
  tft.fillRect(0,20,4,10,RA8875_WHITE);
  tft.fillRect(6,18,4,12,RA8875_WHITE);
  tft.fillRect(12,16,4,14,RA8875_WHITE);
  tft.fillRect(18,14,4,16,RA8875_WHITE);
  tft.fillRect(24,12,4,18,RA8875_WHITE);
  if(connected_Wifi == 1)
  {
    pox.update();
    tft.textEnlarge(0);
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textSetCursor(30, 15);
    tft.textWrite("                            ");
    tft.textEnlarge(0);
    tft.textTransparent(RA8875_WHITE);
    tft.textSetCursor(30, 15);
    tft.textWrite(SSID_Temp);
    pox.update();
  }
  else
  {
    pox.update();
    tft.textEnlarge(0);
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textSetCursor(30, 15);
    tft.textWrite("                            ");
    tft.textSetCursor(30, 15);
    tft.textWrite("no connect");
    pox.update();
  }
}
void Speed_Mode(int x, int y, int h, int w,int level,const char a[]){
  pox.update();
  tft.drawRect(x,y,h,w,RA8875_WHITE);
  if(level !=0)
    tft.fillRect(x+2,y+2,h-4,w-4,RA8875_BLUE); 
  else 
    tft.fillRect(x+2,y+2,h-4,w-4,RA8875_BLACK); 
  tft.textSetCursor(x+5,y+2);
  tft.textTransparent(RA8875_WHITE);
  tft.textEnlarge(2);
  tft.textWrite(a);
  pox.update();
}
void GIAO_DIEN_LEVEL()
{
  pox.update();
  Speed_Mode(180,295,175,50,Level_1,"Level 1");
  Speed_Mode(380,295,175,50,Level_2,"Level 2");
  Speed_Mode(180,355,175,50,Level_3,"Level 3");
  Speed_Mode(380,355,175,50,Level_4,"Level 4");
}
void GIAO_DIEN_2(){
  pox.update();
  GIAO_DIEN_1();  
  tft.textSetCursor(5, 45);
  tft.textTransparent(RA8875_WHITE);
  tft.textEnlarge(2);
  tft.textWrite(string);
  tft.textWrite(string0);
  tft.textSetCursor(5, 95);
  tft.textWrite(string1);
  tft.textSetCursor(5, 140);
  tft.textWrite(string2);
  tft.textSetCursor(5, 185);
  tft.textWrite(string3);
  tft.textSetCursor(5, 230);
  tft.textWrite(string4);
  tft.textSetCursor(5, 280);
  tft.textWrite(string8);
  tft.textSetCursor(590, 50);
  tft.textWrite(string5);
  pox.update();
  GIAO_DIEN_LEVEL();
  pox.update();
  tft.drawRect(600,98,80,302,RA8875_WHITE);
  tft.textSetCursor(703, 50);
  tft.textWrite(string6);
  tft.drawRect(700,98,80,302,RA8875_WHITE);  
  tft.drawRect(10,425,450,50,RA8875_WHITE);
  tft.textSetCursor(20, 420);
  tft.textWrite(string7);
  tft.drawRect(625,425,175,50,RA8875_WHITE);  
  tft.textSetCursor(630, 420);
  tft.textWrite("Logout");
  Infor_Patient();
  pox.update();
}
void GIAO_DIEN_3(){
  pox.update();
  GIAO_DIEN_1();
  tft.textSetCursor(5, 45);
  tft.textTransparent(RA8875_WHITE);
  tft.textEnlarge(2);
  tft.textWrite(string7);
  tft.textSetCursor(5, 95);
  tft.textWrite(string1);
  tft.textWrite("...");
  tft.drawRect(180,95,620,50,RA8875_WHITE);
  tft.textSetCursor(5, 147);
  tft.textWrite(string2);
  tft.textWrite("...");
  tft.drawRect(180,147,620,50,RA8875_WHITE);
  tft.textSetCursor(5, 199);
  tft.textWrite(string3);
  tft.textWrite("...");
  pox.update();
  tft.drawRect(180,199,620,50,RA8875_WHITE);
  tft.textSetCursor(5, 251);
  tft.textWrite(string4);
  tft.textWrite("...");
  tft.drawRect(180,251,620,50,RA8875_WHITE);
  tft.drawRect(650,400,120,50,RA8875_WHITE);
  tft.textSetCursor(660, 398);
  tft.textWrite("SAVE");
  tft.drawRect(10,400,120,50,RA8875_WHITE);
  tft.textSetCursor(20, 398);
  tft.textWrite("BACK");
  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(185, 95);
  tft.textWrite(NamePatient);
  tft.textSetCursor(185, 147);
  tft.textWrite(age);
  tft.textSetCursor(185, 199);
  tft.textWrite(Weight);
  tft.textSetCursor(185, 251);
  tft.textWrite(Sex);
  pox.update();
}
void copy(const char a[],char b[]){
  int leng = strlen(a);
  for(int i = 0; i<leng; i++){
    b[i] = a[i];
  }
  b[leng] = '\0';
}
void phim(int x,int y,char string8){
  tft.drawRect(x,y,80,40,RA8875_WHITE);
  tft.textSetCursor((x+30), y);
  tft.textTransparent(RA8875_WHITE);
  tft.textEnlarge(1);
  tft.textWrite(&string8);
}
void phim2(int x,int y,char string8){
  tft.drawRect(x,y,120,40,RA8875_WHITE);
  tft.textSetCursor((x+50), y);
  tft.textTransparent(RA8875_WHITE);
  tft.textEnlarge(1);
  tft.textWrite(&string8);
}
void phim3(int x,int y,char string8){
  tft.drawRect(x,y,50,40,RA8875_WHITE);
  tft.textSetCursor((x+17), y);
  tft.textTransparent(RA8875_WHITE);
  tft.textEnlarge(1);
  tft.textWrite(&string8);
}
void BAN_PHIM()
{
  if(flag_DT == 0)
  {
    if(flag_Caps == 0)
    {
      copy(chu_Thuong,chu);
      tft.drawTriangle(40,412,33,420,48,420,RA8875_WHITE);
      tft.drawRect(35,420,10,8,RA8875_WHITE);
      tft.drawLine(35,420,43,420,RA8875_BLACK);      
    }
    else 
    {
      copy(chu_In,chu);
      tft.fillTriangle(40,412,33,420,48,420,RA8875_WHITE);
      tft.fillRect(35,420,10,8,RA8875_WHITE);
    }
    for(int i = 0; i<26;i++){
    if(i<10)
      phim((i*80),320,chu[i]);
    else if(i<19)
      phim((40+(i-10)*80),360,chu[i]);
    else
      phim((120+(i-19)*80),400,chu[i]);
    }
    tft.textSetCursor(156, 448);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite(",");
    tft.textSetCursor(636, 448);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite(".");
    tft.drawRect(200,440,400,40,RA8875_WHITE);
    tft.textSetCursor(15, 440);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite("123");
    tft.textSetCursor(680, 445);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite("ENTER");
    tft.drawTriangle(744,415,734,423,744,430,RA8875_WHITE);
    tft.drawRect(744,415,16,16,RA8875_WHITE);
    tft.drawLine(744,416,744,429,RA8875_BLACK);
    tft.drawLine(746,417,756,427,RA8875_WHITE);
    tft.drawLine(746,427,756,417,RA8875_WHITE);
    tft.drawLine(35,430,44,430,RA8875_WHITE);
  }
  else if(flag_DT==1) 
  {
    for(int i = 0; i<26;i++)
    {
      if(i==0)
        phim2(i,320,so[i]);
      else if(0<i && i<5)
        phim3((120+(i-1)*50),320,so[i]);
      else if(4<i && i<9){
        phim2((320+(i-5)*120),320,so[i]);
      }
      else if(i==9)
        phim2((i-9),360,so[i]);
      else if(9<i && i<14)
        phim3((120+(i-10)*50),360,so[i]);
      else if(13<i && i<18){
        phim2((320+(i-14)*120),360,so[i]);
      }
      else if(i==18)
        phim2((i-18),400,so[i]);
      else if(18<i && i<23)
        phim3((120+(i-19)*50),400,so[i]);
      else if(22<i && i<26){
        phim2((320+(i-23)*120),400,so[i]);
      }
    }
    tft.textSetCursor(156, 448);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite(",");
    tft.textSetCursor(610, 448);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite(".");
    tft.drawRect(220,440,220,40,RA8875_WHITE);
    tft.drawRect(440,440,120,40,RA8875_WHITE);
    tft.textSetCursor(490,440);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite("0");
    tft.textSetCursor(35, 440);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite("abc");
    tft.textSetCursor(680, 445);
    tft.textTransparent(RA8875_WHITE);
    tft.textEnlarge(1);
    tft.textWrite("ENTER");
    tft.drawTriangle(744,415,734,423,744,430,RA8875_WHITE);
    tft.drawRect(744,415,16,16,RA8875_WHITE);
    tft.drawLine(744,416,744,429,RA8875_BLACK);
    tft.drawLine(746,417,756,427,RA8875_WHITE);
    tft.drawLine(746,427,756,417,RA8875_WHITE);
  }
}
void get_Text(int index,uint16_t Min_Columns_, uint16_t Max_Columns_, uint16_t Min_Row_, uint16_t Max_Row_){
    if(Min_Columns_ < Columns && Columns < Max_Columns_ && Min_Row_ < Row && Row < Max_Row_)
    {
      flag_Display = 1;
      flag_AddText = 1;
      Min_Columns = 0;
      Max_Columns = 0;
      Min_Row = 0;
      Max_Row = 0;
      Columns = 0;
      Row = 0;
      if(flag_DT == 0)
        temp = chu[index];
      else 
        temp = so[index];
      exit_for = 1;
    }
}
void set_cursor(uint16_t Min_Columns_, uint16_t Max_Columns_, uint16_t Min_Row_, uint16_t Max_Row_){
    Min_Columns = Min_Columns_;
    Max_Columns = Max_Columns_;
    Min_Row = Min_Row_;
    Max_Row = Max_Row_;
}
void nhan(){
  if(flag_DT==0)//ban phim chu
  {
    if(flag_Caps == 0)
        copy(chu_Thuong,chu);     
    else 
        copy(chu_In,chu);
    for(int i = 0; i<29;i++)
    {
        if(0<Columns && Columns<120 && 440<Row && Row<480)
        {
            flag_bpnhap = 1;
            flag_get_ = 1;
            flag_Display = 1;
            flag_DT = 1;
            Columns = 0;
            Row = 0;
            tft.fillRect(0,319,800,161,RA8875_BLACK);
            break;
        }
        if(680<Columns && Columns<800 && 400<Row && Row<440)
        {
            flag_Display = 1;
            flag_Delete = 1;
            flag_AddText = 1;
            Columns = 0;
            Row = 0;
            break;
        }
        if(680<Columns && Columns<800 && 440<Row && Row<480)
        {
            flag_Display = 1;
            flag_BP = 0;
            flag_DT = 0;
            flag_Caps = 0;
            update_Infor = 0;
            Columns = 0;
            Row = 0;
            tft.fillScreen(RA8875_BLACK);
            break;
        }
        if(0<Columns && Columns<120 && 400<Row && Row<440)
        {
            flag_bpnhap = 1;
            flag_get_ = 1;
            flag_Display = 1;
            flag_Caps =! flag_Caps;
            Columns = 0;
            Row = 0;
            tft.fillRect(0,319,800,161,RA8875_BLACK);
            break;
        }
        if(i == 26) 
            set_cursor(200,600,440,480);
        else if(i == 27)
            set_cursor(120,200,440,480);
        else if(i == 28)
            set_cursor(600,680,440,480); 
        else if(i < 10)
            set_cursor(i*80,80*(i+1),320,360);        
        else if(i < 19)
            set_cursor(40+(i-10)*80,40+80*((i-10)+1),360,400); 
        else if(i < 26)
            set_cursor(120+(i-19)*80,120+80*((i-19)+1),400,440); 
        get_Text(i,Min_Columns,Max_Columns,Min_Row,Max_Row);
        if(exit_for == 1) 
        {
            exit_for = 0;
            break;
        }
    }
  } 
  else if(flag_DT==1)//ban phim so
  {
    for(int i = 0; i<30;i++)
    {
        if(0<Columns && Columns<120 && 440<Row && Row<480)
        {
            flag_Display = 1;
            flag_bpnhap = 1;
            flag_get_ = 1;
            flag_DT = 0;
            Columns = 0;
            Row = 0;
            tft.fillRect(0,319,800,161,RA8875_BLACK);
            break;
        }
        if(680<Columns && Columns<800 && 400<Row && Row<440)
        {
            flag_Display = 1;
            flag_Delete = 1;
            flag_AddText = 1;
            Columns = 0;
            Row = 0;
            break;
        }
        if(680<Columns && Columns<800 && 440<Row && Row<480)
        {
            flag_Display = 1;
            flag_BP = 0;
            flag_DT = 0;
            flag_Caps = 0;
            update_Infor = 0;
            Columns = 0;
            Row = 0;
            tft.fillScreen(RA8875_BLACK);
            break;
        }
        if(i == 26)
            set_cursor(220,440,440,480);
        else if(i == 27)
            set_cursor(120,220,440,480);
        else if(i == 28)
            set_cursor(560,680,440,480);
        else if(i == 29)
            set_cursor(440,560,440,480);
        else if(i == 0)
            set_cursor(0,120,320,360);
        else if(0<i && i<5)
            set_cursor(120+(i-1)*50,170+(i-1)*50,320,360);
        else if(4<i && i<9)
            set_cursor(320+(i-5)*120,440+(i-5)*120,320,360);
        else if(i==9)
            set_cursor(0,120,360,400);
        else if(9<i && i<14)
            set_cursor(120+(i-10)*50,170+(i-10)*50,360,400);
        else if(13<i && i<18)
            set_cursor(320+(i-14)*120,440+(i-14)*120,360,400);
        else if(i==18)
            set_cursor(0,120,400,440);
        else if(18<i && i<23)
            set_cursor(120+(i-19)*50,170+(i-19)*50,400,440);
        else if(22<i && i<26)
            set_cursor(320+(i-23)*120,440+(i-23)*120,400,440);
        else if(i == 29)
            set_cursor(440,440,440,480);
        get_Text(i,Min_Columns,Max_Columns,Min_Row,Max_Row);
        if(exit_for == 1) 
        {
            exit_for = 0;
            break;
        }
    } 
  }
}
void seclect_level(uint16_t Min_Columns_, uint16_t Max_Columns_, uint16_t Min_Row_, uint16_t Max_Row_,bool* level_, int mode_){
    if(Min_Columns_ < Columns && Columns < Max_Columns_ && Min_Row_ < Row && Row < Max_Row_)
    {
      pox.update();
      flag_settimer = 1;
      flag_Display = 1;
      flag_change = 1;
      Min_Columns = 0;
      Max_Columns = 0;
      Min_Row = 0;
      Max_Row = 0;
      Columns = 0;
      Row = 0;
      flag_level = 1;
      if(mode_ == 1)
      {
        Level_2 = 0;
        Level_3 = 0;
        Level_4 = 0;
      }else if(mode_ == 2){
        Level_1 = 0;
        Level_3 = 0;
        Level_4 = 0;
      }else if(mode_ == 3){
        Level_1 = 0;
        Level_2 = 0;
        Level_4 = 0;
      }else if(mode_ == 4){
        Level_1 = 0;
        Level_2 = 0;
        Level_3 = 0;
      }
      (*level_) =! (*level_);
      if((*level_ == true))
      {
        Set_Level = mode_;
        dem ++;
      }
      else
      {
        Set_Level = 0;
        dem = 0;
        timerWrite(timer, 0);
        timerAlarmDisable(timer);
        flag_off = 1;
      }
      pox.update();
    }
}
void Switch_Session(){
    if(contact != 1){
      if(Columns !=0 &&  Row != 0)
      {
        if(switch_session == 0)
        {
            if(133<Columns && Columns<627 && 80<Row && Row<120)
            {
                flag_BP = 1;
                save_login = 1;
                sellect_login = 1;
                flag_Display = 1;
                flag_Display_login = 1;
                flag_bpnhap = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else if(133<Columns && Columns<627 && 170<Row && Row<210)
            {
                flag_BP = 1;
                save_login = 1;
                sellect_login = 2;
                flag_Display = 1;
                flag_Display_login = 1;
                flag_bpnhap = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else if(315<Columns && Columns<485 && 230<Row && Row<270)
            {
                Serial.println("login");
                S_ID_patient = String(ID_patient);
                S_name_doctor = String(name_doctor);
                switch_session = 1;
                flag_DT = 0;
                flag_Caps = 0;
                flag_Display = 1;
                flag_bpnhap = 0;
                flag_get_ = 0;
                flag_BP = 0;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
                Serial.println(S_ID_patient);
            }
        }
        else if(switch_session == 1)
        {
            if(10<Columns && Columns<460 && 425<Row && Row<475)
            {
                pox.shutdown();
                switch_session = 3;
//                update_thongtin = 1; //session update info
                flag_Display = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else if(0<Columns && Columns<28 && 0<Row && Row<30)
            {
                pox.shutdown();
                switch_session = 2; //session WIFI
                flag_Display = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            } 
            else if(625<Columns && Columns<800 && 425<Row && Row<475)
            {
                switch_session = 0; //session WIFI
                flag_Display = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else
            {
              seclect_level(180,355,295,345,&Level_1,1);
              seclect_level(380,555,295,345,&Level_2,2);
              seclect_level(180,355,355,405,&Level_3,3);
              seclect_level(380,555,355,405,&Level_4,4);
            }
        }
        else if(switch_session == 2){
          if(flag_Enter_Pass == 0)
          {
            if(flag_Enter_SSID == 0)
            {
              if(0<Columns && Columns<70 && 0<Row && Row<60)
              {
                  pox.resume(); 
                  pox.update();
                  switch_session = 1;
                  flag_Display = 1;
                  Columns = 0;
                  Row = 0;
                  tft.fillScreen(RA8875_BLACK);
              }
              else if(695<Columns && Columns<795 && 0<Row && Row<40)
              { 
                  flag_Display = 1;
                  flag_Wifi =! flag_Wifi;
                  Columns = 0;
                  Row = 0;
                  tft.fillScreen(RA8875_BLACK);
              }
              if(flag_Wifi == 1)
              {
                if(460<Columns && Columns<690 && 0<Row && Row<40)
                {
                  flag_Display = 1;
                  flag_Enter_SSID = 1;
                  find_ssid = 1;
                  Columns = 0;
                  Row = 0;
                  tft.fillScreen(RA8875_BLACK);
                }
                connect_Wifi();
              }
            }
            else
            {
              if(130<Columns && Columns<280 && 220<Row && Row<270)
              {
                  Serial.println("cancle ssid");
                  flag_DT = 0;
                  flag_Caps = 0;
                  xoa_toan(SSID_Temp);
                  flag_Enter_SSID = 0;
                  flag_Display = 1;
                  flag_bpnhap = 0;
                  flag_BP = 0;
                  Columns = 0;
                  Row = 0;
                  tft.fillScreen(RA8875_BLACK);
              }
              if(153<Columns && Columns<647 && 150<Row && Row<190)
              {
                  flag_BP = 1;
                  save_ssid = 1;
                  flag_Display = 1;
                  flag_Display_ssid = 1;
                  flag_bpnhap = 1;
                  Columns = 0;
                  Row = 0;
                  tft.fillScreen(RA8875_BLACK);
              }
              if(495<Columns && Columns<645 && 220<Row && Row<260)
              {
                  Serial.println("connect ssid");
                  flag_DT = 0;
                  flag_Caps = 0;
                  flag_Display = 1;
                  flag_Enter_Pass = 1;
                  flag_Enter_SSID = 0;
                  flag_bpnhap = 0;
                  flag_BP = 0;
                  Columns = 0;
                  Row = 0;
                  tft.fillScreen(RA8875_BLACK);
              }
            }
          }
          else {
            if(130<Columns && Columns<280 && 220<Row && Row<270)
            {
                Serial.println("cancle pass");
                flag_DT = 0;
                flag_Caps = 0;
                xoa_toan(password);
                flag_bpnhap = 0;
                flag_BP = 0;
                flag_Enter_SSID = 0;
                flag_Enter_Pass = 0;
                flag_connect = 0;
                flag_Display = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            if(153<Columns && Columns<647 && 150<Row && Row<190)
            {
                flag_BP = 1;
                save_pass = 1;
                flag_Display = 1;
                flag_Display_Pass = 1;
                flag_bpnhap = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            if(495<Columns && Columns<645 && 220<Row && Row<260)
            {
                Serial.println("connnect pass");
                flag_DT = 0;
                flag_Caps = 0;
                int leng = strlen(password);
                if(leng >= 8)
                {
                  flag_connect = 1;
                  flag_Display = 1;
                  flag_bpnhap = 0;
                  flag_BP = 0;
                  Columns = 0;
                  Row = 0;  
                  tft.fillScreen(RA8875_BLACK);
                }
                
            }
          }
        }                   
        else if(switch_session == 3)
        {
          if(flag_BP == 0)
          {
            if(10<Columns && Columns<130 && 400<Row && Row<450)
            {
                pox.resume(); 
                pox.update();
                switch_session = 1; //session update info
                flag_Display = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            if(650<Columns && Columns<770 && 400<Row && Row<450)
            {
                send_information = 1;
                flag_Display = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
          }
            if(180<Columns && Columns<800 && 95<Row && Row<145)
            {
                update_Infor = 1; //name
                flag_bpnhap = 1;
                flag_DT = 0;
                flag_Caps = 0;
                flag_Display = 1;
                flag_BP = 1;
                Display_UP = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else if(180<Columns && Columns<800 && 147<Row && Row<197)
            {
                update_Infor = 2; //Age
                flag_bpnhap = 1;
                flag_DT = 0;
                flag_Caps = 0;
                flag_Display = 1;
                flag_BP = 1;
                Display_UP = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else if(180<Columns && Columns<800 && 199<Row && Row<249)
            {
                update_Infor = 3; //weight
                flag_bpnhap = 1;
                flag_DT = 0;
                flag_Caps = 0;
                flag_Display = 1;
                flag_BP = 1;
                Display_UP = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
            else if(180<Columns && Columns<800 && 251<Row && Row<301)
            {
                update_Infor = 4; //sex
                flag_bpnhap = 1;
                flag_DT = 0;
                flag_Caps = 0;
                flag_Display = 1;
                flag_BP = 1;
                Display_UP = 1;
                Columns = 0;
                Row = 0;
                tft.fillScreen(RA8875_BLACK);
            }
        }
        if(flag_BP == 1)
            nhan();
    }
    }
}
void them(char strin[],char c)
{
  int leng = strlen(strin);
  strin[leng] = c;
  strin[leng+1] = '\0';
}
void xoa(char strin[])
{
  int leng = strlen(strin);
  strin[leng-1] = '\0';
}
void xoa_toan(char strin[])
{
  strin[0] = '\0';
}
void GetData_()
{

    String Name_user;
    String Age_user;
    String Weight_user;
    String Sex_user;
    xoa_toan(NamePatient);
    xoa_toan(age);
    xoa_toan(Weight);
    xoa_toan(Sex);
    pox.shutdown();
    Firebase.getString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/name");
    Name_user = firebaseData.stringData();
    Name_user.toCharArray(NamePatient,(Name_user.length()+1));
    Firebase.getString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/age");
    Age_user = firebaseData.stringData();
    Age_user.toCharArray(age,(Age_user.length()+1));
    Firebase.getString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/weight");
    Weight_user = firebaseData.stringData();
    Weight_user.toCharArray(Weight,(Weight_user.length()+1));
    Firebase.getString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/gender");
    Sex_user = firebaseData.stringData();
    Sex_user.toCharArray(Sex,(Sex_user.length()+1));
    pox.resume();
    pox.update();
}
void send_infor()
{
    String Name_user = String(NamePatient);
    String Age_user = String(age);
    String Weight_user = String(Weight);
    String Sex_user = String(Sex);
//    pox.shutdown();
    Firebase.setString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/name", Name_user);
    Firebase.setString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/age", Age_user);
    Firebase.setString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/weight", Weight_user);
    Firebase.setString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/gender", Sex_user);
//    pox.resume();
//    pox.update();
}
void SendData()
{
    if(connected_Wifi == 1)
    {
       pox.shutdown();
       if(Level != Set_Level)
       {
            Firebase.setInt(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/level", Set_Level);
            Level = Set_Level;
       }
       Firebase.getInt(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/level");
       int New_Level = 0;
       New_Level = firebaseData.intData();
       if(Level != New_Level)
       {
        if(New_Level == 1)
        {
            Level_2 = 0;
            Level_3 = 0;
            Level_4 = 0;
            Level_1 = 1;
        }else if(New_Level == 2){
            Level_1 = 0;
            Level_3 = 0;
            Level_4 = 0;
            Level_2 = 1;
        }else if(New_Level == 3){
            Level_1 = 0;
            Level_2 = 0;
            Level_4 = 0;
            Level_3 = 1;
        }else if(New_Level == 4){
            Level_1 = 0;
            Level_2 = 0;
            Level_3 = 0;
            Level_4 = 1;
        }
        Set_Level = New_Level;
        Level = New_Level;
        flag_settimer = 1;
        flag_level = 1;
        flag_Display = 1;
        flag_change = 1;
        dem ++;
        if(New_Level == 0)
        {
            Level_1 = 0;
            Level_2 = 0;
            Level_3 = 0;
            Level_4 = 0;
           Set_Level = 0;
           Level = 0;
           dem = 0;
           timerWrite(timer, 0);
           timerAlarmDisable(timer);
           flag_off = 1;
        }
       }
      if((Bpm_ != 0) && (Spo2_ != 0))
      {
           Firebase.setFloat(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/bpm", Bpm_);
           Firebase.setFloat(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/spO2", Spo2_);
           if(90 < Spo2_ && Spo2_ < 94)
           {
             status_ = "Danger" ;
           }
           else 
             status_ = "Normal" ;
           Firebase.setString(firebaseData, "/Patient/" + S_name_doctor + "/" + S_ID_patient + "/status", status_);
      }
             
       pox.resume();
       pox.update();
    }
}
void canh_bao()
{
  if((Bpm_ != 0) && (Spo2_ != 0))
  {
    if(90 < Spo2_ && Spo2_ < 94)
    {
      digitalWrite(BUZZER,HIGH);
    }
    else digitalWrite(BUZZER,LOW);
  }
}
void execute_Task(){
  touch.loop();
  delay(100);
  contact = touch.readInput(rawdata);
  if(switch_session == 1)
  {
    if(connected_Wifi == 1)
    {
      if(WiFi.status() != WL_CONNECTED)
      {
        pox.update();
        connected_Wifi = 0;
        tft.textEnlarge(0);
        tft.textColor(RA8875_WHITE, RA8875_BLACK);
        tft.textSetCursor(30, 15);
        tft.textWrite("                            ");
        tft.textSetCursor(30, 15);
        tft.textWrite("no connect");
        pox.update();
      }
    }
  }
  if(flag_get_ == 0)
  {
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
    { 
      get_sensor();
      canh_bao();
      if(switch_session == 1)
      {
        display_sensor();
      }  
      tsLastReport = millis();
    }
  }
  if(Set_Level != 0)
  {
    pox.update();
    if(Set_Level == 1)
    {
      cycle_pulse = 528;
      Time_bop = 1183152;
    }
    else if(Set_Level == 2)
    {
      cycle_pulse = 528;
      Time_bop = 883152;
    }
    else if(Set_Level == 3)
    {
      cycle_pulse = 924;
      Time_bop = 1074000;
    }
    else if(Set_Level == 4)
    {
      cycle_pulse = 924;
      Time_bop = 945512;
    }
    if(flag_settimer == 1)
    {
      Serial.println("thuc hien");
       timerAlarmWrite(timer,600, true);
       timerAlarmEnable(timer); 
       flag_settimer = 0;
    }
  }
  if(connected_Wifi == 1)
    {
      if(update_thongtin == 1){
        //ham update benh nhan
        GetData_();
        update_thongtin = 0;
      }
      if(send_information == 1)
      {
        send_infor();
        send_information = 0;
      }
    }
  if(flag_Display == 1)
  {
    if(flag_level == 1)
    {
      GIAO_DIEN_LEVEL();
      flag_level = 0;
    }
    else
    {
      if(switch_session == 0)
      {
        enter_login();
      }
      if(switch_session == 1)
          GIAO_DIEN_2();
      else if(switch_session == 2)
      {
        wifi_();
      }
      else if(switch_session == 3)
      {
        if(flag_BP == 1)
        {
          if(flag_AddText == 1)
          {
            if(update_Infor == 1)
            {
                if(flag_Delete == 1)
                {
                    xoa(NamePatient);
                    flag_Delete = 0;
                }  
                else
                    them(NamePatient,temp);
            }
            if(update_Infor == 2)
            {
                if(flag_Delete == 1)
                {
                    xoa(age);
                    flag_Delete = 0;
                }  
                else
                    them(age,temp);
            }
            if(update_Infor == 3)
            {
                if(flag_Delete == 1)
                {
                    xoa(Weight);
                    flag_Delete = 0;
                }  
                else
                    them(Weight,temp);
            }
            if(update_Infor == 4)
            {
                if(flag_Delete == 1)
                {
                    xoa(Sex);
                    flag_Delete = 0;
                }  
                else
                    them(Sex,temp);
            }
            flag_AddText = 0;
          }
          if(flag_bpnhap == 1)
          {
            flag_bpnhap = 0;
            flag_get_ = 0;
            BAN_PHIM();
          }
            if(Display_UP == 1){
              pox.update();
              tft.textSetCursor(5, 45);
              tft.textTransparent(RA8875_WHITE);
              tft.textEnlarge(2);
              tft.textWrite(string7);
              tft.textSetCursor(5, 95);
              tft.textWrite(string1);
              tft.drawRect(180,95,620,50,RA8875_WHITE);
              tft.textSetCursor(5, 147);
              tft.textWrite(string2);
              tft.drawRect(180,147,620,50,RA8875_WHITE);
              tft.textSetCursor(5, 199);
              pox.update();
              tft.textWrite(string3);
              tft.drawRect(180,199,620,50,RA8875_WHITE);
              tft.textSetCursor(5, 251);
              tft.textWrite(string4);
              tft.drawRect(180,251,620,50,RA8875_WHITE);
              tft.textSetCursor(185, 95);
              tft.textWrite(NamePatient);
              tft.textSetCursor(185, 147);
              tft.textWrite(age);
              tft.textSetCursor(185, 199);
              tft.textWrite(Weight);
              tft.textSetCursor(185, 251);
              tft.textWrite(Sex);
              Display_UP = 0;
              pox.update();
            }
             change_Infor();
        }
        else
            GIAO_DIEN_3();
      }
      flag_Display = 0;
    }
  }
  Switch_Session();   
}
void setup()
{
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,LOW);
  digitalWrite(enPin,LOW);
  digitalWrite(dirPin, LOW);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //khởi tạo chân led là output
  //khơit tạo timer với chu kì 1us vì thạch anh của ESP chạy 8MHz
  timer = timerBegin(3, 80, true);
  //khởi tạo hàm xử lý ngắt ngắt cho Timer
  timerAttachInterrupt(timer, &onTimer, true);
  Serial.begin(9600);
    Serial.println("RA8875 start");
  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }
  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(100);
  tft.fillScreen(RA8875_BLACK);
  tft.textMode();
  tft.textEnlarge(5);
  log_in();
  Wire.setClock(400000);
  Wire.begin();
  delay(300);
  touch.setHandler(handleTouch);
  touchStart();
  Serial.print("Initializing pulse oximeter..");
   if (!pox.begin()) {
       Serial.println("FAILED");
   } else {
       Serial.println("SUCCESS");
   }
  pox.setOnBeatDetectedCallback(onBeatDetected);
}
void loop(){
  execute_Task();
  pox.update();
}
