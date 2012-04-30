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
/*
Description:
 Reads a Vixen2 .vix file and prepares the data for playback on the Arduino.
 Saves track length, event period, and number of channels in <filename>.dat in the root directory of the SD card in the form:
 trackLength<eventPeriod<numChannels< unused data EX: 148000<50<32<
 TrackLength, eventPeriod and numChannels are stored in ASCII and unused data is raw.
 Saves event values in <filename>.hex in the root directory of the SD card in the form:
 channel-->1.1<--byte
 
 1.1 2.1 3.1 4.1
 1.2 2.2 3.2 4.2
 1.3 2.3 3.3 4.3
 
 Will check for the existence of <filename>.dat. If found, it will return having done nothing.
 Configuration can be forced by deleting the existing <filename>.dat file before calling vixconfig().
 
 EX: Rudolph.vix
 Code:
 SD.remove("rudolph.dat"); //force config
 Syntax:
 vixconfig(filenameArray)
 
 Parameters:
 filenameArray: Name of the sequence file minus the extension placed into a 12 byte char array. 
 
 EX: Rudolph.vix
 
 Code:
 char seqNum1[12] = "rudolph";
 .
 .
 .
 vixconfig(seqNum1);
 
 Returns:
 Nothing.
 
 Usage:
 1. All file names must be in 8.3 format. This is the name of my file.vix will not work, but rudolph.vix will. 
 
 2. The Track Length must be evenly divisible by the Event Period.
 EX. if your track length is 4:30:551 (4 minutes, 30seconds and 551 milliseconds) and your Event Period is 25ms, the configuration will fail.
 If your track length was set to the length of your music in Vixen, this will likely happen. Round up sequence time to the next event period, ie 4:30:575
 3. Files must be placed in the root of the SD card.
 
 4. Only files created by Vixen 2.1.4.0 are supported
 
 5. Be sure to include the SD library and the Base64 library (included)
 
 EX:
 
 Code:
 #include <SD.h>
 #include <Base64.h>
 
 Errors:
 Debug messages are printed to the serial port.
 
 ERROR - FILE 
 The library was unable to find the .vix file you defined. Check the file name at the beginning of the code and ensure the file is in the root directory of the SD card.
 
 ERROR - TRACKLENGTH
 The library was unable to find the location of the track length in the .vix file you defined. Ensure you are using Vixen 2.1.4.0 for sequencing. Replace the .vix file.
 
 ERROR - EVENT
 The library was unable to find the location of the event period in the .vix file you defined. Try replacing the .vix file on the SD card.
 
 ERROR - CHANNELS
 The library was unable to find the location of the number of channels in the .vix file you defined. Try replacing the .vix file on the SD card.
 
 ERROR - REMAINDER
 The library detected that the track length is not even divisible by the event period. Round up your track length to the nearest event period. 
 
 ERROR - VALUES
 The library was unable to find the location of the event values in the .vix file you defined. Try replacing the .vix file on the SD card.
 
 ERROR - ORGANIZE
 The library was unable to find the location of the event values in the .dat file. Try force-config for this track
 
 */
#include "Arduino.h"
#include "Base64.h"
#include "SD.h"

void vixconfig(char *fileName);
void decode(uint32_t loops, char *vixName, char *datName);
void organize(uint32_t trackLength, uint16_t eventPeriod, uint8_t numChannels, char *fileName, char *datName);

