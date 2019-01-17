
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
#include <stdint.h>
#include <sys/time.h>


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
//define buttom pin
#define buttonPin 1
//define the LED pin
#define ledPin 0
//define the pin of sensor
#define DHT11_Pin  3 

//sonar setup
#define trigPin 4       
#define echoPin 5
#define MAX_DISTANCE 220        // define the maximum measured distance
#define timeOut MAX_DISTANCE*60 // calculate timeout according to the maximum measured distance

////read return flag of sensor
#define DHTLIB_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999
#define DHTLIB_DHT11_WAKEUP     18 //18 original value
#define DHTLIB_TIMEOUT          400  //400 original

double humidity,temperature;    //use to store temperature and humidity data read
uint8_t bits[5];    //Buffer to receiver data
int readSensor(int pin,int wakeupDelay);
int readDHT11(int pin);     //read DHT11
int sumCnt=0,sumCntFailures=0;
int displayState=HIGH;	//store the State of button
int lastDisplayState=HIGH;//store the lastState of button
int captureTime=50;	//set the button state stable time
int lastChangeTime;	//store the change time of button state
int reading;
float hightemp=0,lowtemp=100,highhumid=0,lowhumid=100;
float percentFailure=0;


int lcdhd;// used to handle LCD

int getDHT(void);
void printTemperature(int);
void printHumidity(int);
void printTemperatureErrors (void);
void printCPUTemperature(int);
void printDataTime(int);
void printSonar(int);
void printClear(int);
void printMillis(void);
float getSonar();  // get the measurement results of ultrasonic module,with unit: cm


//function pulseIn: obtain pulse time of a Sonar pin
int pulseIn(int pin, int level, int timeout);


int main(void)
    {
    int DHTloop,dhtRet=0,loopCnt=0,i=0;
    
    if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
        printf("setup wiringPi failed !");
        return 1;
    }
    pcf8574Setup(BASE,pcf8574_address);// initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(BASE+i,OUTPUT);     // set PCF8574 port to output mode
    }
    pinMode(trigPin,OUTPUT);
    pinMode(echoPin,INPUT);
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
    while(1)
        {
	reading = digitalRead(buttonPin); //read the current state of button
	if(reading == LOW)
	    printTemperatureErrors ();
	// toggle display state    
	if(lastDisplayState==LOW)
	    displayState = HIGH;
	else
	    displayState = LOW;

        if(displayState == LOW) //pressed
            {
	    for (DHTloop =0; DHTloop <10; DHTloop++)
		{
		//printf("\nReading DHT - %d\n",DHTloop);
		dhtRet=getDHT();
		if(dhtRet == 0)
		    break;
		delay(1000);
		}
	    if(lastDisplayState == HIGH)
		{
		printClear(0);
		printClear(1);
		lastDisplayState=LOW;
		}
	    digitalWrite(ledPin, HIGH);  //led on
	    delay(250);
	    digitalWrite(ledPin, LOW);  //led off
	    delay(250);
	    printCPUTemperature(1);
            if(dhtRet == 0)
                {
                printTemperature(0);
                printHumidity(1);
                }
            loopCnt++;
            }

        else
            {
	    if(lastDisplayState == LOW)
		{
		printClear(0);
		printClear(1);
		lastDisplayState=HIGH;
		}
	    //printCPUTemperature(1);
	    printSonar(1);
	    printDataTime(0);
	    delay( 1000 ); /* wait 1 seconds before next read */
            }
	loopCnt=0;
	delay( 3000 ); /* wait x seconds before next read */
        }
    }

void printTemperatureErrors (void)
    {
    printf("\n\e[1;31mThe sumCnt is : %d  FailureCnt = %d  Failure%% %2.2f\n",sumCnt,sumCntFailures,percentFailure);
    printf("High Humidity is %.0f %%, \t High Temperature is %.2f *F\n",highhumid,hightemp);
    printf("Low Humidity is %.0f %%, \t Low Temperature is %.2f *F\e[0m\n\n",lowhumid,lowtemp);
    }
    
