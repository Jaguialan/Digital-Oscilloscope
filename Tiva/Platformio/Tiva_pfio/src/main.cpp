#include <Arduino.h>

#include "Energia.h"
#include "PeakDetection.hpp"
#include <stdint.h> // standard library for integers (used in the next libraries)
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#define SYSTEM_CLOCK 120000000

#define TIMERFREQ 1000

#define PORT1 PE_0

uint16_t i = 0;
uint16_t j = 0;

uint16_t triggerVal = 127;

float peak_p = 0;
float peak_m = 0;

bool printSine = false;

float vpp = 0;
uint16_t period = 0;
float freq = 0;

bool trigger = false;
bool triggerSend = false;
bool measure = false;

uint16_t send = 0;
uint16_t dataIn = 0;

uint16_t max = 0;
uint16_t min = 0;

uint16_t serialBuffer[640] = {0};

int serialPeaks[640] = {0};
int serialPeaks2[640] = {0};

double sineWave[640] = {2048, 2068, 2088, 2108, 2128, 2148, 2168, 2188, 2208, 2228, 2248, 2268, 2288, 2308, 2328, 2348, 2368, 2388, 2407, 2427, 2447, 2467, 2486, 2506, 2525, 2545, 2564, 2584, 2603, 2623, 2642, 2661, 2680, 2699, 2718, 2737, 2756, 2775, 2794, 2812, 2831, 2850, 2868, 2886, 2905, 2923, 2941, 2959, 2977, 2995, 3013, 3030, 3048, 3065, 3083, 3100, 3117, 3134, 3151, 3168, 3185, 3202, 3218, 3235, 3251, 3267, 3283, 3299, 3315, 3331, 3346, 3362, 3377, 3392, 3408, 3423, 3437, 3452, 3467, 3481, 3495, 3509, 3523, 3537, 3551, 3565, 3578, 3591, 3604, 3617, 3630, 3643, 3655, 3668, 3680, 3692, 3704, 3716, 3727, 3739, 3750, 3761, 3772, 3783, 3793, 3804, 3814, 3824, 3834, 3844, 3853, 3863, 3872, 3881, 3890, 3898, 3907, 3915, 3923, 3931, 3939, 3947, 3954, 3961, 3968, 3975, 3982, 3988, 3995, 4001, 4007, 4013, 4018, 4023, 4029, 4034, 4038, 4043, 4047, 4052, 4056, 4059, 4063, 4067, 4070, 4073, 4076, 4078, 4081, 4083, 4085, 4087, 4089, 4090, 4091, 4093, 4093, 4094, 4095, 4095, 4095, 4095, 4095, 4094, 4093, 4093, 4091, 4090, 4089, 4087, 4085, 4083, 4081, 4078, 4076, 4073, 4070, 4067, 4063, 4059, 4056, 4052, 4047, 4043, 4038, 4034, 4029, 4023, 4018, 4013, 4007, 4001, 3995, 3988, 3982, 3975, 3968, 3961, 3954, 3947, 3939, 3931, 3923, 3915, 3907, 3898, 3890, 3881, 3872, 3863, 3853, 3844, 3834, 3824, 3814, 3804, 3793, 3783, 3772, 3761, 3750, 3739, 3727, 3716, 3704, 3692, 3680, 3668, 3655, 3643, 3630, 3617, 3604, 3591, 3578, 3565, 3551, 3537, 3523, 3509, 3495, 3481, 3467, 3452, 3437, 3423, 3408, 3392, 3377, 3362, 3346, 3331, 3315, 3299, 3283, 3267, 3251, 3235, 3218, 3202, 3185, 3168, 3151, 3134, 3117, 3100, 3083, 3065, 3048, 3030, 3013, 2995, 2977, 2959, 2941, 2923, 2905, 2886, 2868, 2850, 2831, 2812, 2794, 2775, 2756, 2737, 2718, 2699, 2680, 2661, 2642, 2623, 2603, 2584, 2564, 2545, 2525, 2506, 2486, 2467, 2447, 2427, 2407, 2388, 2368, 2348, 2328, 2308, 2288, 2268, 2248, 2228, 2208, 2188, 2168, 2148, 2128, 2108, 2088, 2068, 2048, 2027, 2007, 1987, 1967, 1947, 1927, 1907, 1887, 1867, 1847, 1827, 1807, 1787, 1767, 1747, 1727, 1707, 1688, 1668, 1648, 1628, 1609, 1589, 1570, 1550, 1531, 1511, 1492, 1472, 1453, 1434, 1415, 1396, 1377, 1358, 1339, 1320, 1301, 1283, 1264, 1245, 1227, 1209, 1190, 1172, 1154, 1136, 1118, 1100, 1082, 1065, 1047, 1030, 1012, 995, 978, 961, 944, 927, 910, 893, 877, 860, 844, 828, 812, 796, 780, 764, 749, 733, 718, 703, 687, 672, 658, 643, 628, 614, 600, 586, 572, 558, 544, 530, 517, 504, 491, 478, 465, 452, 440, 427, 415, 403, 391, 379, 368, 356, 345, 334, 323, 312, 302, 291, 281, 271, 261, 251, 242, 232, 223, 214, 205, 197, 188, 180, 172, 164, 156, 148, 141, 134, 127, 120, 113, 107, 100, 94, 88, 82, 77, 72, 66, 61, 57, 52, 48, 43, 39, 36, 32, 28, 25, 22, 19, 17, 14, 12, 10, 8, 6, 5, 4, 2, 2, 1, 0, 0, 0, 0, 0, 1, 2, 2, 4, 5, 6, 8, 10, 12, 14, 17, 19, 22, 25, 28, 32, 36, 39, 43, 48, 52, 57, 61, 66, 72, 77, 82, 88, 94, 100, 107, 113, 120, 127, 134, 141, 148, 156, 164, 172, 180, 188, 197, 205, 214, 223, 232, 242, 251, 261, 271, 281, 291, 302, 312, 323, 334, 345, 356, 368, 379, 391, 403, 415, 427, 440, 452, 465, 478, 491, 504, 517, 530, 544, 558, 572, 586, 600, 614, 628, 643, 658, 672, 687, 703, 718, 733, 749, 764, 780, 796, 812, 828, 844, 860, 877, 893, 910, 927, 944, 961, 978, 995, 1012, 1030, 1047, 1065, 1082, 1100, 1118, 1136, 1154, 1172, 1190, 1209, 1227, 1245, 1264, 1283, 1301, 1320, 1339, 1358, 1377, 1396, 1415, 1434, 1453, 1472, 1492, 1511, 1531, 1550, 1570, 1589, 1609, 1628, 1648, 1668, 1688, 1707, 1727, 1747, 1767, 1787, 1807, 1827, 1847, 1867, 1887, 1907, 1927, 1947, 1967, 1987, 2007, 2027};

