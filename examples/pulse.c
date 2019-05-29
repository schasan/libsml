#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <ctime>

#define PULSE_PIN 1
#define CALM 10

// the event counter 
volatile long eventCounter = 0L, previous = 0L;
clock_t eventTime, previousTime, deltaTime;

void myInterrupt(void) {
	eventCounter++;
	if (eventCounter % CALM == 0) {
		eventTime = clock();
		deltaTime = eventTime - previousTime;
		previousTime = eventTime;
		printf("%f\n", 36000.0*CALM/deltaTime);
	}
}


int main(void) {
	long delta;

	if (wiringPiSetup () < 0) {
		fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
		return 1;
	}

	pinMode(PULSE_PIN, INPUT);
	pullUpDnControl(PULSE_PIN, PUD_UP);

	if (wiringPiISR (PULSE_PIN, INT_EDGE_FALLING, &myInterrupt) < 0 ) {
		fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
		return 1;
	}

	// display counter value every second.
	while (1) {
		delta = eventCounter - previous;
		previous = eventCounter;
		//printf( "%7ld %f\n", eventCounter, (float)delta);
		delay(1000); // wait 1 second
	}

	return 0;
}
