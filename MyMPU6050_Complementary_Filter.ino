
#include<Wire.h>
#include<Servo.h>


TwoWire Wire2(PB11,PB10);
HardwareSerial Serial2(PA3, PA2);
#define led PC13
int c=0;
int  MPU6050_AXOFFSET ;
int  MPU6050_AYOFFSET ;
int  MPU6050_AZOFFSET ;
int  MPU6050_GXOFFSET ;
int  MPU6050_GYOFFSET ;
int  MPU6050_GZOFFSET ;
Servo ESC1, ESC2, ESC3, ESC4;
long sampling_timer;
const int MPU_addr=0x68;  // I2C address of the MPU-6050

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; // Raw data of MPU6050
float GAcX, GAcY, GAcZ; // Convert accelerometer to gravity value
float Cal_GyX,Cal_GyY,Cal_GyZ; // Pitch, Roll & Yaw of Gyroscope applied time factor
float acc_pitch, acc_roll, acc_yaw; // Pitch, Roll & Yaw from Accelerometer
float angle_pitch, angle_roll, angle_yaw; // Angle of Pitch, Roll, & Yaw
float alpha = 0.9996; // Complementary constant

float pid_roll_setpoint=0, pid_pitch_setpoint=0, pid_yaw_setpoint;
float gyro_pitch_input, gyro_roll_input, gyro_yaw_input;

float p_tune=2, d_tune=10 , i_tune=0.0001;
float pid_p_gain_roll=p_tune, pid_i_gain_roll=i_tune, pid_d_gain_roll=d_tune; //pid roll
float pid_p_gain_pitch=p_tune, pid_i_gain_pitch=i_tune, pid_d_gain_pitch=d_tune; //pid pitch
float pid_p_gain_yaw=0, pid_i_gain_yaw, pid_d_gain_yaw; // pid yaw


float pid_error_temp;
float pid_i_mem_roll, pid_i_mem_pitch, pid_i_mem_yaw;
int pid_max_roll=400, pid_max_pitch=400, pid_max_yaw=400;
float pid_output_roll, pid_output_pitch, pid_output_yaw;
float pid_last_roll_d_error,pid_last_pitch_d_error,pid_last_yaw_d_error;
float esc1, esc2, esc3, esc4;
int throttle = 1000 , read_Serial;
float esc1_;
int int_test;
String Sr2;
void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.setTimeout(50);
//  Blynk.begin(auth, ssid, pass);
  Serial.println("Attach esc.....");
  Wire2.begin();
  ESC1.attach(PA7);
  ESC2.attach(PB0);
  ESC3.attach(PA6);
  ESC4.attach(PB1);
  Serial.println("Set up ESCs:");
  ESC1.writeMicroseconds(1000);
  ESC3.writeMicroseconds(1000);
  ESC2.writeMicroseconds(1000);
  ESC4.writeMicroseconds(1000);
  Serial.println("init_MPU");
  init_MPU6050();
  delay(2000);
  calib_gyro();
  Serial.println("Gyro offset value:");
  Serial.println(MPU6050_AXOFFSET);
  Serial.println(MPU6050_AYOFFSET);
  Serial.println(MPU6050_AZOFFSET);
  Serial.println(MPU6050_GXOFFSET);
  Serial.println(MPU6050_GYOFFSET);
  Serial.println(MPU6050_GZOFFSET);
    ESC1.writeMicroseconds(1100);
  ESC3.writeMicroseconds(1100);
  ESC2.writeMicroseconds(1100);
  ESC4.writeMicroseconds(1100);
  delay(3000); 
  Serial.println("Waiting for start quadcopter :");
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  sampling_timer=micros();

  pinMode(led, OUTPUT);
  digitalWrite(led ,HIGH);
  ESC1.writeMicroseconds(1000);
  ESC3.writeMicroseconds(1000);
  ESC2.writeMicroseconds(1000);
  ESC4.writeMicroseconds(1000);
}

void loop(){
  
    read_imu();
  
    if (Serial2.available() > 0) {
    Sr2 = Serial2.readString();
        if (Sr2.startsWith("tt ")){
            String a = Sr2.substring(3);
//            Serial.println(a.toInt());
            throttle = a.toInt();
//            Serial.println("       ");
//            Serial.println(throttle);
//            Serial.println("       ");
        }
        if (Sr2.startsWith("p ")){
            String P = Sr2.substring(2);
//            Serial.println(P.toFloat());
            p_tune = P.toFloat();
            pid_p_gain_roll=p_tune;
            pid_p_gain_pitch=p_tune;
        }
        if (Sr2.startsWith("i ")){
            String I = Sr2.substring(2);
            i_tune = I.toFloat();
            pid_i_gain_roll=i_tune;
            pid_i_gain_pitch=i_tune;
//            Serial.println(I.toFloat());
        }
        if (Sr2.startsWith("d ")){
            String D = Sr2.substring(2);
            d_tune = D.toFloat();
            pid_d_gain_roll=d_tune;
            pid_d_gain_pitch=d_tune;
//            Serial.println(D.toFloat());
        }
    }
  gyro_pitch_input = angle_pitch;
  gyro_roll_input = angle_roll;    
  calculate_pid();

//
  if (throttle > 1700) {throttle=1700;}
  
  esc1= throttle - pid_output_pitch + pid_output_roll;
  esc2= throttle + pid_output_pitch - pid_output_roll;
  esc3= throttle + pid_output_pitch + pid_output_roll;
  esc4= throttle - pid_output_pitch - pid_output_roll;

//
  if (esc1 <1000) esc1 =1000;
  if (esc2 <1000) esc2 =1000;
  if (esc3 <1000) esc3 =1000;
  if (esc4 <1000) esc4 =1000;

  if (esc1 >2000) esc1 =2000;
  if (esc2 >2000) esc2 =2000;
  if (esc3 >2000) esc3 =2000;
  if (esc4 >2000) esc4 =2000; 
//  if(Serial.available()) 
//    read_Serial = Serial.parseInt();
//    if (read_Serial>0)
//        {Serial.println(read_Serial);
//        throttle=map(read_Serial,1000,2000,1000,2000);}
  ESC1.writeMicroseconds(int(esc1));
  ESC2.writeMicroseconds(int(esc2));
  ESC3.writeMicroseconds(int(esc3));
  ESC4.writeMicroseconds(int(esc4));
//  Serial.println("-------------xx
//  Serial.println("--------------------");
//  Serial.println(p_tune);
//  Serial.println(i_tune*1000);
//  Serial.println(d_tune);

  
//if(int(angle_pitch) >20) {
//  digitalWrite(D4 ,HIGH);
//  delay(500);
//  digitalWrite(D4, LOW);
//  delay(500);
//  }
blink_pc13(c,50);c++;
//Serial.println(micros() - sampling_timer);
  
  // Sampling Timer
  while(micros() - sampling_timer < 4000); //
  sampling_timer = micros(); //Reset the sampling timer  
//  Serial.println(micros() - sampling_timer);
}
void blink_pc13(int c, int d){
  
if (c==100000){c=0;}
if (c%100 <d){
  digitalWrite(led ,LOW);
  }
  else 
  {digitalWrite(led ,HIGH);}
  }
