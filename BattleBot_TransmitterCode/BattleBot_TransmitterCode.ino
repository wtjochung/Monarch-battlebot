#include <Adafruit_MotorShield.h>

Adafruit_MotorShield MotorShield= Adafruit_MotorShield();
Adafruit_DCMotor *wheel_l = MotorShield.getMotor(1);
Adafruit_DCMotor *wheel_r = MotorShield.getMotor(2);
Adafruit_DCMotor *wheel_weapon = MotorShield.getMotor(3);
//Adafruit_DCMotor *test_motor = MotorShield.getMotor(4);

#define CHANNEL1 11 //Right - horizontal
#define CHANNEL2 10 //Right - vertical
#define CHANNEL3 9 //Left - vertical
#define CHANNEL4 6 //Left - horizontal
#define CHANNEL5 5 //VRA
#define CHANNEL6 3 //VRB

int channel1Value;
int channel2Value;
int channel3Value;
int channel4Value;
int channel5Value;
int channel6Value;

uint8_t max_speed;
uint8_t wheel_speed;
uint8_t turning_speed;
uint8_t turning_cur;

// Read the number of a specified channel and map the reading to the provided range
// If the channel is off, return the default value
long readChannel(int channelInput, int mapLower, int mapUpper, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000); // Gets length of HIGH pulse, times out after 30000 useconds
  if (ch < 100) return defaultValue; // If less than 100 microseconds, not a significant pulse
  long mapped_value = map(ch, 1000, 2000, mapLower, mapUpper); // Map the value to the provided min/max
  // this mapped value could end up being greater than 2000 useconds or less than 1000 useconds, hence these if cases
  if (mapped_value < -255) 
    return -255;
  else if (mapped_value > 255) 
    return 255;
  else
    return mapped_value; 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Monarch code running;");
  pinMode(CHANNEL1, INPUT);
  pinMode(CHANNEL2, INPUT);
  pinMode(CHANNEL3, INPUT);
  pinMode(CHANNEL4, INPUT);
  pinMode(CHANNEL5, INPUT);
  pinMode(CHANNEL6, INPUT);

  if (!MotorShield.begin()) {
    Serial.println("Motor shield didn't boot correctly :( Check wiring.");
    while (1);
  }
  Serial.println("Motor shield did boot correctly :D");

  //Wheel start
  wheel_l->run(RELEASE);
  wheel_r->run(RELEASE);
  wheel_l->setSpeed(0);
  wheel_r->setSpeed(0);

  //Weapon start 
  wheel_weapon->run(RELEASE);
  wheel_weapon->setSpeed(0);
  
}


