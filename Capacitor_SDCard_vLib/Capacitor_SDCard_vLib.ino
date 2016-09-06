//program for charging and discharging of capacitor
//data can be stored in sd card
//programmed by nelio c. altoveros and hazel joyce m.ramirez
//february 11, 2014

//Revision 1.1
//February 12, 2014
//Added sequential filenames
//Revision 1.2
//February 20, 2014
//Incorporate advance gathering of data
//  to ensure that zero readings are stored in SD Card

// The SDCard circuit:
// * SD card attached to SPI bus as follows:
// ** CS - pin 10 - pin 4 is used by LCD
// ** MOSI - pin 11
// ** MISO - pin 12
// ** SCK - pin 13 (CLK)

#include <SPI.h>
#include <SD.h>
#include <stdio.h>
#include <vLib.h>

vLib vlib;

//Analog Sensor circuit:
// * analog sensor on A1
const int analogInPin = A1;  // Analog input pin
const int chipSelect = 10;    // SDCard Chip Select

// On the Ethernet Shield, Chip Select is pin 4. Note that even if it's not
// used as the CS pin, the hardware Chip Select pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.

int suffix=1;
char fname[15]="CAP",filename[15]="CAP001.csv";
float volts=0.0;             //sensor reading
unsigned long ttime=0;                //elapsed time
String dataString = "";


void setup() {
  pinMode(10, OUTPUT); // chipSelect pin = 10
  // initialize serial communications at 9600 bps:
  Serial.begin(57600);
  vlib.lcdInitialize(9,8);
  initSDCard();  
  initvLibDisplay();
  delay(3000);
}

void initSDCard()
{
  Serial.print("");
  Serial.println("Charging and Discharging Capacitor");
  Serial.println("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
   if (!SD.begin(chipSelect)) {
     Serial.println("Card failed, or not present");
     // don't do anything more:
   }
   else {
   Serial.println("Card initialized.");
   }
   delay(2000);
}


void initvLibDisplay()
{
  //vlib.lcdClear();  
  vlib.lcdCmd(0x80);
  vlib.lcdPrint("Charging and");
  vlib.lcdCmd(0xC0);
  vlib.lcdPrint("Discharging of C"); 
}

void vlibDisplayData()
{
  vlib.lcdCmd(0x80);
  vlib.lcdPrint("T(ms): ");
  vlib.lcdCmd(0x87);
  char ttimes[10];
  floatToString(ttimes, ttime, 7, 1);
  vlib.lcdPrint(ttimes);
  vlib.lcdCmd(0xC0);
  vlib.lcdPrint("Vc(v): ");
  vlib.lcdCmd(0xCA);
  char voltss[5];
  floatToString(voltss, volts, 2, 2);
  vlib.lcdPrint(voltss);
}

void labelData()
{
  while (SD.exists(filename)) {   
      Serial.print(filename);
      Serial.println(" exists");
      snprintf(filename, 12, "%s%03d.csv", fname,++suffix);
  }
  Serial.print("Saving data to ");
  Serial.println(filename);
  File dataFile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print("TIME");
    dataFile.print(",");
    dataFile.println("VOLTAGE");    
    dataFile.close();
  }
    //if the file isn't open, pop up an error:
  else {
    Serial.print("Error opening ");
    Serial.println(filename);
  } 
}

void logData(){
  File dataFile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    
    dataFile.print(dataString);
    dataFile.println(volts, 5);
    dataFile.close();
  }  
}   

/*
      LOOP
*/

void loop() {
  int sensorValue = 0;         // value read from the sensor
  long int start;              //start of sampling
  long int loopcnt=0;
  labelData();
  //vlib.lcdClear();
  vlib.lcdCmd(0x80);
  vlib.lcdPrint("Flip to Charge");
  delay(1000);
  //vlib.lcdClear();
  Serial.println("Flip to Charge");
  Serial.print("TIME(ms)");
  Serial.print('\t'); 
  Serial.println("VOLTAGE(v)");  
  start=millis();
  while (1) {
    loopcnt++;
    sensorValue = analogRead(analogInPin);
    volts=(sensorValue*5.0)/(1023.0);
    ttime= (millis()-start);
    Serial.print(ttime);
    Serial.print("\t\t");  
    Serial.println(volts,5);
    dataString=String(ttime)+",";
    if (loopcnt>100) {
      vlibDisplayData();
    }
    logData();
    delay(20);     
  }
}


char * floatToString(char * outstr, double val, byte precision, byte widthp) {
  char temp[16]; //increase this if you need more digits than 15
  byte i;

  temp[0] = '\0';
  outstr[0] = '\0';

  if (val < 0.0) {
    strcpy(outstr, "-\0"); //print "-" sign
    val *= -1;
  }

  if ( precision == 0) {
    strcat(outstr, ltoa(round(val), temp, 10)); //prints the int part
  }
  else {
    unsigned long frac, mult = 1;
    byte padding = precision - 1;

    while (precision--)
      mult *= 10;

    val += 0.5 / (float)mult;    // compute rounding factor

    strcat(outstr, ltoa(floor(val), temp, 10)); //prints the integer part without rounding
    strcat(outstr, ".\0"); // print the decimal point

    frac = (val - floor(val)) * mult;

    unsigned long frac1 = frac;

    while (frac1 /= 10)
      padding--;

    while (padding--)
      strcat(outstr, "0\0");   // print padding zeros

    strcat(outstr, ltoa(frac, temp, 10)); // print fraction part
  }

  // generate width space padding
  if ((widthp != 0) && (widthp >= strlen(outstr))) {
    byte J = 0;
    J = widthp - strlen(outstr);

    for (i = 0; i < J; i++) {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp, outstr);
    strcpy(outstr, temp);
  }

  return outstr;
}

