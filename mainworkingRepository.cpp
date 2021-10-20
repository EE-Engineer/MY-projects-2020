
#include <Arduino.h>

// Uncomment this define to enable debug output.
#define PCA9685_ENABLE_DEBUG_OUTPUT     1

#include <Wire.h>
#include "PCA9685.h"
#include "BasicStepperDriver.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120

PCA9685 pwmController(Wire, PCA9685_PhaseBalancer_Weaved); // Library using Wire1 and weaved phase balancing scheme

// Linearly interpolates between standard 2.5%/12.5% phase length (102/512) for -90°/+90°
PCA9685_ServoEvaluator pwmServo1;

// Testing our second servo has found that -90° sits at 128, 0° at 324, and +90° at 526.
// Since 324 isn't precisely in the middle, a cubic spline will be used to smoothly
// interpolate PWM values, which will account for said discrepancy. Additionally, since
// 324 is closer to 128 than 526, there is less resolution in the -90° to 0° range, and
// more in the 0° to +90° range.
PCA9685_ServoEvaluator pwmServo2(128,324,526);

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, 2, 3);

void setup() {

  Serial.begin(115200);

  // put your setup code here, to run once:
  Wire.begin();                       // Wire must be started first

  pwmController.resetDevices();

  pwmController.init(B000000);        // Address pins A5-A0 set to B000000
  pwmController.setPWMFrequency(50);  // 50Hz provides 20ms standard servo phase length

  pwmController.setChannelPWM(0, pwmServo1.pwmForAngle(-90));
  Serial.println(pwmController.getChannelPWM(0)); // Should output 102 for -90°
stepper.begin(RPM, 1);

}

unsigned char Count = 0;
 
void loop() {
  // put your main code here, to run repeatedly:

if (Count < 90)

{
  pwmController.setChannelPWM(4,  Count + 30);
  pwmController.setChannelPWM(8,  Count - 10);
  Count+=10;

  Count &= 0x7f;

  stepper.move( 1 );

  delay(10);
}
  else {
  
  pwmController.setChannelPWM(4, pwmServo2.pwmForAngle(0));
  
  pwmController.setChannelPWM(8, pwmServo2.pwmForAngle(0));
  Count++;
  Count &= 0x7f;

  stepper.move( 1 );

  delay(10);
}
}