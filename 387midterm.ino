//Sketch to communicate with the MPU6050 chip and output the values from it to various leds and to motors;
//Very basic i2c communication based on sketch from Arduino User JohnChi from 2014.
//reads in the sensor data from the MPU-6050 and holds the past 10 read values in memory to calculate offsets
    #include<Wire.h>
    #include<stdlib.h>
    #include<stdio.h>
    const int MPU_addr=0x68;  // I2C address of the MPU-6050
    int16_t* hist = (int16_t * ) malloc(sizeof(int16_t) * 60);
    int16_t tmp;
    int16_t* offsets = (int16_t *) malloc(sizeof(int16_t) * 6);
      
    void setup(){
      pinMode(A0, OUTPUT);
      pinMode(A1, OUTPUT);
      pinMode(A2, OUTPUT);
      pinMode(10, OUTPUT);
      pinMode(11, OUTPUT);
      pinMode(12, OUTPUT);
      pinMode(5, OUTPUT);
      pinMode(6, OUTPUT);
      pinMode(7, OUTPUT);
      
      Wire.begin();
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x6B);  // PWR_MGMT_1 register
      Wire.write(0);     // set to zero (wakes up the MPU-6050)
      Wire.endTransmission(true);
      Serial.begin(9600);
      
      //block to establish a baseline for all of the values in the starting position and calibrate future measurements to that
      for(int i = 0; i < 30; i++){
         updateHist();
         Wire.beginTransmission(MPU_addr);
         Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
         Wire.endTransmission(false);
         Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
         hist[0]=Wire.read()<<8|Wire.read();  // AcX    
         hist[10]=Wire.read()<<8|Wire.read();  // AcY
         hist[20]=Wire.read()<<8|Wire.read();  // AcZ
         tmp = Wire.read()<<8|Wire.read();  // Throw away the temperature reading
         hist[30]=Wire.read()<<8|Wire.read();  // GyX
         hist[40]=Wire.read()<<8|Wire.read();  // GyY
         hist[50]=Wire.read()<<8|Wire.read();  // GyZ
         Wire.endTransmission(true);
         
      }
      
      
      //with a sample of data gathered, use this to calculate the offsets of all of the measurements.
      //we will consider the starting position as perfectly level for the purposes of calibration
      //this means that the values should supposedly be {0, 0, 16384, 0, 0, 0}
      for(int i = 0; i < 6; i++){
        long total = 0;
        for(int j = 0; j < 10; j ++){
          total = total + hist[(10 * i) + j];
        }
        offsets[i] = total / 10; 
      }
      //make sure the AcY is adjusted for gravity
      offsets[2] = offsets[2] - 16384; 
      
      for(int i = 0; i < 6; i++){
        Serial.println(offsets[i]);
      }
      Serial.println("Done Settting Up");
    }
    
    
    
    
    void loop(){
     
      updateHist();
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
      hist[0]= (Wire.read()<<8|Wire.read());  // AcX    
      hist[10]=(Wire.read()<<8|Wire.read());  // AcY
      hist[20]=(Wire.read()<<8|Wire.read());  // AcZ
      tmp = Wire.read()<<8|Wire.read();  // Throw away the temperature reading
      hist[30]=(Wire.read()<<8|Wire.read());  // GyX
      hist[40]=(Wire.read()<<8|Wire.read());  // GyY
      hist[50]=(Wire.read()<<8|Wire.read());  // GyZ
     
      Serial.print("AcX = ");    Serial.print(hist[0]);
      Serial.print(" | AcY = "); Serial.print(hist[10]);
      Serial.print(" | AcZ = "); Serial.print(hist[20]);
      Serial.print(" | GyX = "); Serial.print(hist[30]);
      Serial.print(" | GyY = "); Serial.print(hist[40]);
      Serial.print(" | GyZ = "); Serial.println(hist[50]);
      
      int AxAvg = (hist[1] / 3) + (hist[2] / 3) + (hist[3] / 3);
      int AyAvg = (hist[11] / 3) + (hist[12] / 3) + (hist[13] / 3);
      int AzAvg = (hist[21] / 3) + (hist[22] / 3) + (hist[23] / 3);
      int GxAvg = (hist[31] / 3) + (hist[32] / 3) + (hist[33] / 3);
      int GyAvg = (hist[41] / 3) + (hist[42] / 3) + (hist[43] / 3);
      int GzAvg = (hist[51] / 3) + (hist[52] / 3) + (hist[53] / 3);
      
      if(hist[0] >= 18000 || hist[0] <= -18000){
        digitalWrite(A0, HIGH);
        digitalWrite(A1, HIGH);
        digitalWrite(A2, HIGH);
      }else if(hist[0] >= 12000 || hist[0] <= -12000){
        digitalWrite(A0, HIGH);
        digitalWrite(A1, HIGH);
        digitalWrite(A2, LOW);
      }else if(hist[0] >= 6000 || hist[0] <= -6000){
        digitalWrite(A0, HIGH);
        digitalWrite(A1, LOW);
        digitalWrite(A2, LOW);
      }else{
        digitalWrite(A0, LOW);
        digitalWrite(A1, LOW);
        digitalWrite(A2, LOW);
      }
      
      if(hist[10] >= 18000 || hist[10] <= -18000){
        digitalWrite(10, HIGH);
        digitalWrite(11, HIGH);
        digitalWrite(12, HIGH);
      }else if(hist[10] >= 12000 || hist[10] <= -12000){
        digitalWrite(10, HIGH);
        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
      }else if(hist[10] >= 6000 || hist[10] <= -6000){
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
      }else{
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
      }
      
      if(hist[20] >= 18000 || hist[20] <= -18000){
        digitalWrite(7, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(5, HIGH);
      }else if(hist[20] >= 12000 || hist[20] <= -12000){
        digitalWrite(7, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(5, LOW);
      }else if(hist[20] >= 6000 || hist[20] <= -6000){
        digitalWrite(7, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(5, LOW);
      }else{
        digitalWrite(7, LOW);
        digitalWrite(6, LOW);
        digitalWrite(5, LOW);
      }
      
      
      
      
    }
    
    
    void updateHist(){
       for(int i = 58; i >= 0; i--){
         hist[i + 1] = hist[1];
      }
}
