#include <Servo.h>
#include <SPI.h>
#include "RF24.h"
#include "printf.h"


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

// The magical RF radio device
RF24 g_radio(7, 8);

// This is how the conroller barks orders at me
struct RFPacket
{
  char command;
  int16_t value;
};

// Uncomment for each robot
/* uint64_t g_RFAddress = 0xA11CE; //Alice */
/* uint64_t g_RFAddress = 0xB0B; //Bob */
uint64_t g_RFAddress = 0xE11E; //Eve


void setup()
{
#ifdef SERIAL_DEBUG
  printf_begin();
  Serial.begin(9600);
#endif

  // Setup the servos.
  g_servoLeft.attach(LEFT_SERVO_PIN);
  g_servoRight.attach(RIGHT_SERVO_PIN);

  // Boot up the radio
  g_radio.begin();

  // Open the pipe
  g_radio.openReadingPipe(1, g_RFAddress);

  // Listen good and listen well
  g_radio.startListening();

#ifdef SERIAL_DEBUG
  g_radio.printDetails();
#endif
}


void loop()
{
  // Poll the RF radio
  if(g_radio.available())
  {
    Serial.println("Packet!");
    struct RFPacket packet;
    g_radio.read(&packet, sizeof(struct RFPacket));
    handle_packet(packet);
  }
}


/**
 * Handle a command from the controller.
 *
 * @param packet The command packet
 */
void handle_packet(struct RFPacket& packet)
{
#ifdef SERIAL_DEBUG
  Serial.print("Got command: ");
  Serial.println(packet.command);
#endif

  switch(packet.command)
  {
    case 'M':
      process_motion(packet.value, 0);
      break;
    case 'T':
      process_motion(0, packet.value);
      break;
    case 'S':
      process_motion(0, 0);
      break;
    case 'H':
      digitalWrite(packet.value, HIGH);
      break;
    case 'L':
      digitalWrite(packet.value, LOW);
      break;
  }
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
bool process_motion(int16_t linearVel, int16_t angularVel)
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
