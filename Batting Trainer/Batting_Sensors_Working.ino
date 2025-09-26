/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

MPU9250_asukiaaa mySensor;
char aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic_aX = NULL;
BLECharacteristic* pCharacteristic_y = NULL;
BLECharacteristic* pCharacteristic_z = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
char sensorValues[18];

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        ""
#define CHARACTERISTIC_UUID_aX ""
#define CHARACTERISTIC_UUID_y ""
#define CHARACTERISTIC_UUID_z ""

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  #ifdef _ESP32_HAL_I2C_H_ // For ESP32
    Wire.begin(SDA_PIN, SCL_PIN);
    mySensor.setWire(&Wire);
  #endif

  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
  
  Serial.println("Starting BLE work!");

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_aX = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_aX,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic_y = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_y,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic_z = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_z,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
                    

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic_aX->addDescriptor(new BLE2902());
  pCharacteristic_y->addDescriptor(new BLE2902());
  pCharacteristic_z->addDescriptor(new BLE2902());
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  char aX[8],aY[8],aZ[8],gX[8],gY[8],gZ[8],mX[8],mY[8],mZ[8];

  double floataX;
  float floatX,floatY,floatZ;

  int counter = 0;
  char buff[40];
  if (mySensor.accelUpdate() == 0) {  
      floataX = mySensor.accelX(); 
      
Serial.println(floataX); 
     dtostrf(floataX, 6, 2, buff);
Serial.println(aX);
     
//      //appendFile(SD, "/dataLog.csv", aX);
      strcpy(sensorValues,buff);
//      strcpy(sensorValues,",");
      //appendFile(SD, "/dataLog.csv", ",");    
Serial.println(sensorValues);
Serial.println(); 
//  pCharacteristic_aX->setValue(floataX);
//  pCharacteristic_aX->notify();  
   
      floatY = mySensor.accelY(); 
      dtostrf(floatY, 6, 2, aY); 
      //appendFile(SD, "/dataLog.csv", aY);
  
      //appendFile(SD, "/dataLog.csv", ",");  
    
      floatZ = mySensor.accelZ(); 
      dtostrf(floatZ, 6, 2, aZ); 
      //appendFile(SD, "/dataLog.csv", aZ);
  
      counter = counter + 1;
  }else{
    if (counter != 0){
        //appendFile(SD, "/dataLog.csv", ",");
    }
    
    //appendFile(SD, "/dataLog.csv", "0,0,0");
  }
//Serial.println(counter);
//Serial.print("gyroUpdate");
//Serial.println(mySensor.gyroUpdate());
  if (mySensor.gyroUpdate() == 0) {  
      if (counter != 0){
        //appendFile(SD, "/dataLog.csv", ",");
      }  
      floatX = mySensor.gyroX(); 
      dtostrf(floatX, 6, 2, gX);
      //appendFile(SD, "/dataLog.csv", gX);
 
      //appendFile(SD, "/dataLog.csv", ",");    
    
      floatY = mySensor.gyroY(); 
      dtostrf(floatY, 6, 2, gY); 
      //appendFile(SD, "/dataLog.csv", gY);
  
      //appendFile(SD, "/dataLog.csv", ",");  
    
      floatZ = mySensor.gyroZ(); 
      dtostrf(floatZ, 6, 2, gZ); 
      //appendFile(SD, "/dataLog.csv", gZ);
  
      counter = counter+1;
    }else{
      if (counter != 0){
          //appendFile(SD, "/dataLog.csv", ",");
      }
      //appendFile(SD, "/dataLog.csv", "0,0,0");
    }    

// Serial.println(counter);
//Serial.print("magUpdate");
//Serial.println(mySensor.magUpdate()); 
  if (mySensor.magUpdate() == 0) {   
      if (counter != 0){
        //appendFile(SD, "/dataLog.csv", ",");
      }  
      floatX = mySensor.magX(); 
      dtostrf(floatX, 6, 2, mX);
      //appendFile(SD, "/dataLog.csv", mX);
//  char btNotifyData_X(8) = "aX:")
//  strcpy(btNotifyData_X,aX);
//  pCharacteristic_X->setValue((btNotifyData_X));
//  pCharacteristic_X->notify();
      //appendFile(SD, "/dataLog.csv", ",");    
    
      floatY = mySensor.magY(); 
      dtostrf(floatY, 6, 2, mY); 
      //appendFile(SD, "/dataLog.csv", mY);
//  char btNotifyData_Y(8) = "aY:")
//  strcpy(btNotifyData_Y,aY);
//  pCharacteristic_Y->setValue((btNotifyData_Y));
//  pCharacteristic_Y->notify(); 
      //appendFile(SD, "/dataLog.csv", ",");  
    
      floatZ = mySensor.magZ(); 
      dtostrf(floatZ, 6, 2, mZ); 
      //appendFile(SD, "/dataLog.csv", mZ);      
  }else{
    if (counter != 0){
        //appendFile(SD, "/dataLog.csv", ",");
    }
    //appendFile(SD, "/dataLog.csv", "0,0,0");
  }
  
  
  //appendFile(SD, "/dataLog.csv", " \n");

  
 
  if (deviceConnected) {
    Serial.println(sensorValues);
      pCharacteristic_aX->setValue(floataX);
      pCharacteristic_aX->notify();
      delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
  }
  
  delay(2000);
}
