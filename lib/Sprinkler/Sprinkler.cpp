/*
  Sprinkler.cpp - Functions and libraries for the automated sprinkler system as 
  part of the Irrigation Automation Project for ESP32 Using both the 
  5 wirevalve project and the watermeter projects in the design.  More at
  https://www.youtube.com/c/jordanrubin6502
  2021 Jordan Rubin.
*/
#include <Arduino.h>
#include <Sprinkler.h>

//CONSTRUCTOR -----------------------------------------------------------------]
//SPRINKLERSYSTEM SPRINKLERSYSTEM(bool test)
SPRINKLERSYSTEM::SPRINKLERSYSTEM(bool test)
{
  //Nothing for now 
}
// ----------------------------------------------------------------------------]

//CONSTRUCTOR -----------------------------------------------------------------]
SPRINKLERSYSTEM::Zone::Zone(int pin, const char* name)
{ 
  this->gpio = pin;
  this->thisname = name;
  this->enabled = false;
  this->description = "unset";
  pinMode(gpio,OUTPUT);
}
// ----------------------------------------------------------------------------]

// FUNCTION - [addMeter] - [Returns the current version from the Object--------]
void SPRINKLERSYSTEM::addMeter(int gpioPin, bool usePullup, char measure, long debounceDelay, bool useSpiffs, double increment, bool verbose){
  //WATERMETER WATERMETER(SignalGPIOpin,useInternalPullupResistor,measure[g|l],metervalue)
  flowmeter = new WATERMETER(gpioPin,usePullup,measure,debounceDelay,useSpiffs,increment,verbose);
  flowmeter->setMeter(32.2);
  Serial.println(flowmeter->initFilesys());
}
// ----------------------------------------------------------------------------]

// FUNCTION - [addValve] - [Returns the current version from the Object--------]
void SPRINKLERSYSTEM::addValve(int relaygpio, int opengpio, int closedgpio, bool usepullup){
  thisvalve = new FIVEWIREVALVE(relaygpio,opengpio,closedgpio,usepullup);
}
// ----------------------------------------------------------------------------]

// FUNCTION - [addZone] - [Returns the current version from the Object---------]
void SPRINKLERSYSTEM::addZone(int gpio, const char* zonename){
  if (strlen(zonename) > 25){
    Serial.println(F("addZone::Zone Name limited to 25 Characters"));
    return;
  }  
  for(int i=0; i< maxZones; i++){
     if (storedZones[i]){continue;}
     else {
      int myIndex = this->getIndex(zonename);
      if (myIndex >=0 && myIndex <=maxZones+1) {
          Serial.print(F("addZone::Error adding ")); Serial.print(zonename);
          Serial.println(F(" name in use...")); 
          return;
      }
      myIndex = this->getIndex(gpio);
      if (myIndex >=0 && myIndex <=maxZones+1) {
          Serial.print(F("addZone::Error adding ")); Serial.print(gpio);
          Serial.print(F(" as ")); Serial.print(zonename);
          Serial.println(F(" GPIO in use..."));
          return;
      }
      Serial.print("addZone::Adding Solenoid on GPIO "); Serial.print(gpio);
      Serial.print(" as "); Serial.println(zonename);
      storedZones[i] = new Zone(gpio,zonename);
      digitalWrite(storedZones[i]->gpio,HIGH);
      break;
    }
  }
}
// ----------------------------------------------------------------------------]

// FUNCTION - [meterMoved] - [Returns the current version from the Object------]
bool SPRINKLERSYSTEM::meterMoved(void){
  bool status;
  status = flowmeter->checkUpdate();
  return status;
}
 // ----------------------------------------------------------------------------] 

// FUNCTION - [closeZone] - [Returns the current version from the Object-------]
void SPRINKLERSYSTEM::closeZone(const char* name){
  int myIndex = this->getIndex(name);
  if (myIndex >=0 && myIndex <=maxZones+1) {
    Serial.print(F("closeZone::Closing Zone ")); Serial.println(myIndex);
    digitalWrite(storedZones[myIndex]->gpio,HIGH);
    storedZones[myIndex]->open = false;
  }
  else {
    Serial.print(F("closeZone::Invalid Zone ")); Serial.println(name); 
  }  
}
// ----------------------------------------------------------------------------]

// PRIVATE - [getIndex] - [xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx---------]
int SPRINKLERSYSTEM::getIndex(const char* name){
  int returnIndex = 99;
  for (int i=0; i<sizeof storedZones/sizeof storedZones[0]; i++) {
    if(storedZones[i]){
      if (strcmp(storedZones[i]->thisname,name)==0){ 
        returnIndex = i;
        return returnIndex;
      }
    }
  }
  return returnIndex;
}
// ----------------------------------------------------------------------------]

