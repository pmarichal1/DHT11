create: I2CLCD1602.c
	gcc -o humidity I2CLCD1602.c -g -I. -lwiringPi -lwiringPiDev
