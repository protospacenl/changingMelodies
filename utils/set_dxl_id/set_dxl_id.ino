#include <ax12.h> //include the ArbotiX DYNAMIXEL library

void setup()
{
  dxlInit(1000000);  //start dynamixel library at 1mbps to communicate with the servos
  Serial.begin(9600); //start serial at 9600bps  

  delay(250);
  //dxlSetId(1, 2);
  //delay(250);
  //dxlSetBaud(2, 1);
  //delay(250);
  int reg = dxlGetRegister(2, 4, 1);
  Serial.println(reg);
}
void loop()
{
   
}