void loop() {
  channel1Value = readChannel(CHANNEL1, -255, 255, 0);
  channel2Value = readChannel(CHANNEL2, -255, 255, 0);
  channel3Value = readChannel(CHANNEL3, -255, 255, 0);
  channel4Value = readChannel(CHANNEL4, -255, 255, 0);
  //channel5Value = readChannel(CHANNEL5, -255, 255, 0);
  //channel6Value = readChannel(CHANNEL6, -255, 255, 0);

  //radio test code
  
  Serial.print("Right stick horizontal: ");
  Serial.print(channel1Value);
  Serial.print(", ");
  Serial.print("Right stick vertical: ");
  Serial.print(channel2Value);
  Serial.print(" ");
  /*
  Serial.print(", ");
  Serial.print("Left stick vertical: ");
  Serial.print(channel3Value);
  Serial.print(", ");
  Serial.print("Left stick horizontal: ");
  Serial.print(channel4Value);
  
  /*
  Serial.print(", ");
  Serial.print("VRA: ");
  Serial.print(channel5Value);
  Serial.print(", ");
  Serial.print("VRB: ");
  Serial.print(channel6Value);
  */
  //Serial.println();


  //delay(250);
  
  //motor test code
  /*
  uint8_t i;
  wheel_l->run(FORWARD);
  i = 100;
  wheel_l->setSpeed(i);
  delay(1000);
  
  wheel_l->run(BACKWARD);
  i = 500;
  wheel_l->setSpeed(i);
  delay(1000);
  */



  //Motor control
  //Note: uint8_t values only span from 0-255
  max_speed = 255;
  wheel_speed = abs(map(channel2Value, -260, 260, max_speed * -1, max_speed));
  //wheel_speed = readChannel(channel2Value, -255, 255, 0);
  //wheel_speed = sqrt(channel2Value*channel2Value + horizontalValue*horizontalValue)/sqrt(2);
  
  turning_speed = 150;
  //TODO: make turning_cur go from 20-turning_speed
  
  turning_cur = turning_speed - (abs(map(channel1Value, -260, 260, turning_speed * -1, turning_speed)));
  //turning_cur = readChannel(channel1Value, -255, 255, 0);
  
  int center_min = -20;
  int center_max = 20;

  //test_motor->run(FORWARD);
  //test_motor->setSpeed(wheel_speed);

  /*
  Serial.print("Wheel_speed: ");
  Serial.print(wheel_speed);
  Serial.print("; ");

  Serial.print("Wheel direction: ");
  */
  if ((channel1Value >= center_min) && (channel1Value <= center_max)){//right horizontal: the center postion
    //Serial.print("center ");
    if (channel2Value <= center_min){ //center backward
          //Serial.println("backward");
          wheel_l->run(BACKWARD);
          wheel_r->run(BACKWARD);
      
          wheel_l->setSpeed(wheel_speed);
          wheel_r->setSpeed(wheel_speed);
    } else if (channel2Value >= center_max) { //center foward
          //Serial.println("forward");
          wheel_l->run(FORWARD);
          wheel_r->run(FORWARD);
      
          wheel_l->setSpeed(wheel_speed);
          wheel_r->setSpeed(wheel_speed);
    } else { //center position. stop
          //Serial.println("stop");
          wheel_l->run(FORWARD);
          wheel_r->run(FORWARD);
      
          wheel_l->setSpeed(0);
          wheel_r->setSpeed(0);
    }
    
  } else if (channel1Value > center_max){ //going right
    //Serial.print("right ");
    if (channel2Value > center_max){ //going forward right
           // Serial.println("forward");
      wheel_l->run(FORWARD);
      wheel_r->setSpeed(turning_cur);
      wheel_r->run(FORWARD);
      wheel_l->setSpeed(wheel_speed);
    } else if (channel2Value < center_min) { //going backward right

      //Serial.println("backward");
      wheel_l->run(BACKWARD);
      wheel_r->setSpeed(turning_cur);
      wheel_r->run(BACKWARD);
      wheel_l->setSpeed(wheel_speed);

      
    } else { //right position. turn in place
      //Serial.println("in-place");
      uint8_t turning_speed_reverse = (abs(map(channel1Value, -260, 260, turning_speed * -1, turning_speed)));
      wheel_l->run(FORWARD);
      wheel_l->setSpeed(turning_speed_reverse);
      
      wheel_r->run(BACKWARD);
      wheel_r->setSpeed(turning_speed_reverse);
    }
    
  } else if (channel1Value < center_min){ //going left
    //Serial.print("left ");
    if (channel2Value > center_max){ //going forward left


      //Serial.println("forward");
      wheel_l->run(FORWARD);
      wheel_r->run(FORWARD);
      
      wheel_r->setSpeed(wheel_speed);
      wheel_l->setSpeed(turning_cur);
    } else if (channel2Value < center_min) { //going backward left


      //Serial.println("backward");
      wheel_l->run(BACKWARD);
      wheel_r->run(BACKWARD);
      
      wheel_r->setSpeed(wheel_speed);
      wheel_l->setSpeed(turning_cur);
    } else { //left position. turn in place
      //Serial.println("in-place");
      uint8_t turning_speed_reverse = (abs(map(channel1Value, -260, 260, turning_speed * -1, turning_speed)));
      wheel_l->run(BACKWARD);
      wheel_l->setSpeed(turning_speed_reverse);
      wheel_r->run(FORWARD);
      wheel_r->setSpeed(turning_speed_reverse);
    }
  } 

  //Weapon control
  uint8_t max_weapon_speed = 250;
  uint8_t weapon_speed = abs(map(channel3Value, -260, 260, 0, max_weapon_speed));
  //uint8_t weapon_speed = channel3Value;
 
  int weapon_start_threshold = -230;
  int weapon_reverse_threshold = 50;

  Serial.print("Channel3value: ");
  Serial.print(channel3Value);
  Serial.print("; Weapon_speed: ");
  Serial.print(weapon_speed);
  Serial.print("; Direction: ");

  //Cases: low forward, high forward, low backward, high backward
  if (channel3Value <= weapon_start_threshold || channel3Value == 0){
    //Do not start weapon motor
    Serial.println("weapon stopped");
    wheel_weapon->run(FORWARD);
    wheel_weapon->setSpeed(0);
  } else if (channel4Value > weapon_reverse_threshold){
      Serial.println("backward");
      wheel_weapon->run(BACKWARD);
      wheel_weapon->setSpeed(weapon_speed);
    } else {
      Serial.println("forward");
      wheel_weapon->run(FORWARD);
      wheel_weapon->setSpeed(weapon_speed);
    }
  }
  
