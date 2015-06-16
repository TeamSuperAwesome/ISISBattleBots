#include <Servo.h>

// Comment out for quickness.
#define SERIAL_DEBUG

// Centre point of the servo.
#define SERVO_CENTRE 1500

// Must be these pins for use of the timer, da ze.
#define LEFT_SERVO_PIN 9
#define RIGHT_SERVO_PIN 10

// Servo objects.
Servo g_servoLeft;
Servo g_servoRight;


void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
#endif

  // Setup the servos.
  g_servoLeft.attach(LEFT_SERVO_PIN);
  g_servoRight.attach(RIGHT_SERVO_PIN);
}


void loop()
{
  processMotion(500, 0);
  delay(1000);
  processMotion(-500, 0);
  delay(1000);
  processMotion(0, -500);
  delay(1000);
  processMotion(0, 500);
  delay(1000);
}


/**
 * Do motion related stuffs.
 *
 * I'm lazy so it can only do either rotational or linear motion, ze.
 *
 * @param linearVel Velocity in the linear direction (positive is forward)
 * @param angularVel Velocity in the angular direction (positive is right)
 * @return Boolean indicating if you read the f***ing manual
 */
bool processMotion(int16_t linearVel, int16_t angularVel)
{
  // NOOOOOOOOOOOOO!!!!!! ze.
  if(linearVel != 0 && angularVel != 0)
    return false;

  int16_t left = SERVO_CENTRE;
  int16_t right = SERVO_CENTRE;

  left += linearVel;
  right -= linearVel;

  left += angularVel;
  right += angularVel;

  g_servoLeft.writeMicroseconds(left);
  g_servoRight.writeMicroseconds(right);

#ifdef SERIAL_DEBUG
  Serial.print("Left servo (ns): ");
  Serial.println(left);
  Serial.print("Right servo (ns): ");
  Serial.println(right);
#endif

  return true;
}
