//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiAP.h>

//WiFiServer server(80);

// Set these to your desired credentials.
const char *ssid = "ArduinoSensor";
const char *password = "";

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

//#include "FS.h"
//#include "SD.h"
//#include "SPI.h"

bool doRecord = false;
bool deviceConnected = false;
bool oldDeviceConnected = false;

const char* sessionName;
const char* clientCommand;

char sensorAccCSV[5];
char sensorGyroCSV[5];
char sensorPointsCSV[250];

char baselineReady,baselineContact,baselineFinish;



void getAccPoints(){
    float aX,aY,aZ;
    char buff[5];
    
    sensorAccCSV[0] = '\0';
    Serial.print("mySensor.accelUpdate() ");
    Serial.println(mySensor.accelUpdate());
    if (mySensor.accelUpdate() == 0) {
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();      
      
      dtostrf(aX, 6, 2, buff);
      strcat(sensorAccCSV,buff);
      strcat(sensorAccCSV,",");
      dtostrf(aY, 6, 2, buff);
      strcat(sensorAccCSV,buff);
      strcat(sensorAccCSV,",");
      dtostrf(aZ, 6, 2, buff);
      strcat(sensorAccCSV,buff);
       Serial.print("getAccPoints ");
        Serial.println(sensorAccCSV);
  
  
    } 
    
}

void getGyroPoints(){
    float gX,gY,gZ;
    char buff[5];
    
    sensorGyroCSV[0] = '\0';
    Serial.print("mySensor.gyroUpdate() ");
    Serial.println(mySensor.gyroUpdate());
    if (mySensor.gyroUpdate() == 0) {
      gX = mySensor.gyroX();
      gY = mySensor.gyroY();
      gZ = mySensor.gyroZ();

      dtostrf(gX, 6, 2, buff);
      strcat(sensorGyroCSV,buff);
      strcat(sensorGyroCSV,",");
      dtostrf(gY, 6, 2, buff);
      strcat(sensorGyroCSV,buff);
      strcat(sensorGyroCSV,",");
      dtostrf(gZ, 6, 2, buff);
      strcat(sensorGyroCSV,buff);
      Serial.print("sensorGyroCSV ");
      Serial.println(sensorGyroCSV);
    }
     
}

void getSensorPoints(){
  sensorPointsCSV[0] = '\0';
  getAccPoints();
  delay(100);
  getGyroPoints();
  strcat(sensorPointsCSV,sensorAccCSV);
  strcat(sensorAccCSV,",");
  strcat(sensorPointsCSV,sensorGyroCSV);
  Serial.print("sensorPointsCSV getSensorPoints ");
  Serial.println(sensorPointsCSV);

}
void recordSensorPoints(){
  clearSensorPoints();
  getSensorPoints();
  Serial.print("sensorPointsCSV1 recordSensorPoints ");  
Serial.println(sensorPointsCSV);
  strcat(sensorPointsCSV,",");
  strcat(sensorPointsCSV,"start_recording");
Serial.print("sensorPointsCSV2 recordSensorPoints ");  
Serial.println(sensorPointsCSV);

}

void clearSensorPoints(){
  Serial.print("sensorPointsCSV1 clearSensorPoints ");  
Serial.println(sensorPointsCSV);
  sensorAccCSV[0]=0;
  sensorGyroCSV[0]=0;
  sensorPointsCSV[0]=0;
  Serial.print("sensorPointsCSV2 clearSensorPoints ");  
Serial.println(sensorPointsCSV);
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

//#define CHARACTERISTIC_UUID_Sensors_Recordings "0"

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
    //std::string chUUIDIn = cUUIDIn.toString();   
    //char charaUUIDIn = chUUIDIn;
Serial.println("onwrite");
    std::string valueIn = CHARACTERISTIC_UUID->getValue(); 
    
    
    clientCommand = valueIn.substr(0,valueIn.find_first_of("|")).c_str();
    sessionName = valueIn.substr(valueIn.find_first_of("|")+1,valueIn.length()).c_str();
    Serial.println(clientCommand); 
    //Serial.println(sessionName); 

    
        Serial.print("onWrite1 doRecord ");
        Serial.println(doRecord);
         if (String(clientCommand)=="start_recording"){
           doRecord = true;
         }else{
           doRecord = false;
         }
       Serial.print("onWrite2 doRecord ");
        Serial.println(doRecord);    
   }; 

   void onRead(BLECharacteristic *CHARACTERISTIC_UUID) { 
    //Serial.println("onread");   
  
   }; 

   void onNotify(BLECharacteristic *CHARACTERISTIC_UUID) {  
    //Serial.println("onnotify");  
//              std::string rvals = "onnotify";
//          //const char rvals = "Hello2";
//     CHARACTERISTIC_UUID->setValue(rvals);
//     Serial.flush();  
   }; 

   void onStatus(BLECharacteristic *CHARACTERISTIC_UUID) {    
    //Serial.println("onstatus");
 
   }; 

};
void setup(){
   
    Serial.begin(115200);

    while(!Serial); 
//
//    //Create the WiFi Access Point
//    WiFi.softAP(ssid);
//    IPAddress myIP = WiFi.softAPIP();
//    Serial.print("AP IP address: ");
//    Serial.println(myIP);
//    server.begin();
//    
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

    #ifdef _ESP32_HAL_I2C_H_ // For ESP32
      Wire.begin(SDA_PIN, SCL_PIN);
      mySensor.setWire(&Wire);
    #endif
    
    mySensor.beginAccel();
    mySensor.beginGyro();
    //mySensor.beginMag();
    
}


void loop() {
//  WiFiClient client = server.available();   // listen for incoming clients
//  if (client) {                             // if you get a client,
//    //Serial.println("New Client.");           // print a message out the serial port
//    String currentLine = "";                // make a String to hold incoming data from the client
//    while (client.connected()) {            // loop while the client's connected
//      if (client.available()) {             // if there's bytes to read from the client,
//        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
//        if (c == '\n') {                    // if the byte is a newline character
//
//          // if the current line is blank, you got two newline characters in a row.
//          // that's the end of the client HTTP request, so send a response:
//          if (currentLine.length() == 0) {
//            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
//            // and a content-type so the client knows what's coming, then a blank line:
//            client.println("HTTP/1.1 200 OK");
//            client.println("Content-type:text/html");
//            client.println();
//
//            // the content of the HTTP response follows the header:
//            //client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
//            //client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
//
//            // The HTTP response ends with another blank line:
//            client.println();
//            // break out of the while loop:
//            break;
//          } else {    // if you got a newline, then clear currentLine:
//            currentLine = "";
//          }
//        } else if (c != '\r') {  // if you got anything else but a carriage return character,
//          currentLine += c;      // add it to the end of the currentLine
//        }
//
//        // Check to see if the client request was "GET /H" or "GET /L":
////        if (currentLine.endsWith("GET /H")) {
////          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
////        }
////        if (currentLine.endsWith("GET /L")) {
////          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
////        }
//      }
//    }
//    // close the connection:
//    client.stop();
//    //Serial.println("Client Disconnected.");
//  }
Serial.print("doRecord loop1 ");
        Serial.println(String(doRecord));
    if(doRecord == true){
        recordSensorPoints();
        
//pCharacteristic_doRecord->setValue(svals); 
//pCharacteristic_doRecord->notify(); 
    
    }
Serial.print("doRecord loop2 ");
        Serial.println(String(doRecord));
// std::string rvals = "in loop";
//pCharacteristic_doRecord->setValue(rvals); 
//pCharacteristic_doRecord->notify(); 

 
 
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
  
