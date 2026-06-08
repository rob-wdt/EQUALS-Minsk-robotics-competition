#include "I2Cdev.h"
//#include "MPU6050_6Axis_MotionApps20.h"
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Servo dribblerESC;
//MPU6050 mpu;

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29, &Wire);
//коэффициенты
#define KP 0.8  //Пропрц.коэф.
#define KD 20.0 //Диф.коэф. 
#define KPX 0.31 //0.22
#define KPY 0.33 //0.35
//#define KPX2 0.35 //0.22
#define KPY2 0.71 //0.35 
#define KDX 1.5
#define KPX2 0.15
const int right_out2G = 80;
const int left_out2G = -78;
const int right_out1G = 55;
const int left_out1G = -57;
int right_out = 0;
int left_out = 0;

const int forward_out = 15;
const int backward_out = 15;

const float YzeroFront = 40;//36
const float YzeroFront2 = 34;
const float XzeroRight = 35;
float abs_ball_angle_old;
//const float Yzero = 34;
bool flagCentr = false;
//Порты управления цопами
#define ADDR_P1 29
#define ADDR_P2 27
#define ADDR_P3 25
#define ADDR_P4 23
#define BALL_SEN_SIGNAL_1 A15
#define BALL_SEN_SIGNAL_2 A13

#define BUT1 32
#define BUT2 36
#define BUT3 34

#define FOTOTRANZ A14

#define RAD2DEG  57.2957795130823208767
#define DEG2RAD 0.01745329251994329576
#define ec 2.7182

#define DRIBLER_PORT 3
#define ESC_ARM_HIGH 2000   // Max throttle (for arming, if required)
#define ESC_ARM_LOW  1000   // Minimum throttle
#define ESC_STOP     1000   // Stop signal
#define ESC_RUN      1600   // Dribbler working speed (adjust if needed)

//led 30 24 22
//Порты управления моторами
//черный
//#define M1_1 10
//#define M1_2 12
//#define M2_1 9
//#define M2_2 11
//#define M3_1 6
//#define M3_2 8
//#define M4_1 5
//#define M4_2 7

//зеленый
#define M1_1 12
#define M1_2 10
#define M2_1 11
#define M2_2 9
#define M3_1 8
#define M3_2 6
#define M4_1 5
#define M4_2 7
//Порт управления солиноидом
#define pinsolin 31

#define OTLADKA 0

bool Dribler = true;//true false
/*Макрос отладки
  0-рабочий режим
  1-проверка моторов
  2-проверка гиро
  3-проверка передачи с камеры
  4-проверка цопов
  5-проверка солинойда
  6-проверка дриблера
  7-проверка дальномеров
  8-координаты
*/


volatile bool MPUInterrupt = false;

uint8_t MPUIntStatus;
uint16_t packetSize;
uint8_t fifoBuffer[45];// буфер
uint32_t timer;
uint32_t timer_kick;
uint32_t timer_kick2;
uint32_t timer_goForward;
uint32_t timer_goForward2;
bool flagZeroGate = false;
float angleGyro;
float errAngleGyro;
float corAng;
float abs_angle;
byte ball_retention;
float ball_angle;
float ball_dist;
float forward_dist;
float backward_dist;
float forward_angle;
float backward_angle;
int camera_sign;
byte data_cam[6];
float yel_angle, yel_dist, blue_angle, blue_dist;
double err_old = 0;     //Кубическая составляющая

double _data, angK, distK;
//координаты

float abs_forward_angle;
float abs_backward_angle;
float abs_ball_angle;

float x_forward;
float y_forward;

float x_backward;
float y_backward;

float Correct_coef;

float x;
float y;

float xx;
float yy;

//-80 40
//80 40
//80 110
//-70 110
int OneGate_Out_KofX;

const int spdMinX = 0;
const int spdMinY = 0;
const int spdMaxX = 10;
const int spdMaxY = 15;

float out_angle;
float Fang;
bool flagOne = true;
bool kickDel = false;
bool lastFlagZeroGate = false;  
bool flagOneGate = false;
bool flagOut = false;
bool flagKick = false;
bool GoFor = false;
bool wasMoving = true;
const int ir_addr3[32] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 31, 30, 29, 28, 24, 25, 26, 27};
int ball_data[32];
float d_alpha = 11.25;
float ball_ts_angle, ball_ts_dist;

