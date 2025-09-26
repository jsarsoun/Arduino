#include <ArduinoTrace.h>

#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
  #define SDA_PIN 21
  #define SCL_PIN 22
#endif

MPU9250_asukiaaa mySensor;

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "FS.h"
#include "SD.h"
#include "SPI.h"

bool doRecord = false;
bool deviceConnected = false;
bool oldDeviceConnected = false;

String readyPos;
String contactPos;
String finishPos;
String sessionName = "";
String clientCommand = "";

float aX,aY,aZ,gX,gY,gZ;
char aSqrt,mDirection,baselineReady,baselineContact,baselineFinish;


uint32_t value = 0;
  
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/


void handleSensorData(const char * sensorValueUpdate){
  
}

void postSensorData(const char * dataPoint){
  
}


void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        //Serial.println("Failed to open file for reading");
        return;
    }

    //Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        //Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        //Serial.println("File written");
    } else {
        //Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        //Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        //Serial.println("Message appended");
    } else {
        //Serial.println("Append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        //Serial.println("File deleted");
    } else {
        //Serial.println("Delete failed");
    }
}

String getSensorPoints(){
  String accelPoints = getAccelPoints();
  String gyroPoints = getGyroPoints();
  String sensorPoints = accelPoints + "," + gyroPoints;
  return sensorPoints;
}

String getAccelPoints(){
    String SensorCSVString;
    
    if (mySensor.accelUpdate() == 0) {
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();
    } 
    
    SensorCSVString = String(aX)+","
      +String(aY)+","
      +String(aZ);

    return SensorCSVString;
}

String getGyroPoints(){
    String SensorCSVString;
    
    if (mySensor.accelUpdate() == 0) {
      gX = mySensor.accelX();
      gY = mySensor.accelY();
      gZ = mySensor.accelZ();
    } 
    
    SensorCSVString = String(gX)+","
      +String(gY)+","
      +String(gZ);

    return SensorCSVString; 
}

void recordSensorPoints(){

  String sensorPoints = getSensorPoints();
  sensorPoints = sensorPoints + ","; 
  sensorPoints = sensorPoints + "\n";
  String recordSession = sessionName + "," + sensorPoints.c_str() + "," + clientCommand;
  

Serial.println(recordSession); 
  appendFile(SD, "/dataLog.csv", recordSession.c_str());

}

///////////////////////////////////BLE SETUP////////////////////////////////////////////
BLEServer* pServer = NULL;

BLECharacteristic* pCharacteristic_Baseline_Ready_Pos = NULL;BLECharacteristic* pCharacteristic_Baseline_Contact_Pos = NULL;BLECharacteristic* pCharacteristic_Baseline_Finish_Pos = NULL;

//BLECharacteristic* pCharacteristic_Sensors_Recordings = NULL;
BLECharacteristic* pCharacteristic_doRecord = NULL;
BLECharacteristic* pCharacteristic_AP = NULL;

const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";

const char* CHARACTERISTIC_UUID_Baseline_Ready = "40579781-73bf-47d8-a4d3-0ef140ae76ea";
const char* CHARACTERISTIC_UUID_Baseline_Contact = "2d855b55-5e71-406d-b62c-09c7714077f8";
const char* CHARACTERISTIC_UUID_Baseline_Finish = "87a53f31-122e-4d27-a69f-9f550bd9aca5";

//#define CHARACTERISTIC_UUID_Sensors_Recordings "cba4ca8d-53ff-451f-bcc6-813486fdc030"

const char* CHARACTERISTIC_UUID_doRecord = "1bed52c4-4b38-40b1-a909-be7d9b0d8895";

const char* CHARACTERISTIC_UUID_AP = "62a02e0e-dce8-459b-bf7e-ca22b245566d";

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }    
};

class characteristicCallbacks: public BLECharacteristicCallbacks {
  
  void onWrite(BLECharacteristic *CHARACTERISTIC_UUID) {
    BLEUUID cUUIDIn = CHARACTERISTIC_UUID->getUUID();    
    std::string chUUIDIn = cUUIDIn.toString();   
    String charaUUIDIn = chUUIDIn.c_str();
Serial.println("onwrite");
    std::string valueIn = CHARACTERISTIC_UUID->getValue();    
    String value = valueIn.c_str();   
    clientCommand = value.substring(0,value.indexOf("|"));
    sessionName = value.substring(value.indexOf("|")+1);

    if (charaUUIDIn == CHARACTERISTIC_UUID_doRecord) {
        //Serial.println("here");
         if (clientCommand == "start_recording"){
           doRecord = true;
         }else if (clientCommand == "stop_recording"){
           doRecord = false;
          }
     }else {        
        recordSensorPoints();
      };
       Serial.flush();    
   }; 