// PRIVATE - [getIndex] - [xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx---------]
int SPRINKLERSYSTEM::getIndex(int gpio){
  int returnIndex = 99;
  for (int i=0; i<sizeof storedZones/sizeof storedZones[0]; i++) {
    if(storedZones[i]){
      if (storedZones[i]->gpio == gpio){ 
        returnIndex = i;
        return returnIndex;
      }
    }
  }
  return returnIndex;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [openZone] - [Returns the current version from the Object--------]
void SPRINKLERSYSTEM::openZone(const char* name){
  int myIndex = this->getIndex(name);
  if (myIndex >=0 && myIndex <=maxZones+1) {
    Serial.print(F("openZone::Opening Zone ")); Serial.println(myIndex);
    digitalWrite(storedZones[myIndex]->gpio,LOW);
    storedZones[myIndex]->open = true;
  }
  else {
    Serial.print(F("openZone::Invalid Zone ")); Serial.println(name); 
  }  
}
// ----------------------------------------------------------------------------]

// FUNCTION - [readmeter] - [Returns the current version from the Object------]
double SPRINKLERSYSTEM::readMeter(void){
  double val;
  val = flowmeter->readOut();
  return val;
}
 // ----------------------------------------------------------------------------] 

// FUNCTION - [readmeter] - [Returns the current version from the Object------]
double SPRINKLERSYSTEM::readMeter(char type){
  double val;
  val = flowmeter->readOut(type);
  return val;
}
 // ----------------------------------------------------------------------------] 

// FUNCTION - [removeZone] - [Returns the current version from the Object------]
void SPRINKLERSYSTEM::removeZone(const char* name){
  int myIndex = this->getIndex(name);
  if (myIndex >=0 && myIndex <=maxZones+1) {
    Serial.print(F("removeZone::Removing Zone ")); Serial.print(myIndex);
    Serial.print(F(" ")); Serial.println(name);
    storedZones[myIndex] = NULL; 
  }
  else {
    Serial.print(F("removeZone::Invalid Zone ")); Serial.println(name); 
  }  
}
// ----------------------------------------------------------------------------]

// FUNCTION - [setDescription] - [xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx---------]
void SPRINKLERSYSTEM::setDescription(const char* name, const char* description){
  int myIndex = this->getIndex(name);
  if (myIndex >=0 && myIndex <=maxZones+1) {  
    Serial.print("Setting description on "); Serial.println(myIndex);
    if (strlen(description) > 60){
      Serial.println("ERROR: Description limited to 60 Characters");
      return;
    }
    storedZones[myIndex]->description=description;
  }
  else {
    Serial.println("Invalid Zone name");
  }
}
// ----------------------------------------------------------------------------]

// FUNCTION - [setValve] - [Returns the current version from the Object--------]
char* SPRINKLERSYSTEM::setValve(char* value){
 char* result = thisvalve->setValvePosition(value);
 return result;
}
// ----------------------------------------------------------------------------]

// FUNCTION - [valveLastDuration] - [Returns the position of the valve---------]
int SPRINKLERSYSTEM::valveLastDuration(char* event){
  return thisvalve->getLastDuration(event);
}
// ----------------------------------------------------------------------------]

// FUNCTION - [valveMaxTravelTime] - [Returns the position of the valve--------]
int SPRINKLERSYSTEM::valveMaxTravelTime(void){
  return thisvalve->getMaxTravelTime();
}
// ----------------------------------------------------------------------------]

// FUNCTION - [valveMaxTravelTime] - [Returns the position of the valve--------]
void SPRINKLERSYSTEM::valveMaxTravelTime(int delay){
  thisvalve->setMaxTraveltime(delay);
}
// ----------------------------------------------------------------------------]

// FUNCTION - [valvePosition] - [Returns the position of the valve-------------]
char* SPRINKLERSYSTEM::valvePosition(void){
  return thisvalve->getValvePosition();
}
// ----------------------------------------------------------------------------]

// FUNCTION - [zoneInfo] - [Returns the current version from the Object--------]
void SPRINKLERSYSTEM::zoneInfo(){
  char buf[200];
  Serial.println("\n\n***ZONE INFO***");
  sprintf(buf,"%-8s %-8s %-18s %-18s\n","INDEX","GPIO","NAME","DESCRIPTION");
  Serial.print(buf);
  for (int i=0; i<sizeof storedZones/sizeof storedZones[0]; i++) {
    buf[0] = '\0';
    char iindex[3]; 
    char igpio[3];
    if(storedZones[i]){
     itoa(i,iindex,10);
     itoa(storedZones[i]->gpio,igpio,10);
     sprintf(buf,"%-8s %-8s %-18s %-18s\n",iindex,igpio,storedZones[i]->thisname,storedZones[i]->description);
     Serial.print(buf);
    }
  }
}
// ----------------------------------------------------------------------------]

// FUNCTION - [zoneInfo] - [Returns the current version from the Object--------]
void SPRINKLERSYSTEM::zoneInfo(const char* name){
  int myIndex = this->getIndex(name);
  if (myIndex >=0 && myIndex <=maxZones+1) {
    Serial.print("\n\n***Zone info for "); Serial.print(name);Serial.println("***");
    char buf[200];
    char igpio[3];
    const char *enabled = "FALSE";
    if (storedZones[myIndex]->enabled ==1){enabled = "TRUE";}
    itoa(storedZones[myIndex]->gpio,igpio,10);
    sprintf(buf,"%-11s %-11s\n%-11s %-11s\n%-11s %-11s\n%-11s %-11s\n","GPIO",igpio,"ENABLED",enabled,"STATE","VALUE","DESCIPTION",storedZones[myIndex]->description);
    Serial.print(buf);
  }
  else {
    Serial.println("Invalid Zone name");
  }
}
// ----------------------------------------------------------------------------]
