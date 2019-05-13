
/********************************************************
 * 


 ***********************************************************************************/

#include <ax12.h>               //include base library for DYNAMIXELs
#include <BioloidController.h>  //include bioloid libary for poses/movements
#include "poses.h"              

BioloidController bioloid = BioloidController(1000000); //start the BIOLOID library at 1mbps. This will run the dxlInit() function internally, so we don't need to call it

const int SERVOCOUNT = 3;  //number of servos in this robot
int id;                    //temperary id for movement
int pos;                   //temporary position for movement
boolean runCheck = false;  //flag to see if we're running, so that we don't print out the menu options unnecessarily

void setup()
{
  Serial.begin(38400);
  bioloid.loadPose(hang);
}

void loop()
{   
    Serial.print("Temperature: ");
    for (int i=0; i<SERVOCOUNT; i++) {
      int temp = dxlGetTemperature(i+1);
      Serial.print(temp);
      Serial.print(", ");
    }
    Serial.println();
    
    delay(100);                      // recommended pause
    bioloid.loadPose(hang);        // load the pose from FLASH, into the nextPose buffer
    bioloid.readPose();              // read in current servo positions to the curPose buffer
    bioloid.interpolateSpeedSetup(1000);  // setup for interpolation from current->next over 1 second
    while(bioloid.interpolating > 0) // do this while we have not reached our new pose
    {  
        bioloid.interpolateSpeedStep();     // move servos, if necessary. 
    }
    
    bioloid.loadPose(pose1);        // load the pose from FLASH, into the nextPose buffer
    bioloid.readPose();              // read in current servo positions to the curPose buffer
    bioloid.interpolateSpeedSetup(1500);  // setup for interpolation from current->next over 1 second
    while(bioloid.interpolating > 0) // do this while we have not reached our new pose
    {  
        bioloid.interpolateSpeedStep();     // move servos, if necessary. 
    }
    
    bioloid.loadPose(pose2);        // load the pose from FLASH, into the nextPose buffer
    bioloid.readPose();              // read in current servo positions to the curPose buffer
    bioloid.interpolateSpeedSetup(1500);  // setup for interpolation from current->next over 1 second
    while(bioloid.interpolating > 0) // do this while we have not reached our new pose
    {  
        bioloid.interpolateSpeedStep();     // move servos, if necessary. 
    }

    bioloid.loadPose(pose3);        // load the pose from FLASH, into the nextPose buffer
    bioloid.readPose();              // read in current servo positions to the curPose buffer
    bioloid.interpolateSpeedSetup(1500);  // setup for interpolation from current->next over 1 second
    while(bioloid.interpolating > 0) // do this while we have not reached our new pose
    {  
        bioloid.interpolateSpeedStep();     // move servos, if necessary. 
    }

    bioloid.loadPose(pose4);        // load the pose from FLASH, into the nextPose buffer
    bioloid.readPose();              // read in current servo positions to the curPose buffer
    bioloid.interpolateSpeedSetup(1500);  // setup for interpolation from current->next over 1 second
    while(bioloid.interpolating > 0) // do this while we have not reached our new pose
    {  
        bioloid.interpolateSpeedStep();     // move servos, if necessary. 
    }
}
