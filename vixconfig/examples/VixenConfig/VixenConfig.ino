#include <SD.h>
#include <Base64.h>
#include <vixconfig.h>

/******************************************
 *When defining file names, leave off .vix
 *For example, rudolph.vix would be defined as "rudolph"
 *Make sure filenames are in the 8.3 format
 *******************************************/
#define file1 "rudolph"
#define file2 "donner"
#define file3 "prancer"
#define chipSelect 10 //this is the pin used for the chip select on the SD card. 


void setup()
{
  //Put file names into arrays. If file2/3 are not being used, comment them out with //
  char seqNum1[12] = file1;
  char seqNum2[12] = file2;
  //char seqNum3[12] = file3;

  //Initialize serial debug
  Serial.begin(9600); //baud rate
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);

  //Initialize SD card
  if (!SD.begin(chipSelect)) 
  {
    //If card failed to initialize, exit.
    Serial.println(" Failed.");
    return;
  }
  Serial.println(" Finished.");

  /************************************************************
   * Call the config function to configure each track. 
   * use config(fileName#); where # is the number of the track to configure.
   * 
   * To force a file config, use the following code before calling the config function.
   * Be sure to change file1 to file2/3 if needed:
   * 
   * SD.remove(file1".dat"); 
   *************************************************************/

  SD.remove(file1".dat"); //force config mode for file1
  SD.remove(file2".dat"); //force config mode for file2
  //SD.remove(file3".dat"); //force config mode for file3
  
  vixconfig(seqNum1);
  vixconfig(seqNum2);
  //vixconfig(seqNum3);
/*****************************************
After configuration, data will be found on the root directory of the SD card.
*/



}


void loop()
{
}






