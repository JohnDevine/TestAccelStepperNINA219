/*
This tests the stepper (28YBJ-48 and ULN2003)
It also tests the switch bounce library
*/
// Manual & Info here:
// http://www.airspayce.com/mikem/arduino/AccelStepper/
// https://hackaday.io/project/183279-accelstepper-the-missing-manual
// https://hackaday.io/project/183713-using-the-arduino-accelstepper-library

// In1 = Pin D33
// In2 = Pin D32
// In3 = Pin D27
// In4 = Pin D26
// Switch = Pin D19

#include <JDGlobal.h>
#include <MyStepper.h>
#include <elapsedMillis.h>
#include "Adafruit_INA219.h"

Adafruit_INA219 ina219;

// Define some steppers and the pins the will use
AccelStepper stepper1(AccelStepper::FULL4WIRE, 33, 27, 32, 26); // NOTE!!! Pin order for ULN2003 driver board is IN1, IN3, IN2, IN4

elapsedMillis printTime; // Keep track of print time

#define kDoOneRevolution 2048             // 4096 steps per revolution for 28BYJ-48 NOT true
#define kTargetSteps kDoOneRevolution * 3 // 3 revolutions
#define kMaxSpeed 500                     // Maximum speed in steps per second

void setup()
{
    Serial.begin(115200);

    stepper1.setMaxSpeed(kMaxSpeed); // Set the maximum speed in steps per second
    stepper1.setAcceleration(50);    // Acceleration in steps per second per second

    // stepper1.setSpeed(300); // use this if using constant speed

    stepper1.moveTo(kTargetSteps); // Move

    // Setup the INA219

    if (!ina219.begin())
    {
        Serial.println("Failed to find INA219 chip");
        while (1)
        {
            delay(10);
        }
    }
}

void loop()
{
    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power_mW = 0;
    // create a buffer for sprintf
    char buffer[200];

    if (printTime >= 3000)
    {
        printTime = 0;
        float mSpeed = stepper1.speed();

        sprintf(buffer, "Speed: %3.2f , distToGo: %d , currentPos: %d , targetPos: %d --- ", mSpeed, stepper1.distanceToGo(), stepper1.currentPosition(), stepper1.targetPosition());
        Serial.print(buffer);

        /*
        Serial.print(mSpeed);
        Serial.print(" ");
        Serial.print(stepper1.distanceToGo());
        Serial.print(" ");
        Serial.print(stepper1.currentPosition());
        Serial.print(" ");
        Serial.println(stepper1.targetPosition());
        */

        shuntvoltage = ina219.getShuntVoltage_mV();
        busvoltage = ina219.getBusVoltage_V();
        current_mA = ina219.getCurrent_mA();
        power_mW = ina219.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        // Use sprintf to print the shuntvoltage, busvoltage, current_mA, power_mW and loadvoltage to the buffer

        sprintf(buffer, "shuntV:   %3.2f , BusV: %3.2f ,LoadV: %3.2f, Current_mA: %3.2f ,Power mW: %3.2f  \n", shuntvoltage, busvoltage, loadvoltage, current_mA, power_mW);

        Serial.print(buffer);
    }

    if (stepper1.distanceToGo() == 0)
    {
        stepper1.disableOutputs();  // Test to see if this turns off the IN2003. (It does :)
    }
    stepper1.run();
}