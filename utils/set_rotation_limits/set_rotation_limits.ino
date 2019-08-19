#include <ax12.h> //include the ArbotiX DYNAMIXEL library

struct limits_stc {
  int id;
  int cw;
  int ccw;
};

struct limits_stc limits[] = {
  { 1, 0, 4095 },
  { 2, 1000, 3050 },
  { 3, 2048, 3220 },
  { 4, 190, 790 },
  { 5, 190, 790 },
  { -1, -1, -1 },
};

void setup() {
  limits_stc *limit = limits;
  
  dxlInit(1000000);  //start dynamixel library at 1mbps to communicate with the servos
  Serial.begin(9600); //start serial at 9600bps

  while (limit->id > 0) {
    int cur_cwlimit = dxlGetCWAngleLimit(limit->id);
    int cur_ccwlimit = dxlGetCCWAngleLimit(limit->id);

    Serial.print("Current limits ("); Serial.print(limit->id); Serial.print(": ");
    Serial.print("CW: "); Serial.print(cur_cwlimit);
    Serial.print(" - CCW: "); Serial.println(cur_ccwlimit);

    if (cur_cwlimit != limit->cw) {
      Serial.print("Setting new cw limit: "); Serial.println(limit->cw);
      dxlSetCWAngleLimit(limit->id, limit->cw);
      delay(33);
    }
    
    if (cur_ccwlimit != limit->ccw) {
      Serial.print("Setting new ccw limit: "); Serial.println(limit->ccw);
      dxlSetCCWAngleLimit(limit->id, limit->ccw);
      delay(33);
    }

    cur_cwlimit = dxlGetCWAngleLimit(limit->id);
    cur_ccwlimit = dxlGetCCWAngleLimit(limit->id);
    Serial.print("new limits ("); Serial.print(limit->id); Serial.print(": ");
    Serial.print("CW: "); Serial.print(cur_cwlimit);
    Serial.print(" - CCW: "); Serial.println(cur_ccwlimit);
    Serial.println("");

    limit++;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