   void onRead(BLECharacteristic *CHARACTERISTIC_UUID) { 
    Serial.println("onread");   
              std::string rvals = "onread";
          //const char rvals = "Hello2";
     CHARACTERISTIC_UUID->setValue(rvals);
     Serial.flush();  
   }; 

   void onNotify(BLECharacteristic *CHARACTERISTIC_UUID) {  
//    Serial.println("onnotify");  
//              std::string rvals = "onnotify";
//          //const char rvals = "Hello2";
//     CHARACTERISTIC_UUID->setValue(rvals);
//     Serial.flush();  
   }; 

   void onStatus(BLECharacteristic *CHARACTERISTIC_UUID) {    
    Serial.println("onstatus");
              std::string rvals = "onstatus";
          //const char rvals = "Hello2";
     CHARACTERISTIC_UUID->setValue(rvals);
     Serial.flush();  
   }; 

};
void setup(){
   
    Serial.begin(115200);

    while(!Serial);    
    
    // Create the BLE Device
    BLEDevice::init("ESP32");
    BLEDevice::setMTU(50);
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service

    BLEService *pService = pServer->createService(SERVICE_UUID);
  
    ////////////////////// Create BLE Characteristics
    //////////////////////SENSORS                  

    /////////////////////Baselines                 
    pCharacteristic_Baseline_Ready_Pos = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_Baseline_Ready,
                      BLECharacteristic::PROPERTY_READ  |
                      BLECharacteristic::PROPERTY_WRITE  |
                      
                      BLECharacteristic::PROPERTY_INDICATE
                    );

    pCharacteristic_Baseline_Finish_Pos = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_Baseline_Finish,
                      BLECharacteristic::PROPERTY_READ  |
                      BLECharacteristic::PROPERTY_WRITE  |
                     
                      BLECharacteristic::PROPERTY_INDICATE
                    );

    pCharacteristic_Baseline_Contact_Pos = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_Baseline_Contact,
                      BLECharacteristic::PROPERTY_READ  |
                      BLECharacteristic::PROPERTY_WRITE  |
                      
                      BLECharacteristic::PROPERTY_INDICATE                    );
                    
    
    ////////////////MISC.
    pCharacteristic_doRecord = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_doRecord,
                      BLECharacteristic::PROPERTY_READ  |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );      
    pCharacteristic_doRecord->setCallbacks(new characteristicCallbacks());  
                                             
    /////////////////////////AP
    pCharacteristic_AP = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_AP,
                      BLECharacteristic::PROPERTY_READ  |
                      BLECharacteristic::PROPERTY_WRITE  |
                      
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
    // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
    // Create a BLE Descriptor
    
//    pCharacteristic_Sensors_Recordings->addDescriptor(new BLE2902());
    
    pCharacteristic_Baseline_Ready_Pos->addDescriptor(new BLE2902());
    pCharacteristic_Baseline_Contact_Pos->addDescriptor(new BLE2902());
    pCharacteristic_Baseline_Finish_Pos->addDescriptor(new BLE2902()); 
      
    pCharacteristic_doRecord->addDescriptor(new BLE2902());
    
    pCharacteristic_AP->addDescriptor(new BLE2902());     
    // Start the service
    pService->start();
  
    // Start advertising
  
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
 
    
    mySensor.beginAccel();
    mySensor.beginGyro();
    //mySensor.beginMag();
    
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    if(deleteFile){
      deleteFile(SD, "/dataLog.csv");
    }
    
}


void loop() {
Serial.println(doRecord);
    if(doRecord){
       Serial.print("sensorValues ");
       Serial.println(clientCommand);
std::string rvals = getSensorPoints().c_str();
Serial.println(rvals);
pCharacteristic_doRecord->setValue(String(rvals)); 
pCharacteristic_doRecord->notify(); 
    
    }

// std::string rvals = "in loop";
//pCharacteristic_doRecord->setValue(rvals); 
//pCharacteristic_doRecord->notify(); 

Serial.flush(); 
 
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      //Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {      
      
      oldDeviceConnected = deviceConnected;
  }

    
   delay(1000);

}
  