bool increasing = false;

uint16_t peakSample;
uint16_t peakSample2;

uint8_t lastPeakStatus = 0;

PeakDetection peakDetection;

unsigned long lastTime = 0;

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
  Serial7.begin(10000);
  delay(1000);
  Serial.flush();
  // Serial.println("data:,peak:,filtered");
  pinMode(PORT1, INPUT);

  // https://github.com/leandcesar/PeakDetection
  peakDetection.begin(1, 1, 0.1); // peakdetection.begin(lag,threshold,influence); 0.8
  // peakDetection.begin();

  initTimer(TIMERFREQ);
}

void loop()
{

  if (measure)
  {
    measure = false;

    peakDetection.add(dataIn);
    int peak = peakDetection.getPeak();
    uint16_t filtered = peakDetection.getFilt();

    dataIn = map(dataIn, 0, 4095, 0, 254);
    filtered = map(filtered, 0, 4095, 0, 254);

    if (filtered == triggerVal && peak == 1)
    {
      trigger = true;
    }

    if (trigger)
    {
      serialBuffer[send] = 255 - filtered;
      serialPeaks[send] = peak;
      max = max(max, serialBuffer[send]);
      min = min(min, serialBuffer[send]);
      send++;

      if (send == 640)
      {
        send = 0;
        trigger = false;
        // triggerVal = serialBuffer[639];
        Serial7.write(0b11111111);
        for (uint16_t i = 0; i < 639; i++)
        {
          //Serial.print(255 - serialBuffer[i]);
          Serial7.write(serialBuffer[i]);
          //Serial.print(",");
          //Serial.print(serialPeaks[i]);
          //Serial.print(",");
          //Serial.print(max);
          //Serial.print(",");
          //Serial.println(min);
          //serialPeaks[i] = 0;
          //serialPeaks2[i] = 0;
        }

        delay(10);
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

  measure = true;
  // dataIn = sineWave[j];
  //  Serial.println(dataIn);
  // j = (++j) % 640;

  dataIn = analogRead(PORT1);
}