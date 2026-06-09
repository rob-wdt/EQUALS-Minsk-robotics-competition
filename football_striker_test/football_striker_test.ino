#include "I2Cdev.h"
// #include "MPU6050_6Axis_MotionApps20.h"
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OTLADKA 0
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
  9-выравнивание на 0
*/

#define SPEED 150

#define GOAL_YELLOW 0
#define GOAL_BLUE 1

#define OWN_GOAL GOAL_BLUE //////////////////////////

#if OWN_GOAL == GOAL_YELLOW
#define OPP_GOAL GOAL_BLUE
#elif OWN_GOAL == GOAL_BLUE
#define OPP_GOAL GOAL_YELLOW
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Servo dribblerESC;
// MPU6050 mpu;
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29, &Wire);
// коэффициенты
#define KP 0.8    // Пропрц.коэф.
#define KD 20.0   // Диф.коэф.
#define KC 0.0001 // Куб.коэф.

int right_out2G = 100;
int left_out2G = -55;
int right_out1G = 80;
int left_out1G = -45;

int right_out = 0;
int left_out = 0;

int forward_out = 40;
int backward_out = 35;

int abs_backward_out = 35;
int abs_forward_out = 0;

float abs_ball_angle;
// Порты управления цопами
#define ADDR_P1 29
#define ADDR_P2 27
#define ADDR_P3 25
#define ADDR_P4 23
#define BALL_SEN_SIGNAL_1 A15
#define BALL_SEN_SIGNAL_2 A13

#define BUT1 32
#define BUT2 36
#define BUT3 34

#define PHOTOTRANSISTOR_PIN A14

#define RAD2DEG 57.2957795130823208767
#define DEG2RAD 0.01745329251994329576
#define ec 2.7182

#define DRIBLER_PORT 3
#define ESC_ARM_HIGH 2000 // Max throttle (for arming, if required)
#define ESC_ARM_LOW 1000  // Minimum throttle
#define ESC_STOP 1000     // Stop signal
#define ESC_RUN 1600      // Dribbler working speed (adjust if needed)

// led 30 24 22
// Порты управления моторами

#define M1_1 10
#define M1_2 12
#define M2_1 9
#define M2_2 11
#define M3_1 6
#define M3_2 8
#define M4_1 5
#define M4_2 7

// #define M1_1 12
// #define M1_2 10
// #define M2_1 11
// #define M2_2 9
// #define M3_1 8
// #define M3_2 6
// #define M4_1 5
// #define M4_2 7
// Порт управления солиноидом
#define pinsolin 31

bool Dribler = true; // true false

volatile bool MPUInterrupt = false;

uint8_t MPUIntStatus;
uint16_t packetSize;
uint8_t fifoBuffer[45]; // буфер
uint32_t timer;
uint32_t timer_kick;
uint32_t timer_kick2;
uint32_t placementTimer = 0;
const uint32_t PLACEMENT_ALIGN_MS = 4000;
float angleGyro;
float errAngleGyro;
bool flagST = false;
float curAngle;
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
double err_old = 0; // Кубическая составляющая

double _data, angK, distK;
// координаты

float abs_forward_angle;
float abs_backward_angle;

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
// 80 40
// 80 110
//-70 110
int OneGate_Out_KofX;

float out_angle;
bool flagOne = false;
bool flagOutStart = false;
bool kickDel = false;
bool flagOneGate = false;
bool flagOut = false;
bool flagKick = false;
bool flagKickPosition = false;
bool driblerON = false;
bool flagBadZone = false;
bool flagShortBall = false;
bool flagPlacement = false;
const int ir_addr3[32] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 31, 30, 29, 28, 24, 25, 26, 27};
int ball_data[32];
float d_alpha = 11.25;
float ball_ts_angle, ball_ts_dist;
float Priority_Angle;
float spdGLK;
float alphaGLK;
float ball_cam_angle;
byte switchT_C;
float ball_cam_dist;
bool flagOff = true;
bool flagStart = true;
int spdSHR;
int spdMinl;

int State = 0;
/*
   STATE 1 езда к мячу
   STATE 2 ехать на ворота
   STATE 3 ауты
*/
class Motor
{
private:
    int speedMotor;

public:
    int port1; // порт мотора
    int port2; // порт мотора

