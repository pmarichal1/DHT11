/**********************************************************************
* Filename    : I2CLCD1602.c
* Description : Use the LCD display data
* Author      : freenove
* modification: 2017/04/18
**********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>

//#define pcf8574_address 0x27        // default I2C address of Pcf8574
#define pcf8574_address 0x3F        // default I2C address of Pcf8574A
#define BASE 64         // BASE is not less than 64
//////// Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
#define RS      BASE+0
#define RW      BASE+1
#define EN      BASE+2
#define LED     BASE+3
#define D4      BASE+4
#define D5      BASE+5
#define D6      BASE+6
#define D7      BASE+7
#define buttonPin 1		//define the buttonPin
#define ledPin 0

int pressCnt=1;
int buttonState=HIGH;	//store the State of button
int lastbuttonState=HIGH;//store the lastState of button
int captureTime=50;	//set the button state stable time
int lastChangeTime;	//store the change time of button state
int reading;

int lcdhd;// used to handle LCD
void printCPUTemperature(){// sub function used to print CPU temperature
    FILE *fp;
    char str_temp[15];
    float CPU_temp;
    // CPU temperature data is stored in this directory.
    fp=fopen("/sys/class/thermal/thermal_zone0/temp","r");
    fgets(str_temp,15,fp);      // read file temp
    CPU_temp = atof(str_temp)/1000.0;   // convert to Celsius degrees
    CPU_temp=(CPU_temp*9/5)+32;//convert to Farenheit
    //printf("CPU's temperature : %.2fF \n",CPU_temp);
    lcdPosition(lcdhd,0,0);     // set the LCD cursor position to (0,0)
    lcdPrintf(lcdhd,"CPU:%.2fF",CPU_temp);// Display CPU temperature on LCD
    fclose(fp);
}
void printDataTime(){//used to print system time
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);// get system time
    timeinfo = localtime(&rawtime);// convert to local time
    //printf("%s \n",asctime(timeinfo));
    lcdPosition(lcdhd,0,1);// set the LCD cursor position to (0,1)
    lcdPrintf(lcdhd,"Time:%d:%d:%d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    lcdPosition(lcdhd,0,0);// set the LCD cursor position to (0,1)
    lcdPrintf(lcdhd,"LED ON = %d",millis()/1000);
    }

void printClear(int lineToClear){//used to print system time

    lcdPosition(lcdhd,0,lineToClear);// set the LCD cursor position to (0,1)
    lcdPrintf(lcdhd,"                 ");
    }

 void printMillis(void)
    {
    int elapsedTime;
    elapsedTime = millis() - lastChangeTime;
    lastChangeTime= millis() ;
    printf("Elapsed=%d\n",elapsedTime/1000);
    }

int main(void){
    int i;

    if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
        printf("setup wiringPi failed !");
        return 1;
    }
    pcf8574Setup(BASE,pcf8574_address);// initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(BASE+i,OUTPUT);     // set PCF8574 port to output mode
    }
    digitalWrite(LED,HIGH);     // turn on LCD backlight
    digitalWrite(RW,LOW);       // allow writing to LCD
    pinMode(ledPin, OUTPUT);//Set the pin mode
    lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return “handle” used to handle LCD
    if(lcdhd == -1){
        printf("lcdInit failed !");
        return 1;
    }

    pinMode(buttonPin, INPUT);
	pullUpDnControl(buttonPin, PUD_UP);  //pull up to high level
    while(1){
             reading = digitalRead(buttonPin); //read the current state of button
             buttonState=reading;
//printf("Button State = %d\n", reading);

				if(buttonState == LOW){
					printf("Button is pressed! = %d\n", pressCnt);
					pressCnt++;
					lcdClear(lcdhd);
                    printDataTime();        // print system time
                    for (int loop=0;loop<5 ;loop++)
                    {
                    digitalWrite(ledPin, HIGH);  //led on
			        //printf("led on...\n");
			        delay(250);
			        //printMillis();
                    digitalWrite(ledPin, LOW);  //led off
			        //printf("...led off\n");
                    delay(250);
                    //printMillis();
                    }
					}
				//if the state is high ,the action is releasing
				else {
					//printf("Button is released!\n ");
					lcdClear(lcdhd);
                    printCPUTemperature();
                    read_dht_data();
                        delay( 2000 ); /* wait 2 seconds before next read */
				}
    delay(100);
    }
    }

