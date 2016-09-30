#ifdef ARDUINO_ARCH_ARC32
#include <CurieEEPROM.h>
#include <CurieBLE.h>
#else
#include <EEPROM.h>
#endif

#include <SoftwareSerial.h>
#include <Servo.h>
/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
 int baudrate = 9600;
#include <BowlerCom.h>
int txPin =24;
int rxPin =25;
BowlerCom com;
SoftwareSerial mySerial(rxPin,txPin); // RX, TX

void setup() {

  mySerial.begin(baudrate);
  Serial.begin(baudrate);
#  if  defined(__ARDUINO_ARC__)
  while (!Serial ||!mySerial );    // wait for the serial port to open
#endif
  com.begin(&Serial);
  
  startDebugPint(&mySerial);
  setPrintLevelErrorPrint();
  
  
  println_W("Adding namespaces");
  // ensure DyIO leaves these pins alone
  _EEWriteMode(txPin, IS_DEBUG_TX);
  _EEWriteMode(rxPin, IS_DEBUG_RX);
  // start dyio framework and load DyIO namespace
  com.addDyIO();  

  com.addDyIOPID();

  println_E("Starting DyIO");
 
}

void loop() {
  com.server();
}