void printTemperature(int lineNum)
    {// sub function used to print CPU temperature
    lcdPosition(lcdhd,0,lineNum);     // set the LCD cursor position to (0,0)
    lcdPrintf(lcdhd,"TEMP:%.2fF",temperature);// Display CPU temperature on LCD
    }
 void printHumidity(int lineNum)
    {// sub function used to print CPU temperature
    //printf("CPU's temperature : %.2fF \n",CPU_temp);
    lcdPosition(lcdhd,0,lineNum);     // set the LCD cursor position to (0,0)
    lcdPrintf(lcdhd,"Humidity:%.0f%%",humidity);// Display CPU temperature on LCD
    }
void printSonar(int lineNum)
    {// sub function used to print CPU temperature
    float distance = 0;
    distance = getSonar();
    lcdPosition(lcdhd,0,lineNum);     // set the LCD cursor position to (0,0)
    lcdPrintf(lcdhd,"Distance: %.2f",distance);// Display CPU temperature on LCD
    printf("\n\e[1;33mDistance:%.2f inches \e[0m\n",distance);// Display CPU temperature on LCD
    }
    
void printCPUTemperature(int lineNum)
    {// sub function used to print CPU temperature
    FILE *fp;
    char str_temp[15];
    float CPU_temp;
    // CPU temperature data is stored in this directory.
    fp=fopen("/sys/class/thermal/thermal_zone0/temp","r");
    fgets(str_temp,15,fp);      // read file temp
    CPU_temp = atof(str_temp)/1000.0;   // convert to Celsius degrees
    CPU_temp=(CPU_temp*9/5)+32;//convert to Farenheit
    printf("\nCPU's temperature : %.2fF \n",CPU_temp);
    //lcdPosition(lcdhd,0,lineNum);     // set the LCD cursor position to (0,0)
    //lcdPrintf(lcdhd,"CPU:%.2fF",CPU_temp);// Display CPU temperature on LCD
    fclose(fp);
    }
void printDataTime(int lineNum)
    {//used to print system time
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);// get system time
    timeinfo = localtime(&rawtime);// convert to local time
    //printf("%s \n",asctime(timeinfo));
    lcdPosition(lcdhd,0,lineNum);// set the LCD cursor position to (0,1)
    lcdPrintf(lcdhd,"Time:%d:%d:%d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
 //   lcdPosition(lcdhd,0,0);// set the LCD cursor position to (0,1)
 //   lcdPrintf(lcdhd,"LED ON = %d",millis()/1000);
    }