    void setSpeed(int speed)
    { // функция скорости моторов
        if (speed > 0)
        {
            digitalWrite(port1, LOW);            // Направление вращения мотора
            analogWrite(port2, min(speed, 220)); // Скорость вращения мотора
        }
        else
        {
            digitalWrite(port2, LOW);             // Направление вращения мотора
            analogWrite(port1, min(-speed, 220)); // Скорость вращения мотора
        }
    }
};

Motor motor1;
Motor motor2;
Motor motor3;
Motor motor4;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial3.begin(115200);
    Serial.print("Start");
    // digitalWrite(pinsolin, LOW);

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
    pinMode(PHOTOTRANSISTOR_PIN, INPUT);
    digitalWrite(30, LOW);
    digitalWrite(24, LOW);
    digitalWrite(22, LOW);

    Serial.println("pinModes complete.\nPress button #2 to continue");
    while (digitalRead(BUT1))
    {
        digitalWrite(30, HIGH);
    }

    // Serial.print("dffg");
    Wire.begin();
    Wire.setClock(400000);
    // Wire.setWireTimeout(100);
    //  инициализация DMP
    //   mpu.initialize();
    //   mpu.dmpInitialize();
    //   mpu.setDMPEnabled(true);
    uint16_t gyroTimer = millis();
    unsigned long t0 = millis();
    timer_kick = millis();
    bno.begin();
    bno.setMode(OPERATION_MODE_IMUPLUS);
#if OTLADKA == 0 || OTLADKA == 2 || OTLADKA == 8
    while ((gyroTimer - t0) < 5000)
    {
        gyroTimer = millis(); // без повторного объявления типа
        gyro();
#if OTLADKA != 0
        digitalWrite(LED_BUILTIN, millis() % 200 > 100);
#if OTLADKA != 8
        Serial.println(gyroTimer);
#endif
#endif
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
    if (Dribler == true)
    {
        dribblerESC.attach(DRIBLER_PORT);
        delay(500);
        dribblerESC.writeMicroseconds(1630);
        delay(500);
        dribblerESC.writeMicroseconds(800);
        delay(500);
        //    dribblerESC.writeMicroseconds(ESC_ARM_LOW);
        //    delay(500);
    }
    Serial.println("End");
    digitalWrite(30, LOW);

    Serial.println("Press button #1 to continue");
}

