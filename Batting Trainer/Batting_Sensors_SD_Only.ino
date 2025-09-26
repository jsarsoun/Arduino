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
  String sensorPoints = accelPoints + ", " + gyroPoints;
  return sensorPoints;
}

String getAccelPoints(){
    String SensorCSVString;
    
    if (mySensor.accelUpdate() == 0) {
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();
    } 
    
    SensorCSVString = String(aX)+", "
      +String(aY)+", "
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
    
    SensorCSVString = String(gX)+", "
      +String(gY)+", "
      +String(gZ);

    return SensorCSVString; 
}

void recordSensorPoints(){

  String sensorPoints = getSensorPoints();
  sensorPoints = sensorPoints + ","; 
  sensorPoints = sensorPoints + "\n";
  String recordSession = sessionName + "," + sensorPoints.c_str() + ", " + clientCommand;
Serial.println(recordSession); 
  appendFile(SD, "/dataLog.csv", recordSession.c_str());
}

///////////////////////////////////BLE SETUP////////////////////////////////////////////
BLEServer* pServer = NULL;

BLECharacteristic* pCharacteristic_Baseline_Ready_Pos = NULL;BLECharacteristic* pCharacteristic_Baseline_Contact_Pos = NULL;BLECharacteristic* pCharacteristic_Baseline_Finish_Pos = NULL;

//BLECharacteristic* pCharacteristic_Sensors_Recordings = NULL;
BLECharacteristic* pCharacteristic_doRecord = NULL;
BLECharacteristic* pCharacteristic_AP = NULL;

const char* SERVICE_UUID = "";

const char* CHARACTERISTIC_UUID_Baseline_Ready = "";
const char* CHARACTERISTIC_UUID_Baseline_Contact = "";
const char* CHARACTERISTIC_UUID_Baseline_Finish = "";

//#define CHARACTERISTIC_UUID_Sensors_Recordings ""

const char* CHARACTERISTIC_UUID_doRecord = "";

const char* CHARACTERISTIC_UUID_AP = "";

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
        recordSensorPoints()
      };         
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
    //Serial.println("Waiting a client connection to notify...");    
//  
//    #ifdef _ESP32_HAL_I2C_H_ // For ESP32
//      Wire.begin(SDA_PIN, SCL_PIN);
//      mySensor.setWire(&Wire);
//    #endif
//  
    
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
       recordSensorPoints();
//       String sensorValues;
//       sensorValues = getSensorPoints();
//       appendFile(SD, "/dataLog.csv",sensorValues.c_str());    
      
    }
    
//    String CSVString;
//    String SensorCSVString;
//    float floataX,floataY,floataZ;
//    
//    floataX = mySensor.accelX();     
//
//    floataY = mySensor.accelY(); 
//    
//    floataZ = mySensor.accelZ(); 
//    
//    SensorCSVString = String(floataX)+", "
//      +String(floataY)+", "
//      +String(floataZ);
//
// Serial.println(SensorCSVString);
  //Serial.println(doRecord);
//  const char* sensorValues = "";   
//  String SensorCSVString = "";
//  int counter = 0;

//  if(doRecord){
    
//    if (mySensor.accelUpdate() == 0) { 
//       getAccelPoints();
//          
//       counter = counter + 1;
//
//       sensorValues = SensorCSVString.c_str();
//       appendFile(SD, "/dataLog.csv", sensorValues);
//    }

//    if (mySensor.gyroUpdate() == 0) {
//      if(counter > 0){
//        SensorCSVString = SensorCSVString + ", "; 
//      }
//       
//      //////////////////////gX 
//        floatgX = mySensor.gyroX(); 
//        dtostrf(floatgX, 6, 2, buff);     
//        
//      
//        //////////////////////gY 
//        floatgY = mySensor.gyroY(); 
//        dtostrf(floatgY, 6, 2, buff);
//        
//      
//        //////////////////////gZ 
//        floatgZ = mySensor.gyroZ(); 
//        dtostrf(floatgZ, 6, 2, buff);
//        
//    
//        counter = counter + 1;
//
//        SensorCSVString = SensorCSVString 
//          +floatgX+", "
//          +floatgY+", "
//          +floatgZ;
//         
//        SensorCSVString = SensorCSVString + "\n"; 
//        sensorValues = SensorCSVString.c_str();
//
//        counter++;
//           
//    }
//    if(counter > 0){
//      writeFile (SD, "/dataLog.csv", sensorValues);
//    }
//    counter = 0;
//Serial.println(counter);
//  }  
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
  
