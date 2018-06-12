/**
\file       pov.c
\author     Jack Gularte - 
\date       01/13/2018

\brief      Main persistence of vision file for the static RPi controllers.

This program will run the logic of the static RPi
*/

// Linux C libraries
#include <stdio.h>	//printf, fprintf, stderr, fflush, scanf, getchar
#include <string.h>   //strncpy, strerror
#include <errno.h>	//errno
#include <stdlib.h>   //exit, EXIT_SUCCESS, EXIT_FAILURE
#include <signal.h>   //signal, SIGINT, SIGQUIT, SIGTERM
#include <wiringPi.h> //wiringPiSetup, pinMode, digitalWrite, delay, INPUT, OUTPUT, PWM_OUTPUT
#include <pthread.h>

// Local headers
#include "web_client.h" //initWebClient_new_port

// Variables and Structures
const int pin_array[] = {28, 27, 26, 24, 23, 22, 21, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
int index2, pin, index3 = 0;
int image[120];
char message[2000];

// Local function declaration
/// Function controlling exit or interrupt signals
void control_event(int sig);
void connect_s(void);
void reveive_m(void);
void ISR(void);
void *slave(void *arg);

// Threads
pthread_t cutting;

/**
main function - Entry point function for pov

@param argc number of arguments passed
@param *argv IP address of the rotor RPi

@return number stdlib:EXIT_SUCCESS exit code when no error found.
*/
int main(int argc, char *argv[])
{
	// Inform OS that control_event() function will be handling kill signals
	(void)signal(SIGINT, control_event);
	(void)signal(SIGQUIT, control_event);
	(void)signal(SIGTERM, control_event);

	//Local variable definition
	int create_success;

	printf("  My wireless IP is: %s\n", getMyIP("wlan0"));

	// Initialize the Wiring Pi facility
	printf("Initialize Wiring Pi facility... ");
	if (wiringPiSetup() != 0)
	{
		// Handles error Wiring Pi initialization
		fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
	printf("Done\n");

	// Initialize GPIO pins
	printf("Initialize GPIO pins... ");
	for (int i = 0; i < 24; i++)
	{
		pinMode(pin_array[i], OUTPUT);
	}
	printf("Done\n");

	wiringPiISR(29, INT_EDGE_RISING, &ISR);

	// Server Startup
	printf("Initialize Connection To Server...\n");
	connect_s();

	create_success = pthread_create(&cutting, NULL, slave, NULL);

	printf("Start Main loop\n");
	if (create_success == 0)
	{
		while (1)
		{
			for (pin = 0; pin < 24; pin++)
			{
				digitalWrite(pin_array[pin], image[index2] & (1 << pin));
			}
			index2++;
			index2 %= 120;
			delayMicroseconds(244);
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

void control_event(int sig)
{
	printf("\b\b  \nExiting pov... ");
	for (pin = 0; pin < 24; pin++)
	{
		digitalWrite(pin_array[pin], 0);
	}
	delay(200);
	printf("Done\n");
	exit(EXIT_SUCCESS);
}

void connect_s(void)
{
	char ip[20];
	printf("Enter the IP to send to > \n");
	fflush(stdout);
	scanf("%s", ip);
	printf("Initialize Web Client\n");
	initWebClient(ip);
}

void receive_m(void)
{
	//char message[MESSAGE_BUFFER_SIZE];
	printf("Waiting for a command...\n");
	sprintf(message, "%s", getMessage());
}

void ISR(void)
{
	if( index3 == 0)
		index2 = 61;
	index3++;
	index3 %= 2;
}

void *slave(void *arg)
{
	const char s[] = ",\n";
	char *token;
	int image_idx;
	char static_ip[20];
	char response[2000];

	while (1)
	{
		receive_m();

		token = strtok(message, s);
		sprintf(static_ip, "%s", token);
		token = strtok(NULL, s); //ignore (self IP)
		token = strtok(NULL, s);

		if (!strcmp(token, "display"))
		{
			for (int j = 0; j < 120; j++)
			{
				image[j] = 0;
			}
			token = strtok(NULL, s);
			while (token != NULL)
			{
				image_idx = strtol(token, NULL, 10);
				token = strtok(NULL, s);
				image[image_idx] = strtol(token, NULL, 16);
				token = strtok(NULL, s);
			}
			printf(response, "%s,%s,response,display,ok\n", getMyIP("wlan0"), static_ip);
		}

		else if (!strcmp(token, "test"))
		{
		}

		else if (!strcmp(token, "lock"))
		{
		}

		else if (!strcmp(token, "unlock"))
		{
		}

		else
		{
		}
	}
}