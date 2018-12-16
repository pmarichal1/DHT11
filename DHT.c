/**********************************************************************
* Filename    : DHT.cpp
* Description : DHT Temperature & Humidity Sensor library for Raspberry
* Author      : freenove
* modification: 2018/03/07
**********************************************************************/

#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
////read return flag of sensor
#define DHTLIB_OK               0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2
#define DHTLIB_INVALID_VALUE    -999

#define DHTLIB_DHT11_WAKEUP     18
#define DHTLIB_DHT_WAKEUP       1

#define DHTLIB_TIMEOUT          100


double humidity,temperature;    //use to store temperature and humidity data read
int readDHT11(int pin);     //read DHT11
uint8_t bits[5];    //Buffer to receiver data
int readSensor(int pin,int wakeupDelay);    //

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
int readDHT11(int pin){
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
#define DHT11_Pin  3    //define the pin of sensor

int getDHT()
    {
    float hightemp=0,lowtemp=100,highhumid=0,lowhumid=100;
    int chk,sumCnt;//chk:read the return value of sensor; sumCnt:times of reading sensor
    if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
        printf("setup wiringPi failed !");
        return 1;
    }
 //   while(1){
        chk = readDHT11(DHT11_Pin); //read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
        sumCnt++;       //counting number of reading times
        printf("The sumCnt is : %d \n",sumCnt);
        printf("chk=%x\n",chk);
        switch(chk){
            case DHTLIB_OK:     //if the return value is DHTLIB_OK, the data is normal.
                printf("DHT11,OK! \n");
                break;
            case DHTLIB_ERROR_CHECKSUM:     //data check has errors
                printf("DHTLIB_ERROR_CHECKSUM! \n");
                break;
            case DHTLIB_ERROR_TIMEOUT:      //reading DHT times out
                printf("DHTLIB_ERROR_TIMEOUT! \n");
                break;
            case DHTLIB_INVALID_VALUE:      //other errors
                printf("DHTLIB_INVALID_VALUE! \n");
                break;
        }
        temperature = (temperature*9/5)+32;
        printf("Humidity is %.2f %%, \t Temperature is %.2f *F\n\n",humidity,temperature);
        if(chk ==0)
        {
        if(temperature > hightemp)
            hightemp = temperature;
        if(temperature < lowtemp && temperature <100)
            lowtemp = temperature;
        if(humidity > highhumid)
            highhumid = humidity;
        if(humidity < lowhumid && humidity >0)
            lowhumid = humidity;
        printf("High Humidity is %.2f %%, \t High Temperature is %.2f *F\n\n",highhumid,hightemp);
        printf("Low Humidity is %.2f %%, \t Low Temperature is %.2f *F\n\n",lowhumid,lowtemp);
        }
        delay(3000);
 //   }
    return 1;
}



