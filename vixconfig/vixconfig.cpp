/* vixconfig - Vixen file decoding for Arduino
 Copyright (C) 2012  Jon Proietti
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vixconfig.h"

void vixconfig(char *fileName)
{

  File vixen;
  uint32_t trackLength = 0, Cursor = 0;
  uint16_t eventPeriod = 0; 
  uint8_t numChannels = 0; //No more than 255 channels.
  boolean flag = 0;
  char searchArray[9], Read;
  uint32_t b64loops;
  char datName[12], vixName[12];

  //append extensions to the filename
  strncpy(datName, fileName, 8);
  strncpy(vixName, fileName, 8);
  strcat(vixName, ".vix");
  strcat(datName, ".dat");

  if (!SD.exists(vixName))
  {
    Serial.print("ERROR - ");
    Serial.println("FILE");
    return;
  }

  //check to see if a data file has already been made
  //if it has, exit
  if (!SD.exists(datName))
  {
    //Open .vix and find the track length in milliseconds
    vixen = SD.open(vixName, FILE_READ);
    Serial.print(vixName);
    Serial.print(" Scanning... ");
    vixen.seek(1);

    //Code to find TIME
    while(flag == 0 && vixen.available())
    {
      searchArray[0] = searchArray[1];
      searchArray[1] = searchArray[2];
      searchArray[2] = searchArray[3];
      searchArray[3] = vixen.read();

      if (searchArray[0] == 'i' && searchArray[1] == 'm' && searchArray[2] == 'e' && searchArray[3] == '>')
        flag = 1;
    }

    if (flag == 0)
    {
      Serial.print("ERROR - ");
      Serial.println("TRACKLENGTH");
      vixen.close();
      return;
    }

    flag = 0;
    while (Read != '<')
    { 
      Read = vixen.read();
      if (Read != '<')
      {
        trackLength += Read - 48;
        trackLength *= 10;
      }
      else
        trackLength /= 10;
    }
    Cursor = vixen.position();

    //Find the event period in milliseconds
    while (flag == 0 && vixen.available())
    {
      searchArray[0] = searchArray[1];
      searchArray[1] = searchArray[2];
      searchArray[2] = searchArray[3];
      searchArray[3] = vixen.read();

      if (searchArray[0] == 'n' && searchArray[1] == 'd' && searchArray[2] == 's' && searchArray[3] == '>')
        flag = 1;
    }

    if (flag == 0)
    {
      Serial.print("ERROR - ");
      Serial.println("EVENT");
      vixen.close();
      return;
    }

    flag = 0;
    Read = 0;
    while (Read != '<')
    { 
      Read = vixen.read();

      if (Read != '<')
      {
        eventPeriod += Read - 48;
        eventPeriod *= 10;
      }
      else
        eventPeriod /= 10;
    }

    //Code to find NUMBER OF CHANNELS
    while(vixen.available())
    {
      searchArray[0] = searchArray[1];
      searchArray[1] = searchArray[2];
      searchArray[2] = searchArray[3];
      searchArray[3] = searchArray[4];
      searchArray[4] = searchArray[5];
      searchArray[5] = searchArray[6];
      searchArray[6] = searchArray[7];
      searchArray[7] = searchArray[8];
      searchArray[8] = vixen.read();

      if (searchArray[0] == '/' && searchArray[1] == 'C' && searchArray[2] == 'h' && searchArray[3] == 'a' && searchArray[4] == 'n' && searchArray[5] == 'n' && searchArray[6] == 'e' && searchArray[7] == 'l' && searchArray[8] == '>')
        numChannels += 1;
    }
    if(numChannels == 0)
    {
      Serial.print("ERROR - ");
      Serial.println("CHANNELS");
      vixen.close();
      return;
    }
    else
      Serial.println(" Finsihed.");
    vixen.close();

    //Math to figure b64loops
    if(trackLength%eventPeriod != 0)
    {
      Serial.println("ERROR - REMAINDER");
      return;
    }
    b64loops = trackLength / eventPeriod;
    b64loops *= numChannels;
    flag = b64loops % 3;
    b64loops /= 3;
    if (flag != 0)
      b64loops++;

    //Save data to file
    File dataFile = SD.open(datName, FILE_WRITE);

    dataFile.print(trackLength);
    dataFile.write('<');

    dataFile.print(eventPeriod);
    dataFile.write('<');

    dataFile.print(numChannels);
    dataFile.write('<');

    dataFile.close();



    decode(b64loops, vixName, datName);

    organize(trackLength, eventPeriod, numChannels, fileName, datName);

  }
  else
  {
    Serial.print(datName);
    Serial.println(" found.");
    return;
  }
}


//Base64 decode
void decode(uint32_t loops, char *vixName, char *datName)
{
  Serial.println("Decoding."); 
  Serial.print("This will take a while... ");

  //searchArray is the input array, dataArray is the output array.
  //vixName is the name of the input file, datName is the name of the ouput file.

  char searchArray[4], dataArray[3];
  boolean flag = 0;
  File vixen, dataFile;

  vixen = SD.open (vixName, FILE_READ);
  dataFile = SD.open (datName, FILE_WRITE);

  //Find the location in the file with event values
  while(flag == 0 && vixen.available())
  {
    searchArray[0] = searchArray[1];
    searchArray[1] = searchArray[2];
    searchArray[2] = searchArray[3];
    searchArray[3] = vixen.read();

    if (searchArray[0] == 'u' && searchArray[1] == 'e' && searchArray[2] == 's' && searchArray[3] == '>')
      flag = 1;  
  }
  if (flag == 0)
  {
    Serial.print("ERROR - ");
    Serial.println("VALUES");
    vixen.close();
    dataFile.close();
    return;
  }
  for (long h = 0; h < loops; h++)
  {
    for (int i = 0; i < 4; i++)
      searchArray[i] = vixen.read();

    int j = base64_decode(dataArray, searchArray, 4);

    for (int k = 0; k < j; k++)
      dataFile.write(dataArray[k]);
  }

  Serial.println(" Finished.");
  dataFile.close();
  vixen.close();

}

void organize(uint32_t trackLength, uint16_t eventPeriod, uint8_t numChannels, char *fileName, char *datName)
{
  Serial.println("Organizing.");
  Serial.print("This will take a while... ");
  char hexName[12]; 
  char Read = 0;
  File hexFile, dataFile;
  uint32_t jump, Cursor = 0;
  uint8_t bufferArray[numChannels];  

  strncpy(hexName, fileName, 8);
  strcat(hexName, ".hex");

  SD.remove(hexName); //get rid of the old file so that we don't append to it.

  hexFile = SD.open(hexName, FILE_WRITE); //create our new hexFile
  dataFile = SD.open(datName, FILE_READ); //open the data file for reading

  jump = trackLength/eventPeriod; //jump is equal to the number of bytes per channel
  for (int i = 0; i<3; i++)
  {
    while (dataFile.available() && Read != '<')
      Read = dataFile.read();

    Read = dataFile.read();
  }
  if (dataFile.position() > 30)
  {
    Serial.print("ERROR - ");
    Serial.println("ORGANIZE");
    hexFile.close();
    dataFile.close();
  }

   Cursor = dataFile.position() - 1;

for (int j = 0; j < jump; j++)
  {
    dataFile.seek(Cursor);
    bufferArray[1] = dataFile.peek();

    for (int i = 2; i <= numChannels; i++)
    {
      Cursor += jump;
      dataFile.seek(Cursor);
      bufferArray[i] = dataFile.peek();
    }
	hexFile.write(bufferArray, numChannels);

    Cursor -= jump * (numChannels - 1) - 1;
	  }
  Serial.println(" Finished.");
  Serial.print(hexFile.size());
  Serial.println(" bytes written.");

  dataFile.close();
  hexFile.close();
}

