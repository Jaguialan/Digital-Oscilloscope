#include <Arduino.h>

#include "Energia.h"
#include "PeakDetection.hpp"
#include <stdint.h> // standard library for integers (used in the next libraries)
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#define SYSTEM_CLOCK 120000000

#define TIMERFREQ 7000

#define PORT1 PE_0

PeakDetection peakDetection;

bool measure = false;
uint16_t dataIn;

void Timer0IntHandler();

// Prototypes

void initTimer(unsigned Hz)
{
  // GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  IntMasterEnable(); // Enable Interrupts
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
  unsigned long ulPeriod = (SYSTEM_CLOCK / Hz);
  TimerLoadSet(TIMER1_BASE, TIMER_A, ulPeriod - 1);
  TimerIntRegister(TIMER1_BASE, TIMER_A, Timer0IntHandler);
  IntEnable(INT_TIMER1A);
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER1_BASE, TIMER_A);
}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial7.begin(100000);
  delay(10000);
  Serial.flush();
  // Serial.println("data:,peak:,filtered");
  pinMode(PORT1, INPUT);

  // https://github.com/leandcesar/PeakDetection
  peakDetection.begin(1, 1, 0.1); // peakdetection.begin(lag,threshold,influence); 0.8
  initTimer(TIMERFREQ);
}

void loop()
{
  if (measure)
  {
    measure = false;
    dataIn = analogRead(PORT1);
    dataIn = map(dataIn, 0, 4095, 0, 255);
    peakDetection.add(dataIn);
    double filtered = peakDetection.getFilt();
    Serial.println(filtered*3.3/255.0);
  }
}

void Timer0IntHandler()
{
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  measure = true;
}