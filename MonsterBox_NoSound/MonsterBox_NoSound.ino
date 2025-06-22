#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>
#include <NewPing.h>


#define RED_LED 8

#define WHITE_LED 5

#define FOG_MACHINE   9   // Connect Digital Pin on Arduino to Relay Module

#define LID_BOUNCER   10   // Connect Digital Pin on Arduino to Relay Module


#define PIR_SENSOR      11 // PIR Input
#define PIR_SENSOR_OUT  12

/*
#define PROX_SENSOR   11 // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN      12 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTA0NCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(PROX_SENSOR, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

*/
//unsigned int ferocity = 2;

class MotionSensorThread: public Thread
{
public:
  int value;
  int pin;

  void run(){
    // Reads the analog pin, and saves it localy
    value = digitalRead(pin);

    runned();
  }
};

/*void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
  }
  // Don't do anything here!
}*/

/*class ProximitySensorThread: public Thread
{
public:
  int value;
  int pin;

  void run(){
    
    value = sonar.ping_median(5);
    runned();
  }
};*/

class SmokeThread: public Thread
{
  void run(){
    Serial.println("smoke thread");
    // Reads the analog pin, and saves it localy
    digitalWrite(FOG_MACHINE, HIGH);
    delay(100);
    digitalWrite(FOG_MACHINE, LOW);
    runned();
  }
};

/*class StrobeThread: public Thread
{
  void run(){
    Serial.println("strobe");
    // Reads the analog pin, and saves it localy
    digitalWrite(A5, HIGH);

    delay(700);
    digitalWrite(A5, LOW);
    delay(20);
    runned();
  }
};*/

ThreadController actionControl = ThreadController();
Thread* lidThread = new Thread();
Thread* lightsThread = new Thread();
Thread* smokeThread = new Thread();
Thread* strobeThread = new Thread();

MotionSensorThread moSensorThread = MotionSensorThread();
//ProximitySensorThread proxSensorThread = ProximitySensorThread();
SmokeThread smokeTimerThread = SmokeThread();
/*StrobeThread strobeTimerThread = StrobeThread();*/

ThreadController controll = ThreadController();

void timerCallback(){
  controll.run();
}

void lid(){
  //Serial.println("lid");
    digitalWrite(LID_BOUNCER, HIGH);
    delay(random(50,150));
    digitalWrite(LID_BOUNCER, LOW);  
    delay(50);
}


void lights(){
  //Serial.println("lights");

  //int fire[][3] = {{161,1,0},{218,31,5},{243,60,4},{254,101,13},{255,193,31},{255,247,93},{256,0,0},{255,255,0},{256,0,150}};
  int fire[][3] = {{256,0,0}};
  int fire_hue = random(0,sizeof(fire)-1);
  
  digitalWrite(RED_LED, 256);
//  digitalWrite(GREEN_LED, fire[fire_hue][1]);
//  digitalWrite(BLUE_LED, fire[fire_hue][2]);
  delay(random(50,150));
  digitalWrite(RED_LED, 0);  
//  digitalWrite(BLUE_LED, 0); 
//  digitalWrite(GREEN_LED, 0); 
  /*delay(random(50,150));
  digitalWrite(RED_LED, fire[fire_hue][0]);*/
  delay(20);
}

void smoke(){
  Serial.println("smoke");
  digitalWrite(FOG_MACHINE, HIGH);
  delay(100);
  digitalWrite(FOG_MACHINE, LOW);
}

void strobe(){
  Serial.println("strobe");
  // Reads the analog pin, and saves it localy
  digitalWrite(WHITE_LED, HIGH);

  delay(20);
  digitalWrite(WHITE_LED, LOW);
  delay(5);
}

void setup() {  
  // initialize serial communication:
  Serial.begin(9600);

  //pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  //pinMode(BLUE_LED, OUTPUT);
  
  pinMode(WHITE_LED, OUTPUT);
  
  // Setup all the relay Pins
  pinMode(LID_BOUNCER, OUTPUT);
  //pinMode(RED_LEDS,    OUTPUT);
  
  pinMode(FOG_MACHINE, OUTPUT);

  pinMode(PIR_SENSOR,   INPUT);
  pinMode(PIR_SENSOR_OUT,   OUTPUT);

  //pinMode(PROX_SENSOR,  INPUT);

  
  lidThread->onRun(lid);
  lidThread->setInterval(200);
/*
  lightsThread->onRun(lights);
  lightsThread->setInterval(50);
*/
  smokeThread->onRun(smoke);
  smokeThread->setInterval(20);
  
  strobeThread->onRun(strobe);
  strobeThread->setInterval(150);
  
  actionControl.add(lidThread);
  //actionControl.add(lightsThread);
  actionControl.add(smokeThread);
  actionControl.add(strobeThread);
  
  moSensorThread.pin = PIR_SENSOR;
  moSensorThread.setInterval(100);
  controll.add(&moSensorThread);
 
  /*proxSensorThread.pin = PROX_SENSOR;
  proxSensorThread.setInterval(200);
  controll.add(&proxSensorThread);*/

  smokeTimerThread.setInterval(30000);
  controll.add(&smokeTimerThread);
 /* 
  strobeTimerThread.setInterval(40);
  controll.add(&strobeTimerThread);
*/

  Timer1.initialize(20000);
  Timer1.attachInterrupt(timerCallback);
  Timer1.start();
}


void loop() {  
  digitalWrite(PIR_SENSOR_OUT, moSensorThread.value);
  Serial.println(moSensorThread.value);
  if(moSensorThread.value){
    
    actionControl.run();
  }
  else{
      digitalWrite(RED_LED, 256);
      delay(15);
  }
}