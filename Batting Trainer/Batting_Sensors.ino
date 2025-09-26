
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

Adafruit_MPU6050 mpu;

unsigned long theTime;
long timer = 0;
const int chipSelect = 4;

void setup() {
  pinMode(3,OUTPUT);

  Serial.begin(115200);
//  while(!Serial);
//  Serial.print("Initializing SD card...");
//  if (Serial.available()) {
//     //see if the card is present and can be initialized:
//    if (!SD.begin(chipSelect)) {
//      Serial.println("Card failed, or not present");
//      // don't do anything more:
//      while (1);
//    }
//    Serial.println("card initialized.");
//  } else("serial not available");
    // Try to initialize!
//  if (!mpu.begin()) {
//    Serial.println("Failed to find MPU6050 chip");
//    while (1) {
//      delay(10);
//    }
//  } 
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); 
   
}

void loop() {
    digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
    //File dataFile = SD.open("datalog.csv", FILE_WRITE);
    
    theTime = millis();
    
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    
    Serial.print(theTime);
    Serial.print(",");
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.print(a.acceleration.z);
    Serial.print(",");
    Serial.print(g.gyro.x);      
    Serial.print(",");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print(g.gyro.z);
    Serial.println();      
  
//    if (dataFile) {
//      dataFile.print(theTime);
//      dataFile.print(",");
//      dataFile.print(String(aX));
//      dataFile.print(",");
//      dataFile.print(String(aY));
//      dataFile.print(",");
//      dataFile.print(String(aZ));
//      dataFile.print(",");
//      dataFile.print(String(aSqrt));
//      dataFile.println();
//      
//    } else{
//      Serial.print("Failed to open dataFile");
//    }
//    dataFile.close();
    delay(250);
    digitalWrite(3, LOW); 
    delay(200);  
    
    
//    
//    if (mySensor.gyroUpdate() == 0) {
//      gX = mySensor.gyroX();
//      gY = mySensor.gyroY();
//      gZ = mySensor.gyroZ();
//      Serial.println("gyroX: " + String(gX));
//      Serial.println("gyroY: " + String(gY));
//      Serial.println("gyroZ: " + String(gZ));
//    } else {
//      Serial.println("Cannot read gyro values");
//    }
//  
//    if (mySensor.magUpdate() == 0) {
//      mX = mySensor.magX();
//      mY = mySensor.magY();
//      mZ = mySensor.magZ();
//      mDirection = mySensor.magHorizDirection();
//      Serial.println("magX: " + String(mX));
//      Serial.println("maxY: " + String(mY));
//      Serial.println("magZ: " + String(mZ));
//      Serial.println("horizontal direction: " + String(mDirection));
//    } else {
//      Serial.println("Cannot read mag values");
//    }
//  
//    Serial.println("at " + String(millis()) + "ms");
//    Serial.println(""); // Add an empty line
    
    // if the file is available, write to it:
//    if (dataFile) {
//      dataFile.print(theTime);
//      dataFile.print(mpu6050.getAccX());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getAccY());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getAccZ());
//    
//      dataFile.print(",");
//      dataFile.print(mpu6050.getGyroX());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getGyroY());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getGyroZ());
//    
//      dataFile.print(",");
//      dataFile.print(mpu6050.getAccAngleX());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getAccAngleY());
//      dataFile.print(",");
//    
//      dataFile.print(mpu6050.getGyroAngleX());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getGyroAngleY());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getGyroAngleZ());
//      dataFile.print(",");
//      
//      dataFile.print(mpu6050.getAngleX());
//      dataFile.print(",");
//      dataFile.print(mpu6050.getAngleY());
//      dataFile.print(",");
//      dataFile.println(mpu6050.getAngleZ());
//      
//      dataFile.close();      
//    }
//    // if the file isn't open, pop up an error:
//    else {
//      Serial.println("error opening datalog.txt");
//    }
//    timer = millis();
//   } 
  }
  
