/*
  main.cpp - The application that runs the automated sprinkler system as 
  part of the Irrigation Automation Project for ESP32 Using both the 
  5 wirevalve project and the watermeter projects in the design.  More at
  https://www.youtube.com/c/jordanrubin6502
  2021 Jordan Rubin.
*/
#include <Arduino.h>
#include <Sprinkler.h>

SPRINKLERSYSTEM sprinklersystem(true);

void setup() {
  Serial.begin(38400);
  Serial.println("\n\n***Water Sprinkler Test Programme***");
  sprinklersystem.addZone(21,"Front Yard");
  sprinklersystem.addZone(13,"Back Yard");
  sprinklersystem.addZone(13,"Test Yard");   // Double GPIO error on 13
  sprinklersystem.addZone(14,"Retro Tech & Electronics");
  sprinklersystem.addZone(11,"This is a really long zone name"); // Length err max 25
  sprinklersystem.addZone(15,"Retro Tech & Electronics");     // Double name error on Garden
  sprinklersystem.addZone(27,"West Garden");
  sprinklersystem.removeZone("Retro Tech & Electronics");    //Delete RT&E
  sprinklersystem.addZone(14,"Garden");   //Garden fits on gap from RT&E INDEX 2 
  sprinklersystem.setDescription("Back Yard","The zone to controld back yard area.");
  sprinklersystem.setDescription("West Garden","Micro irrigation.");
  sprinklersystem.setDescription("West Garden","Micro irrigation system on the west side.");
  sprinklersystem.setDescription("Garden","Micro system on the west side. This is a vrey longer than 50 char string"); // Length err
  sprinklersystem.zoneInfo();
  sprinklersystem.zoneInfo("West Garden");
  sprinklersystem.addValve(16,17,18,true);
  sprinklersystem.valveMaxTravelTime(9);
  sprinklersystem.addMeter(15,true,'g',100,true,.1,false);
  delay(2000);
}
void loop() {
  sprinklersystem.openZone("Front Yard");
  delay(1000);
  sprinklersystem.openZone("Back Yard");
  delay(1000);
  sprinklersystem.openZone("Garden");
  delay(1000);
  sprinklersystem.openZone("West Garden");
  delay(1000);
Serial.println(sprinklersystem.valvePosition());
Serial.println(sprinklersystem.setValve("OPEN"));
Serial.println(sprinklersystem.valveLastDuration("OPEN"));
  sprinklersystem.closeZone("Front Yard");
  delay(1000);
  sprinklersystem.closeZone("Back Yard");
  delay(1000);
  sprinklersystem.closeZone("Garden");
  delay(1000);
  sprinklersystem.closeZone("West Garden"); 
Serial.println(sprinklersystem.valvePosition());  
Serial.println(sprinklersystem.setValve("CLOSED"));
Serial.println(sprinklersystem.valveLastDuration("CLOSED"));
  while(1) {
   delay(500);
   if(sprinklersystem.meterMoved()){
     Serial.print(sprinklersystem.readMeter());
     Serial.print("gallons, in litres -> ");
     Serial.println(sprinklersystem.readMeter('l'));
   }
}
}