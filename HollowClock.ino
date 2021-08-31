// Hollow Clock improved by SnowHead, Mart 2021
//
// Thanks to:
// shiura for stepper motor routines
// SnowHead for Internet-Time

#include "WTAClient.h"

#define ROTATION_PERIOD_IN_SEC   1
#define CREEPING_HANDS			 0		// set to 1 for smooth hands movement (needs more current, motor becomes hotter)

extern unsigned long askFrequency;

// Motor and clock parameters
#define STEPS_PER_ROTATION 4096L 		// steps of a single rotation of motor (motor: 64 steps/rotation, gear: 1/64)
#define RATIO 				   5		// minutes per a rotation (minutes wheel: 120 teeth, motors pinion: 10 teeth)
// wait for a single step of stepper
#define HIGH_SPEED_DELAY	   3		// minimal delay for highest speed, delay is calculated from steps and period
unsigned int delaytime;

//=== CLOCK ===
WTAClient wtaClient;
unsigned long locEpoch = 0, netEpoch = 0, start_time;

#define LED		16
#define PHASES  8

// ports used to control the stepper motor
// if your motor rotate to the opposite direction,
// change the order as {15, 13, 12, 14};
int port[4] =
{ 14, 12, 13, 15 };

int seq[PHASES][4] =
#if (PHASES == 4)
{
{ HIGH, LOW,  HIGH,  LOW },
{ LOW,  HIGH, HIGH, LOW },
{ LOW,  HIGH, LOW,  HIGH },
{ HIGH, LOW,  LOW,  HIGH },
#else
// sequence of stepper motor control
{
{ LOW,  HIGH, HIGH, LOW },
{ LOW,  LOW,  HIGH, LOW },
{ LOW,  LOW,  HIGH, HIGH },
{ LOW,  LOW,  LOW,  HIGH },
{ HIGH, LOW,  LOW,  HIGH },
{ HIGH, LOW,  LOW,  LOW },
{ HIGH, HIGH, LOW,  LOW },
{ LOW,  HIGH, LOW,  LOW },
#endif
};

void rotate(int step)
{
	static int phase = 0;
	int i, j;
	int delta = (step > 0) ? 1 : 7;
#if DEBUG
	unsigned long ts = millis();

	Serial.print("rotating steps: ");
	Serial.println(step);
#endif

	step = (step > 0) ? step : -step;
	for (j = 0; j < step; j++)
	{
		phase = (phase + delta) % PHASES;
		for (i = 0; i < 4; i++)
		{
			digitalWrite(port[i], seq[phase][i]);
		}
#if DEBUG
		digitalWrite(LED, j & 1);
#endif
#if CREEPING_HANDS
		delay(delaytime);
#else
		delay(HIGH_SPEED_DELAY);
#endif
	}
	// power cut

    for (i = 0; i < 4; i++)
	{
		digitalWrite(port[i], LOW);
	}

#if DEBUG
	digitalWrite(LED, 1);
	Serial.print("needed milliseconds: ");
	Serial.println(millis()-ts);
#endif
#if ! CREEPING_HANDS
	if(digitalRead(0))
	{
		delay(100);
	}
#endif
}

long calc_step(int sec)
{
	long steps;
	static double round_accu = 0.0;
	double fsteps = (double)STEPS_PER_ROTATION * sec / 60.0 / (double)RATIO;
	double tsteps;

	round_accu += modf(fsteps, &tsteps);
	steps = (long)tsteps;
	round_accu = modf(round_accu, &tsteps);
	steps += (long)tsteps;

#if CREEPING_HANDS
	if(steps)
	{
		delaytime = ((ROTATION_PERIOD_IN_SEC * 1000L) - 250) / labs(steps);
		if(delaytime < HIGH_SPEED_DELAY)
		{
			delaytime = HIGH_SPEED_DELAY;
		}
	}
#endif
	return steps;
}

void setup()
{
	start_time = millis();

	pinMode(port[0], OUTPUT);
	pinMode(port[1], OUTPUT);
	pinMode(port[2], OUTPUT);
	pinMode(port[3], OUTPUT);
	pinMode(0, INPUT_PULLUP);
#if DEBUG
	pinMode(LED, OUTPUT);
	digitalWrite(LED, 1);
#endif

	Serial.begin(115200);
	wtaClient.Setup();
	askFrequency = 50;
}

void loop()
{
#if ! DEBUG
	if(!digitalRead(0))
	{
		delaytime = HIGH_SPEED_DELAY;
		while(!digitalRead(0))
		{
			rotate(1);
			Serial.println("teststep");
			delay(200);
		}
	}
#endif

#if (ROTATION_PERIOD_IN_SEC == 1)
	while(((netEpoch = wtaClient.GetCurrentTime()) == locEpoch) || (!netEpoch))
#else
	while(((netEpoch = wtaClient.GetCurrentTime()) % ROTATION_PERIOD_IN_SEC) || (!netEpoch))
#endif
	{
		delay(100);
	}
	askFrequency = 60 * 60 * 1000;
	if(locEpoch)
	{
		wtaClient.PrintTime();
		if(start_time)
		{
			delay(1000);
			locEpoch -= (millis() - start_time) / 1000;
			start_time = 0;
		}
		rotate(calc_step(netEpoch - locEpoch));
	}
	else
	{
		delay(1000);
	}
	locEpoch = netEpoch;
}