void loop()
{
    digitalWrite(22, HIGH);
    digitalWrite(24, LOW);

    // errAngleGyro = angleGyro;

    if (digitalRead(BUT2) == 0)
    {
        // Serial.println("Start game");
        // flagStart = false;
        flagOutStart = false;
        flagOff = false;
        flagST = true;
    }
    if (digitalRead(BUT3) == 0)
    {
        // recalibrateGyro();
        // flagPlacement = true;
        // flagStart = true;
        placementTimer = millis();
        // flagOut = false;
        flagOutStart = true;
        flagOff = true;
    }
    if (flagOutStart == true)
    {
        gyro();
        curAngle = angleGyro;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if OTLADKA == 0

    digitalWrite(LED_BUILTIN, millis() % 1000 > 500);
    Camera();
    gyro();

    ball_cam_angle = normalize_angle(ball_cam_angle);

    // circular ball detour
    int16_t move_angle{ball_cam_angle};
    if (ball_cam_angle < -10)
    {
        move_angle -= 90;
    }
    else if (ball_cam_angle > 10)
    {
        move_angle += 90;
    }

    int16_t align_angle{};
#if OPP_GOAL == GOAL_YELLOW
    align_angle = normalize_angle(yel_angle);
#elif OPP_GOAL == GOAL_BLUE
    align_angle = normalize_angle(blue_angle);
#endif

    goAngle(move_angle, align_angle, SPEED);

    Serial.println(String{ball_cam_angle} + '\t' + String{move_angle});

#elif OTLADKA == 1
    updates();
    goAngle(0, 0, 100);
    //  motor1.setSpeeds(150); //m1
    //  motor2.setSpeeds(150); //m2
    //  motor3.setSpeeds(150); //m3
    //  motor4.setSpeeds(150); // m4
#elif OTLADKA == 2
    curAngle = lead_to_degree_borders(angleGyro - errAngleGyro);
    gyro();
    Serial.print("curAng\t");
    Serial.print(curAngle); // 180-
    Serial.print("\tangleGyro\t");
    Serial.print(angleGyro); // 180-
    Serial.print("\terrAngleGyro\t");
    Serial.print(errAngleGyro); // 180-
    Serial.println();
#elif OTLADKA == 3
    // Camera();
    updates();
    Serial.print("yellow_angle\t  ");
    Serial.print(normalize_angle(yel_angle));
    Serial.print("  ||  ");
    Serial.print("yellow_dist\t  ");
    Serial.print(yel_dist);
    Serial.print("  ||  ");
    Serial.print("blue angle\t  ");
    Serial.print(normalize_angle(blue_angle));
    Serial.print("  ||  ");
    Serial.print("blue dist\t  ");
    Serial.print(blue_dist);
    Serial.print("  ||  ");
    Serial.print("ball_angle\t  ");
    Serial.print(normalize_angle(ball_cam_angle));
    Serial.print("  ||  ");
    Serial.print("ball dist\t  ");
    Serial.print(ball_cam_dist);
    Serial.println(" "); // 180-
#elif OTLADKA == 4
    data_tcops();
    for (int i = 0; i < 32; i++)
    {
        Serial.print(ball_data[i]); // 4tc - 2
        Serial.print(" ");
    }
    Serial.println(" ");
    // Serial.print(ball_ts_distance);
    Serial.print(" ");
    Serial.println(ball_ts_angle);

#elif OTLADKA == 5
    if (millis() - timer_kick >= 5000)
    {
        kick();
        timer_kick = millis(); // обязательно обновить, иначе kick будет вызываться постоянно
    }
    kick_Del();
    //  Serial.print("  ");
    //  Serial.println("1241");
#elif OTLADKA == 6
    updates();
    dribler(1614);
    /*if (ball_cam_dist <= 20) {
        dribler(1615);
        driblerON = true;
        }

        else if (ball_retention <= 3 || (driblerON == true && ball_cam_dist == 0)) {
        dribler(1620);
        }
        else if (ball_cam_dist > 20 && ball_cam_dist != 0) {dribler(0);driblerON = false;}*/
#elif OTLADKA == 7
    readSensors();
    Serial.print("ball_retention:\t");
    Serial.println(ball_retention);
    //  Serial.print("Right_dist");
    //  Serial.print(Right_dist);
    //  Serial.print("Forward_dist");
    //  Serial.print(Forward_dist);
    //  Serial.print("Back_dist");
    //  Serial.println(Back_dist);

#elif OTLADKA == 8 // Coords
    updates();
    Serial.print("curAngle\t");
    Serial.print(curAngle);
    Serial.print("\tforward angle:\t");
    Serial.print(abs_forward_angle);
    Serial.print("\tbackward angle:\t");
    Serial.print(abs_backward_angle);
    Serial.print("\t||\t");
    //  Serial.print(yel_angle);
    //  Serial.print("  ");
    //  Serial.print(blue_angle);
    //  Serial.print("  ||  ");
    Serial.print(forward_dist);
    Serial.print("\t");
    Serial.print(backward_dist);
    Serial.print("\t||\t");
    Serial.print(x_backward);
    Serial.print("\t");
    Serial.print(x_forward);
    Serial.print("\t||\t");
    Serial.print(y_backward);
    Serial.print("\t");
    Serial.print(y_forward);
    Serial.print("\t||\t");
    Serial.print(Correct_coef);
    Serial.print("\t");
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    //  Serial.print("  ");
    //  Serial.print("  ||  ");
    //  Serial.print(Correct_coef);
    //  Serial.print("  ");
    //  Serial.print(xx);
    //  Serial.print("  ");
    //  Serial.print(yy);
    Serial.println("  ");
#elif OTLADKA == 9
    gyro();
    curAngle = lead_to_degree_borders(angleGyro - errAngleGyro);
    Camera();

    int16_t align_angle{};
#if OPP_GOAL == GOAL_YELLOW
    align_angle = normalize_angle(yel_angle);
#elif OPP_GOAL == GOAL_BLUE
    align_angle = normalize_angle(blue_angle);
#endif

    Serial.println("current angle:\t" + String{curAngle} + "\tgoal angle:\t" + String{align_angle});

    goAngle(0, align_angle, 0);
#endif

    // delay(200);  // антидребезг
}
