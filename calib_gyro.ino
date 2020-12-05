void calib_gyro(){
// Read raw data of MPU6050
  double sum_AcX=0,sum_AcY=0,sum_AcZ=0,sum_GyX=0,sum_GyY=0,sum_GyZ=0;
  int i=0;
  while(i<200){
    Wire2.beginTransmission(MPU_addr);
    Wire2.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire2.endTransmission(false);
    Wire2.requestFrom(MPU_addr,20,true);  // request a total of 14 registers
    AcX=Wire2.read()<<8|Wire2.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    AcY=Wire2.read()<<8|Wire2.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire2.read()<<8|Wire2.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire2.read()<<8|Wire2.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire2.read()<<8|Wire2.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire2.read()<<8|Wire2.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire2.read()<<8|Wire2.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    
    
    sum_AcX = sum_AcX +AcX;
    sum_AcY = sum_AcY +AcY;
    sum_AcZ = sum_AcZ +AcZ;
    sum_GyX = sum_GyX +GyX;
    sum_GyY = sum_GyY +GyY;
    sum_GyZ = sum_GyZ +GyZ;
    i++;
  
  
  }
  
  MPU6050_AXOFFSET = sum_AcX/200;
  MPU6050_AYOFFSET = sum_AcY/200;
  MPU6050_AZOFFSET = sum_AcZ/200;
  MPU6050_GXOFFSET = sum_GyX/200;
  MPU6050_GYOFFSET = sum_GyY/200;
  MPU6050_GZOFFSET = sum_GyZ/200;
  Serial.println("Finish calib gyro ");
  Serial.println("=========================================================");
}
