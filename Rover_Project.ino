#include <AFMotor.h>
#include <Servo.h>
#define pingPin A0 // Trigger Pin of Ultrasonic Sensor
#define echoPin A1 // Echo Pin of Ultrasonic Sensor
AF_DCMotor rightengine(1);
AF_DCMotor leftengine(2);

Servo lunaeyes;
void setup() {
  Serial.begin(9600);

  //Ultrasonic Sensor
  pinMode(pingPin, OUTPUT); // Trigger Pin of Ultrasonic Sensor
  pinMode(echoPin, INPUT);  // Echo Pin of Ultrasonic Sensor

  //Servo Motor
  lunaeyes.attach(10); // Defining Pin for Servo Motor
  lunaeyes.write(90); //Alligning the servo

  //Main Engines
  rightengine.setSpeed(200);
  rightengine.run(RELEASE);
  leftengine.setSpeed(200);
  leftengine.run(RELEASE);

  //Infra Red Sensor for detecting Ground Level
  pinMode(A2, INPUT); // Right IR
  pinMode(A3, INPUT); // Left IR
}

void loop() {
  rightengine.run(FORWARD);
  leftengine.run(FORWARD);

  //Finding forward range with Ultrasonic Sensor
  long middlerange, rightrange, leftrange;
  middlerange = howfaristhewall();

  //Engine forward operatin if no obstacle in middle range and rover projected path is at ground level
  //if ( (digitalRead(A2) == 0) && (digitalRead(A3) == 0) && (middlerange > 20)) {
    //rightengine.run(FORWARD);
    //leftengine.run(FORWARD);
  //}

  //Engine stop operation if obstacle found in middle range or rover projected path is not at ground level
  if ( (digitalRead(A2) == 1) || (digitalRead(A3) == 1) || (middlerange <= 20)) {

    //Stopping the engines.
    rightengine.run(RELEASE);
    leftengine.run(RELEASE);

    //Measuring other forward ranges to find a way to avoid obstacle.
    if (middlerange <= 20) {

      lunaeyes.write(90);
      delay(1000);

      lunaeyes.write(60);
      rightrange = howfaristhewall();
      delay(1000);

      lunaeyes.write(120);
      leftrange = howfaristhewall();
      delay(1000);

      lunaeyes.write(90);
      delay(1000);

      //If the range at the left side is spacious, manoeuvre the rover towards left
      if ((leftrange > middlerange) && (leftrange > rightrange)) {
        rightengine.run(FORWARD);
        leftengine.run(BACKWARD);
        delay(500);
      }

      //If the range at the right side is spacious, manoeuvre the rover towards right
      else if ((rightrange > middlerange) && (rightrange > leftrange)) {
        rightengine.run(BACKWARD);
        leftengine.run(FORWARD);
        delay(500);
      }

    }

    //If the projected path at the right side is not at ground level, manoeuvre the rover backward and then turn left side.
    if (digitalRead(A2) == 1) {
      rightengine.run(BACKWARD);
      leftengine.run(BACKWARD);
      delay(1000);
      rightengine.run(FORWARD);
      leftengine.run(BACKWARD);
      delay(500);
    }

    //If the projected path at the left side is not at ground level, manoeuvre the rover backward and then turn right side.
    if (digitalRead(A3) == 1) {
      rightengine.run(BACKWARD);
      leftengine.run(BACKWARD);
      delay(1000);
      rightengine.run(BACKWARD);
      leftengine.run(FORWARD);
      delay(500);
    }

  }
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

//Function to ping ultrasonic waves from the Ultrasonic sensor and finding range in centimeters.
int howfaristhewall() {
  long duration, forwardrange;
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  forwardrange = microsecondsToCentimeters(duration);
  Serial.print(forwardrange);
  Serial.print(" cm");
  Serial.println();
  delay(100);
  return forwardrange;
}
