/* Servo Check

  This is a simple program to check the physical operation of the continuous
  turn servos for the 3 omni-wheel line follower bot (to be) included in the
  asmac display.
  ---> https://github.com/WCRSyyc/omni3-follower/

  There seems to be some interaction between a usb connection and the servo
  software.  With the arduino connected to a usb cable for power, only the
  backward direction commands are working.  The foward directions all result in
  near zero rotation.  Removing the usb cable, and powering the ardino through
  the power plug gets everything to work correctly.  In both cases, the servos
  are power through a separate power source (brick).

  Does not matter whether Serial is used or not.
*/

#include <Servo.h>
Servo myservoA;
Servo myservoB;
Servo myservoC;

void setup()
{
  // Serial.begin(9600);
  myservoA.attach(22);
  myservoB.attach(23);
  myservoC.attach(24);
  myservoA.write(90);
  myservoB.write(90);
  myservoC.write(90);
}

void loop()
{
  // Serial.println("full forward");
  myservoA.write(180);
  myservoB.write(180);
  myservoC.write(180);
  delay(5000);
  myservoA.write(90);
  myservoB.write(90);
  myservoC.write(90);
  delay(1000);
  // Serial.println("full backward");
  myservoA.write(0);
  myservoB.write(0);
  myservoC.write(0);
  delay(5000);
  myservoA.write(90);
  myservoB.write(90);
  myservoC.write(90);
  delay(1000);
}
