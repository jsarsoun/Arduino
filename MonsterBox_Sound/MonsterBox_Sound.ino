#include <Thread.h>
#include <ThreadController.h>
#include <TimerOne.h>
#include <NewPing.h>

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
/*
#define RED_LED 8
//#define BLUE_LED A3
//#define GREEN_LED A4
#define WHITE_LED 5

#define FOG_MACHINE   9   // Connect Digital Pin on Arduino to Relay Module

#define LID_BOUNCER   10   // Connect Digital Pin on Arduino to Relay Module

*/
#define PIR_SENSOR      9 // PIR Input



#define CLK 13       // SPI Clock, shared with SD card
#define MISO 12      // Input data, from VS1053/SD card
#define MOSI 11      // Output data, to VS1053/SD card

#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin



/*#ifdef Adafruit_VS1053_FilePlayer*/
Adafruit_VS1053_FilePlayer musicPlayer = 
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
/*#define music 1
#else
#define music 0
#endif*/

//NewPing sonar(PROX_SENSOR, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int ferocity = 2;

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
/*
class ProximitySensorThread: public Thread
{
public:
  int value;
  int pin;

  void run(){
    // Reads the analog pin, and saves it localy
    value = map(analogRead(pin), 0,1023,0,255);
    runned();
  }
};

class SmokeThread: public Thread
{
  void run(){
    Serial.println("smoke thread");
    // Reads the analog pin, and saves it localy
    digitalWrite(FOG_MACHINE, HIGH);
    delay(3000);
    digitalWrite(FOG_MACHINE, LOW);
    runned();
  }
};
*/
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
/*
ThreadController actionControl = ThreadController();
Thread* lidThread = new Thread();
Thread* lightsThread = new Thread();
Thread* smokeThread = new Thread();
Thread* strobeThread = new Thread();
*/
MotionSensorThread moSensorThread = MotionSensorThread();
/*ProximitySensorThread proxSensorThread = ProximitySensorThread();
SmokeThread smokeTimerThread = SmokeThread();*/
/*StrobeThread strobeTimerThread = StrobeThread();*/

ThreadController controll = ThreadController();

void timerCallback(){
  controll.run();
}
/*
void lid(){
  //Serial.println("lid");
    digitalWrite(LID_BOUNCER, HIGH);
    delay(75);
    digitalWrite(LID_BOUNCER, LOW);  
    delay(random(50,400));
}
*/
void roar(){
  Serial.println("roar");
  /*#ifdef musicplayer*/
  musicPlayer.playFullFile("/ROAR0.mp3");
  /*#endif*/
}

void snore(){
  Serial.println("snore");
  /*#ifdef musicplayer*/
  //Serial.println("snore");
  if(!musicPlayer.playingMusic){
    //musicPlayer.startPlayingFile("/SNORE1.mp3");
  }  
  /*#endif */
}
/*
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
  digitalWrite(RED_LED, fire[fire_hue][0]);
  delay(20);
}

void smoke(){
  Serial.println("smoke");
  digitalWrite(FOG_MACHINE, HIGH);
  delay(500);
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
*/
void setup() {  
  // initialize serial communication:
  Serial.begin(9600);

  /*//pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  //pinMode(BLUE_LED, OUTPUT);
  
  pinMode(WHITE_LED, OUTPUT);
  
  // Setup all the relay Pins
  pinMode(LID_BOUNCER, OUTPUT);
  //pinMode(RED_LEDS,    OUTPUT);
  
  pinMode(FOG_MACHINE, OUTPUT);*/

  pinMode(PIR_SENSOR,   INPUT);

  //pinMode(PROX_SENSOR,   INPUT);

  //digitalWrite(PIR_SENSOR, LOW);

  
  /*#ifdef musicplayer*/
  if (! musicPlayer.begin()) { // initialize the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // Set volume for left, right channels. lower numbers == louder volume!
  
  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)){
    Serial.println(F("DREQ pin is not an interrupt pin"));
  }
  /*#endif*/
  /*
  lidThread->onRun(lid);
  lidThread->setInterval(200);
/*
  lightsThread->onRun(lights);
  lightsThread->setInterval(50);
*//*
  smokeThread->onRun(smoke);
  smokeThread->setInterval(7000);
  
  strobeThread->onRun(strobe);
  strobeThread->setInterval(20);
  
  actionControl.add(lidThread);
  //actionControl.add(lightsThread);
  actionControl.add(smokeThread);
  actionControl.add(strobeThread);*/

  moSensorThread.pin = PIR_SENSOR;
  moSensorThread.setInterval(20);
  controll.add(&moSensorThread);
/*
  smokeTimerThread.setInterval(20000);
  controll.add(&smokeTimerThread);
  */
  /*strobeTimerThread.setInterval(40);
  controll.add(&strobeTimerThread);*/
//
//  proxSensorThread.pin = PROX_SENSOR;
//  proxSensorThread.setInterval(100);
//  controll.add(&proxSensorThread);

  Timer1.initialize(20000);
  Timer1.attachInterrupt(timerCallback);
  Timer1.start();
}


void loop() {  
//Serial.println(moSensorThread.value);
  if(moSensorThread.value){
    musicPlayer.setVolume(0,0);
    if(musicPlayer.currentTrack.name() != "/ROAR0.mp3"){
      musicPlayer.playFullFile("/ROAR0.mp3");
      /*while(musicPlayer.playingMusic){      
        actionControl.run();
      }*/
    }
    /*#else
    actionControl.run();
    #endif*/
  }
  else{
    musicPlayer.setVolume(20,20);
    /*#ifdef musicPlayer*/
    //musicPlayer.startPlayingFile("/SNORE1.mp3");

    while(musicPlayer.playingMusic){
     /* digitalWrite(RED_LED, 256);*/
      //Serial.println("here");
      if(moSensorThread.value){
        
        musicPlayer.stopPlaying();   
      }
      
      delay(25);
    }
    /*#else
    digitalWrite(RED_LED, 256);
    delay(15);
    #endif*/
  }
}