void printClear(int lineToClear)
    {
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



//Function: Read DHT sensor, store the original data in bits[]
// return values:DHTLIB_OK   DHTLIB_ERROR_CHECKSUM  DHTLIB_ERROR_TIMEOUT
int readSensor(int pin,int wakeupDelay)
    {
	int mask = 0x80;
	int idx = 0;
	int i ;
	int32_t t;
	for (i=0;i<5;i++){
		bits[i] = 0;
	}
	pinMode(pin,OUTPUT);
	digitalWrite(pin,LOW);
	delay(wakeupDelay);
	digitalWrite(pin,HIGH);
	delayMicroseconds(40);
	pinMode(pin,INPUT);


	int32_t loopCnt = DHTLIB_TIMEOUT;
	t = micros();
	while(digitalRead(pin)==LOW){
		if((micros() - t) > loopCnt){
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
	loopCnt = DHTLIB_TIMEOUT;
	t = micros();
	while(digitalRead(pin)==HIGH){
		if((micros() - t) > loopCnt){
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
	for (i = 0; i<40;i++){
		loopCnt = DHTLIB_TIMEOUT;
		t = micros();
		while(digitalRead(pin)==LOW){
			if((micros() - t) > loopCnt)
				return DHTLIB_ERROR_TIMEOUT;
		}
		t = micros();
		loopCnt = DHTLIB_TIMEOUT;
		while(digitalRead(pin)==HIGH){
			if((micros() - t) > loopCnt){
				return DHTLIB_ERROR_TIMEOUT;
			}
		}
		if((micros() - t ) > 60){
			bits[idx] |= mask;
		}
		mask >>= 1;
		if(mask == 0){
			mask = 0x80;
			idx++;
		}
	}
	pinMode(pin,OUTPUT);
	digitalWrite(pin,HIGH);
	//printf("bits:\t%d,\t%d,\t%d,\t%d,\t%d\n",bits[0],bits[1],bits[2],bits[3],bits[4]);
	return DHTLIB_OK;
}
//Function：Read DHT sensor, analyze the data of temperature and humidity
//return：DHTLIB_OK   DHTLIB_ERROR_CHECKSUM  DHTLIB_ERROR_TIMEOUT
int readDHT11(int pin)
    {
	int rv ;
	uint8_t sum;
	rv = readSensor(pin,DHTLIB_DHT11_WAKEUP);
	if(rv != DHTLIB_OK){
		humidity = DHTLIB_INVALID_VALUE;
		temperature = DHTLIB_INVALID_VALUE;
		return rv;
	}
	humidity = bits[0];
	temperature = bits[2] + bits[3] * 0.1;
	sum = bits[0] + bits[1] + bits[2] + bits[3];
	if(bits[4] != sum)
		return DHTLIB_ERROR_CHECKSUM;
	return DHTLIB_OK;
    }

int getDHT()
{
    int chk,retVal=0;//chk:read the return value of sensor; sumCnt:times of reading sensor
    if(wiringPiSetup() == -1)
        { //when initialize wiring failed,print messageto screen
        printf("setup wiringPi failed !");
        return 1;
        }
    chk = readDHT11(DHT11_Pin); //read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
    sumCnt++;       //counting number of reading times
    percentFailure = (((float)sumCntFailures/(float)sumCnt)*100);
    //printf("chk=%x\n",chk);
    switch(chk)
        {
        case DHTLIB_OK:     //if the return value is DHTLIB_OK, the data is normal.
	    //printf("DHT11,OK! \n");
            retVal=0;
            break;
        case DHTLIB_ERROR_CHECKSUM:     //data check has errors
            //printf("DHTLIB_ERROR_CHECKSUM! \n");
            sumCntFailures++;
            retVal=1;
            break;
        case DHTLIB_ERROR_TIMEOUT:      //reading DHT times out
            //printf("DHTLIB_ERROR_TIMEOUT! \n");
            sumCntFailures++;
            retVal=1;
            break;
        case DHTLIB_INVALID_VALUE:      //other errors
            //printf("DHTLIB_INVALID_VALUE! \n");
            sumCntFailures++;
            retVal=1;
            break;
        }
    temperature = (temperature*9/5)+32;
    if(chk ==0)
        {
	printf("\n\e[1;32mCurrent Humidity is %.0f%%, \t Current Temperature is %.2fF\e[0m\n",humidity,temperature);
        if(temperature > hightemp)
            hightemp = temperature;
        if(temperature < lowtemp && temperature <100)
            lowtemp = temperature;
        if(humidity > highhumid)
            highhumid = humidity;
        if(humidity < lowhumid && humidity >0)
            lowhumid = humidity;
        }
return retVal;
}


float getSonar()
{   // get the measurement results of ultrasonic module,with unit: cm
    long pingTime;
    float distance;
    digitalWrite(trigPin,HIGH); //trigPin send 10us high level 
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    pingTime = pulseIn(echoPin,HIGH,timeOut);   //read plus time of echoPin
    distance = (float)(pingTime * 340.0/ 2.0 / 10000.0)/2.54; // the sound speed is 340m/s,and calculate distance in inches
     return distance;
}

int pulseIn(int pin, int level, int timeout)
{
   struct timeval tn, t0, t1;
   long micros;
   gettimeofday(&t0, NULL);
   micros = 0;
   while (digitalRead(pin) != level)
   {
      gettimeofday(&tn, NULL);
      if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
      micros += (tn.tv_usec - t0.tv_usec);
      if (micros > timeout) return 0;
   }
   gettimeofday(&t1, NULL);
   while (digitalRead(pin) == level)
   {
      gettimeofday(&tn, NULL);
      if (tn.tv_sec > t0.tv_sec) micros = 1000000L; else micros = 0;
      micros = micros + (tn.tv_usec - t0.tv_usec);
      if (micros > timeout) return 0;
   }
   if (tn.tv_sec > t1.tv_sec) micros = 1000000L; else micros = 0;
   micros = micros + (tn.tv_usec - t1.tv_usec);
   return micros;
}
