//  g++ -lwiringPi -lcurl -pthread -std=gnu++11 -fpermissive -o intr-test intr-test.c

#include <wiringPi.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <thread>
#include <curl/curl.h>
#include "hec_key.h"
#include <stdio.h>

#define PULSE_PIN 1

// the event counter 
int eventCounter = 0;
auto eventClock = std::chrono::system_clock::now();
auto previousClock = eventClock;

struct WriteThis {
        const char *readptr;
        size_t sizeleft;
};

void logger(float watt, time_t tp)
{
	CURL *curl;

	std::cout << watt << '\n';
        curl = curl_easy_init();
        if (curl) {
                CURLcode res;
                struct curl_slist *hdr = NULL;
                struct WriteThis wt;

                asprintf(&(wt.readptr), "{\"event\": {\"Watt\": \"%f\", \"Now\": \"%ld\"}}", watt, tp);
                wt.sizeleft = strlen(wt.readptr);
                hdr = curl_slist_append(hdr, SPLUNK_HEC_ENERGY_P);
                curl_easy_setopt(curl, CURLOPT_URL, "https://192.168.5.201:8443/services/collector");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, wt.readptr);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, wt.sizeleft);
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdr);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                //curl_easy_setopt(curl, CURLOPT_POST, 1L);
                //curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
                //curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
                //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                res = curl_easy_perform(curl);
                if(res != CURLE_OK) fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                /* always cleanup */
                curl_easy_cleanup(curl);
                /* free the custom headers */
                curl_slist_free_all(hdr);
        }

}

void myInterrupt(void) {
	eventCounter++;
	eventClock = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = eventClock - previousClock;
	previousClock =  eventClock;
	std::cout << eventCounter;
	std::thread t(logger, 360.0/diff.count(), std::chrono::system_clock::to_time_t(eventClock));
	t.detach();
}

int main(void) {
	if (wiringPiSetup() < 0) {
		std::cerr << "Unable to setup wiringPi: " << std::strerror(errno);
		return 1;
	}

	pinMode(PULSE_PIN, INPUT);
	pullUpDnControl(PULSE_PIN, PUD_UP);

	if (wiringPiISR(PULSE_PIN, INT_EDGE_FALLING, &myInterrupt) < 0 ) {
		std::cerr << "Unable to setup ISR: " << std::strerror(errno);
		return 1;
	}

	while (1) {
		delay(1000);
	}

	return 0;
}