float spdGLK;
float angle_gate_ball;
float alphaGLK;
float angle_gate;
float angle_gate_ball_old;
float spdX;
float spdY;
float spdX2;
float spdY2;
float alphaY;
float alphaX;
float ball_cam_angle;
byte switchT_C;
float ball_cam_dist;
bool flagStart = true;

int State = 0;
/*
   STATE 1 езда к мячу
   STATE 2 ехать на ворота
   STATE 3 ауты
*/
class Motor {
  private:
    int speedMotor;
  public:
    int port1;  //порт мотора
    int port2;  //порт мотора

    void setSpeeds(int speeds) {       // функция скорости моторов
      if (speeds > 0) {
        digitalWrite(port1, LOW);    //Направление вращения мотора
        analogWrite(port2, min(speeds, 220));  //Скорость вращения мотора
      }
      else {
        digitalWrite(port2, LOW);    //Направление вращения мотора
        analogWrite(port1, min(-speeds, 220));  //Скорость вращения мотора
      }
    }
};


Motor motor1;
Motor motor2;
Motor motor3;
Motor motor4;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial.print("Start");
  //digitalWrite(pinsolin, LOW);


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A14, OUTPUT);
  pinMode(ADDR_P1, OUTPUT);
  pinMode(ADDR_P2, OUTPUT);
  pinMode(ADDR_P3, OUTPUT);
  pinMode(ADDR_P4, OUTPUT);
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);
  pinMode(pinsolin, OUTPUT);
  pinMode(BALL_SEN_SIGNAL_1, INPUT_PULLUP);
  pinMode(BALL_SEN_SIGNAL_2, INPUT_PULLUP);
  pinMode(FOTOTRANZ, INPUT);
  digitalWrite(30, LOW);
  digitalWrite(24, LOW);
  digitalWrite(22, LOW);
  while (1) {
    digitalWrite(30, HIGH);
    if (digitalRead(BUT1) == 0) {

      break;
    }
  }

  //Serial.print("dffg");
  Wire.begin();
  Wire.setClock(400000);
  //Wire.setWireTimeout(100);
  // инициализация DMP
  //  mpu.initialize();
  //  mpu.dmpInitialize();
  //  mpu.setDMPEnabled(true);
  uint16_t gyroTimer = millis();
  unsigned long t0 = millis();
  timer_kick = millis();
  bno.begin();
  bno.setMode(OPERATION_MODE_IMUPLUS);
#if OTLADKA==0
  while ((gyroTimer - t0) < 5000) {
    gyroTimer = millis();   // без повторного объявления типа
    gyro();
    digitalWrite(LED_BUILTIN, millis() % 200 > 100);
    //Serial.println(gyroTimer);
  }
  gyroTimer = 0;
  errAngleGyro = angleGyro;
#elif OTLADKA==2
  while ((gyroTimer - t0) < 5000) {
    gyroTimer = millis();   // без повторного объявления типа
    gyro();
    digitalWrite(LED_BUILTIN, millis() % 200 > 100);
    Serial.println(gyroTimer);
  }
  gyroTimer = 0;
  errAngleGyro = angleGyro;
#elif OTLADKA==8
  while ((gyroTimer - t0) < 5000) {
    gyroTimer = millis();   // без повторного объявления типа
    gyro();
    digitalWrite(LED_BUILTIN, millis() % 200 > 100);
    //Serial.println(gyroTimer);
  }
  gyroTimer = 0;
  errAngleGyro = angleGyro;
#endif

  motor2.port1 = M2_1;
  motor2.port2 = M2_2;
  motor4.port1 = M4_1;
  motor4.port2 = M4_2;
  motor3.port1 = M3_2;
  motor3.port2 = M3_1;
  motor1.port1 = M1_2;
  motor1.port2 = M1_1;
  if (Dribler == true) {
    dribblerESC.attach(DRIBLER_PORT);
    delay(500);
    dribblerESC.writeMicroseconds(1630);
    delay(500);
    dribblerESC.writeMicroseconds(800);
    delay(500);
    //    dribblerESC.writeMicroseconds(ESC_ARM_LOW);
    //    delay(500);
  }
  Serial.print("End");
  digitalWrite(30, LOW);



}

