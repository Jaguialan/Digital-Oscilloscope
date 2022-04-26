// MISEA-UC3M 2014-15
// Timer example
// Proyectos Experimentales I
// Proyectos Experimentales II
// Copy Right Universidad Carlos III de Madrid

#include "Energia.h"
#include <stdint.h> // standard library for integers (used in the next libraries)
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "PeakDetection.h"

#define SYSTEM_CLOCK 120000000

#define TIMERFREQ 100
#define TIMERFREQ2 1

#define PORT1 PE_0

uint16_t i = 0;

float peak_p = 0;
float peak_m = 0;

bool printSine = false;

float vpp = 0;
unsigned long period = 0;
float freq = 0;


unsigned long peak1 = 0;
unsigned long peak2 = 0;

bool peak1Flag = false;
bool increase = false;

PeakDetection peakDetection;

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

void initTimer2(unsigned Hz)
{
  // GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  IntMasterEnable(); // Enable Interrupts
  TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
  unsigned long ulPeriod = (SYSTEM_CLOCK / Hz);
  TimerLoadSet(TIMER2_BASE, TIMER_A, ulPeriod - 1);
  TimerIntRegister(TIMER2_BASE, TIMER_A, Timer2IntHandler);
  IntEnable(INT_TIMER2A);
  TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER2_BASE, TIMER_A);
}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial7.begin(10000);

  Serial.println("data:,peak:,filtered");
  pinMode(PORT1, INPUT);

  // https://github.com/leandcesar/PeakDetection
  peakDetection.begin(1, 1, 0.8); // peakdetection.begin(lag,threshold,influence);
  // peakDetection.begin();

  initTimer(TIMERFREQ);
  initTimer2(TIMERFREQ2);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (printSine)
  {
    printSine = false;

    // double data = sine[i];
    double data = 255 * sin(TWO_PI * i / 180);

    peakDetection.add(data);
    int peak = peakDetection.getPeak();
    double filtered = peakDetection.getFilt();

    //    Serial.print(data);
    //    Serial.print(",");
    //    Serial.print(peak*255);
    //    Serial.print(",");
    //    Serial.println(filtered);

    i = (++i) % 360;
    if (peak == 1)
    {
      if (!increase)
      {
        increase = true;
        peak_m = data;
        vpp = (peak_p - peak_m) * 3.3 / 255;
      }

      if (!peak1Flag)
      {
        peak1Flag = true;
        peak1 = micros();
        //        Serial.print("Vp: ");
        //        Serial.print(peak_p);
        //        Serial.println("V");
      }
      else
      {
        peak1Flag = false;
        peak2 = micros();

        period = peak2 - peak1;
        freq = 1E6 / (float)period;
      }
    }
    else if (peak == -1)
    {
      if (increase)
      {
        increase = false;
        peak_p = data;
      }
    }
  }
}

void Timer0IntHandler()
{
  // Clear the timer interrupt

  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

  // Timer ISR. The corresponding flag needs to be cleared.
  // Add here the code that is going to be executed every Timer period.
  printSine = true;
}

void Timer2IntHandler()
{
  // Clear the timer interrupt

  TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

  // Timer ISR. The corresponding flag needs to be cleared.
  // Add here the code that is going to be executed every Timer period.
  Serial.print("Vpp: ");
  Serial.print(vpp);
  Serial.println("V");

  Serial.print("Period: ");
  Serial.print(period);
  Serial.println("us");

  Serial.print("Freq: ");
  Serial.print(freq);
  Serial.println("Hz");
}
