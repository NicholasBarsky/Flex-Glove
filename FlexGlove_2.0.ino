#include <Wire.h> //enables communication

  int state = 0; //state for the forward and back cases
  int Steerstate = 0;//state for left and right cases

  // //flex sensors pins
  int FLEX1 = 32; //pinky
  int FLEX2 = 35; //ring
  int FLEX3 = 34; //middle
  int FLEX4 = 39; //index
  int FLEX5 = 36; //thumb

  //gyro centervalues and quick calibrations
  float centervalue = 0;
  float alpha = 0.01;

  //gyro sensor pins
  const int x = 4;//x-axis
  const int y = 0;//y-axis
  const int z = 2;//z-axis
  
//output pins to buttons
const int RIGHTpin = 33;
const int FORWARDpin = 25;
const int REVERSEpin = 26;
const int LEFTpin = 27;

//thresholds of sensors
//threshold of flex sensor seems to be around 1500~1800 sometimes even 2000
int flexthreshold = 1600;
int gyrothreshold = 80;
int deadzone = 20;

//startup controls
unsigned long starttime;
bool startupCal = true;

void setup() {
  Serial.begin(115200);
  pinMode(FORWARDpin, OUTPUT);
  pinMode(REVERSEpin, OUTPUT);
  pinMode(RIGHTpin, OUTPUT);
  pinMode(LEFTpin, OUTPUT);

  digitalWrite(FORWARDpin, HIGH);//set to high do to original controller inputs being inverted
  digitalWrite(REVERSEpin, HIGH); //originally LOW = OFF & HIGH = ON; now HIGH = OFF & LOW = ON
  digitalWrite(RIGHTpin, HIGH);
  digitalWrite(LEFTpin, HIGH);

  //------ instant calibration for the gyro sensor -----------
  centervalue = analogRead(y);//set to y due to sensor positioning

  starttime = millis();

}

void loop() {
  //-----------------------------------------------------
  //------------|| FORWARD AND REVERSE ||-----------------
  //flex sensors pins
  int f1 = analogRead(FLEX1); //pinky
  int f2 = analogRead(FLEX2); //ring
  int f3 = analogRead(FLEX3); //middle
  int f4 = analogRead(FLEX4); //index
  int f5 = analogRead(FLEX5); //thumb
  
  //cases for flex sensor activations
  bool FORWARD = (f1 > flexthreshold) && (f2 > flexthreshold) && 
                 (f3 > flexthreshold) && (f4 > flexthreshold);
  bool REVERSE = (f1 > flexthreshold) && (f2 > flexthreshold) && 
                  (f3 > flexthreshold);
   bool STOP = (f5 > flexthreshold);


  digitalWrite(FORWARDpin, HIGH);
  digitalWrite(REVERSEpin, HIGH);
  digitalWrite(RIGHTpin, HIGH);
  digitalWrite(LEFTpin, HIGH);


  //priority logic for forward and backwards
   if(STOP) {state = 0;}
   else if(FORWARD) {state = 2;}
   else if(REVERSE) {state = 3;}

  switch(state){
     case 2: 
     digitalWrite(FORWARDpin, LOW);
     if (!FORWARD){state = 0;}
     break;

     case 3:
     digitalWrite(REVERSEpin, LOW);
     if (!REVERSE){state = 0;}
     break;

     case 0:
     digitalWrite(FORWARDpin, HIGH);
     digitalWrite(REVERSEpin, HIGH);
     if (FORWARD) state = 2;
     else if (REVERSE) state = 3;
     break;
    }
  //-------------------------------------------
  //------------|| STEERING ||-----------------

  static float Reading = analogRead(y);
  int raw = analogRead(y);

  Reading = 0.9 * Reading + 0.1 * raw;
  float reading = Reading;
  float rotation = reading - centervalue;//difference from center

  if (millis() - starttime < 3000) {//startup lock to prevent drift 
    centervalue = reading;
    rotation = 0;
  }

  if(abs(rotation) < deadzone) {//adaptive calibration
    centervalue = (1 - alpha) * centervalue + alpha * reading;
  }

  //priority logic for steering left and right
  if (rotation > gyrothreshold) {Steerstate = 1;} //right turn
  else if (rotation < -gyrothreshold) {Steerstate = -1;} //left turn
  else {Steerstate = 0;} //straight

  switch(Steerstate){
    case -1:
      digitalWrite(RIGHTpin, LOW);
      digitalWrite(LEFTpin, HIGH);
      break;

    case 1:
      digitalWrite(LEFTpin, LOW);
      digitalWrite(RIGHTpin, HIGH);
      break;

    case 0:
      digitalWrite(LEFTpin, HIGH);
      digitalWrite(RIGHTpin, HIGH);
  }

  delay(50);
}