void loop() {
  while (flagStart == true) {
    goAngle(0, 0, 0);
    digitalWrite(22, HIGH);
    digitalWrite(24, LOW);
    if (digitalRead(BUT2) == 0) {
      Serial.println("Start game");
      flagStart = false;
      break;
    }
  }


  digitalWrite(22, HIGH);
  digitalWrite(24, LOW);
#if OTLADKA==0

  updates();
  tactic();




  //
#elif OTLADKA==1
  goAngle(0, 0, 100);
  //  motor1.setSpeeds(150); //m1
  //  motor2.setSpeeds(150); //m2
  //  motor3.setSpeeds(150); //m3
  //  motor4.setSpeeds(150); // m4
#elif OTLADKA==2
  corAng = lead_to_degree_borders(angleGyro - errAngleGyro);
  gyro();
  Serial.print("corAng\t  ");
  Serial.print(corAng);//180-
  Serial.print("  angleGyro  ");
  Serial.print(angleGyro);//180-
  Serial.print("  errAngleGyro\t  ");
  Serial.print(errAngleGyro);//180-
  Serial.println(" ");
#elif OTLADKA==3
  //Camera();
  updates();
  Serial.print("yel_angle\t  ");
  Serial.print(yel_angle);
  Serial.print("  ||  ");
  Serial.print(yel_dist);
  Serial.print("  ||  ");
  Serial.print(ball_cam_angle);
  Serial.print("  ||  ");
  Serial.print(ball_cam_dist);
  Serial.print("  ||  ");
  Serial.print(blue_angle);
  Serial.print("  ||  ");
  Serial.print(blue_dist);
  Serial.println(" ");//180-
#elif OTLADKA==4
  data_tcops();
  for (int i = 0; i < 32; i++)
  {
    Serial.print(ball_data[i]);//4tc - 2
    Serial.print(" ");
  }
  Serial.println(" ");
  //Serial.print(ball_ts_distance);
  Serial.print(" ");
  Serial.println(ball_ts_angle);

#elif OTLADKA==5
  if (millis() - timer_kick >= 5000) {
    kick();
    timer_kick = millis(); // обязательно обновить, иначе kick будет вызываться постоянно
  }
  kick_Del();
  //  Serial.print("  ");
  //  Serial.println("1241");
#elif OTLADKA==6
  //  for (int pos = 800; pos <= 1630; pos += 1)
  //  {
  dribblerESC.writeMicroseconds(1620);
  //delay(20);
  //Serial.println(pos);
  //  }

#elif OTLADKA==7
  readSensors();
  Serial.print("ball_retention");
  Serial.println(ball_retention);
  //  Serial.print("Right_dist");
  //  Serial.print(Right_dist);
  //  Serial.print("Forward_dist");
  //  Serial.print(Forward_dist);
  //  Serial.print("Back_dist");
  //  Serial.println(Back_dist);
#elif OTLADKA==8
  updates();
  Serial.print(corAng);
  Serial.print("corAng\t  ");
  Serial.print(abs_forward_angle);
  Serial.print("  ");
  Serial.print(abs_backward_angle);
  Serial.print("  ||  ");
  //  Serial.print(yel_angle);
  //  Serial.print("  ");
  //  Serial.print(blue_angle);
  //  Serial.print("  ||  ");
  Serial.print(forward_dist);
  Serial.print("  ");
  Serial.print(backward_dist);
  Serial.print("  ||  ");
  Serial.print(x_backward);
  Serial.print("  ");
  Serial.print(x_forward);
  Serial.print("  ||  ");
  Serial.print(y_backward);
  Serial.print("  ");
  Serial.print(y_forward);
  Serial.print("  ||  ");
  Serial.print(Correct_coef);
  Serial.print("  ");
  Serial.print(x);
  Serial.print("  ");
  Serial.print(y);
  //  Serial.print("  ");
  //  Serial.print("  ||  ");
  //  Serial.print(Correct_coef);
  //  Serial.print("  ");
  //  Serial.print(xx);
  //  Serial.print("  ");
  //  Serial.print(yy);
  Serial.println("  ");
#endif

  if (digitalRead(BUT3) == 0 && flagStart == false) {

    flagStart = true;
    Serial.println("Stop");
  }
}
