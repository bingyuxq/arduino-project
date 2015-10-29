#line 1 "AnalogInOutSerial.ino"
#include <OneWire.h>

#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
#include "Arduino.h"
void setup();
void loop();
#line 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);



void setup() {
                                                  
  Serial.begin(9600);
  sensors.begin();
  pinMode(4, INPUT);

}

void loop() {
                              
  Serial.print("DATA@");
  Serial.print(millis());
  Serial.print("--->A0=" );  int sensorValue = analogRead(A0);  Serial.print(sensorValue);
  Serial.print(",A1=" );  sensorValue = analogRead(A1);  Serial.print(sensorValue);
  Serial.print(",A2=" );  sensorValue = analogRead(A2);  Serial.print(sensorValue);
  Serial.print(",D0=" );  sensorValue = digitalRead(4);  Serial.print(sensorValue);
  Serial.print(",TEMP=" );  sensors.requestTemperatures();            
  Serial.println(sensors.getTempCByIndex(0));



    
                               
                                                  
                                                                                                                 
                               
                                                  
                                                                                                                 
                                            
                                                                     
                                                           
    
  delay(10000);
}

