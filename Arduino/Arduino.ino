#include "MeMegaPi.h"
#include <SoftwareSerial.h>
bool spd = true;
int moveSpeed = 100;
int minSpeed = 20;
int maxSpeed = 200;
int servoX = 90;
int servoY = 90;
String _buffer = "";
String _commands[6];

MePS2 joystick(PORT_16);

MeMegaPiDCMotor motorLeft(PORT1A);
MeMegaPiDCMotor motorRight(PORT1B);
MeMegaPiDCMotor armLeft(PORT2A);
MeMegaPiDCMotor armRight(PORT2B);
MePort port6(PORT_6);
MePort port7(PORT_7);
Servo svx;
Servo svy;
void setup() {
  Serial2.begin(115200);
  joystick.begin(115200);
  svx.attach(port7.pin1());
  svy.attach(port6.pin1());
  svx.write(90);
  svy.write(70);
}

void loop() {
  if(Serial2.available()){
    char c = Serial2.read();
    if(c!='\n'){
      _buffer+=c;
    }else{
      parseBuffer();
    }
  }
  joystickUpdate();
}
void parseBuffer(){
  int startIndex = 0,endIndex = 0,index = 0,len = _buffer.length(),maxCmdLen = 0;
  while(1){
    endIndex = _buffer.indexOf(",",startIndex);
    if(endIndex>0){
      _commands[index] = _buffer.substring(startIndex,endIndex);
      index++;
      startIndex = endIndex+1;
    }
    if(index>5||endIndex==-1){
      if(startIndex<len){
        _commands[index] = _buffer.substring(startIndex,len);
        index++;
      }
      break;
    }
  }
  maxCmdLen = index-1;
  if(_commands[0].equals("face")){
    float faceX = _commands[1].toFloat();
    float faceY = _commands[2].toFloat();
    float faceW = _commands[3].toFloat();
    float faceH = _commands[4].toFloat();
    moveServo(faceX,faceY);
  }else if(_commands[0].equals("reset")){
    svx.write(90);
    svy.write(70);
  }
  _buffer = "";
}
void joystickUpdate(){
  joystick.loop();
  if (joystick.ButtonPressed(MeJOYSTICK_UP))
  {
        forward();
  }else if (joystick.ButtonPressed(MeJOYSTICK_DOWN))
  {
        backward();
  }else if (joystick.ButtonPressed(MeJOYSTICK_LEFT))
  {
        turnLeft();
  }else if (joystick.ButtonPressed(MeJOYSTICK_RIGHT))
  {
        turnRight();
  }else if (joystick.ButtonPressed(MeJOYSTICK_R1))
  {
    if(spd){
      spd = false;
      moveSpeed+=5;
      moveSpeed = min(maxSpeed,moveSpeed);
    }
  }else if (joystick.ButtonPressed(MeJOYSTICK_R2))
  {
    if(spd){
      spd = false;
      moveSpeed-=5;
      moveSpeed = max(minSpeed,moveSpeed);
    }
  }else{
    spd = true;
    float joystickX = joystick.MeAnalog(MeJOYSTICK_LX)/5.0;
    float joystickY = joystick.MeAnalog(MeJOYSTICK_LY)/5.0;
    float angle = fmod(atan2(joystickY,joystickX)/2.0+PI*2.0,PI*2.0);
    float dist = sqrt(joystickX*joystickX+joystickY*joystickY)*moveSpeed/(minSpeed+40);
    if(angle<PI/2.0){
      move(-dist*cos(angle),dist*sin(angle));
    }else{
      move(dist*cos(angle),dist*sin(angle));
    }
  }
}

void forward(void)
{
  motorLeft.run(-moveSpeed);
  motorRight.run(moveSpeed);
}

void backward(void)
{
  motorLeft.run(moveSpeed);
  motorRight.run(-moveSpeed);
}

void turnRight(void)
{
  motorLeft.run(-moveSpeed);
  motorRight.run(-moveSpeed);
}

void turnLeft(void)
{
  motorLeft.run(moveSpeed);
  motorRight.run(moveSpeed);
}

void stop(void)
{
  motorLeft.stop();
  motorRight.stop();
}

void move(int left,int right)
{
  motorLeft.run(left);
  motorRight.run(right);
}
float prevX = 0.0;
float prevY = 0.0;
float ppX = 0.0;
float ppY = 0.0;
void moveServo(float x,float y)
{
  x = max(min(x+5,45),-45);
  y = max(min(y-5,45),-45);
  float cx = servoX-90;
  float cy = servoY-90;
  float dX = x*(cx*x<0?60:(60-abs(cx)))/45.0/20.0;
  float dY = y*(cy*y<0?60:(60-abs(cy)))/45.0/20.0;
//  for(int i=1;i<10;i++){
//    svx.write(round(servoX+dX*i/10));
//    svy.write(round(servoY+dY*i/10));
//    delay(10);
//  }
  float scale = min(10,4*(abs(prevX-dX)+1));
  servoX += scale*dX-prevX*2.2;//+ppX*0.1;
  servoY += scale*dY-prevY*2.2;//+ppY*0.1;
  ppX = prevX;
  ppY = prevY;
  prevX = dX;
  prevY = dY;
  servoX = max(min(servoX,135),45);
  servoY = max(min(servoY,135),45);
  svx.write(round(servoX));
  svy.write(round(servoY));
  Serial2.println("ok");
}

