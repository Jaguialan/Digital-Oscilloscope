#include <Arduino.h>
#include <Energia.h>

#include <stdint.h> // standard library for integers (used in the next libraries)
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "wiring_private.h"

#include "libs/filters/RCfilter.h"
#include "libs/filters/RCfilter.c"

#include "libs/peak/PeakDetection.hpp";
#include "libs/peak/PeakDetection.cpp";

/////////////////////////////////////////////////
////////////////// DEFINITIONS //////////////////
/////////////////////////////////////////////////

#define SYSTEM_CLOCK 120000000 // System clock speed Hz

#define TIMER_FREQ 2000 // Frequency of the timer ISR
#define INTERVAL 2      // Trigger +/- interval
#define MAX_DATA 2000   // Maximun data to store
#define SEND_DATA 640   // Maximun data to send

#define CUTTOF_FREQ 500.0f // LPF cutoff frequency
#define SAMPLE_TIME 0.005f // LPF sample time

/////////////////////////////////////////////////
/////////////////// VARIABLES ///////////////////
/////////////////////////////////////////////////

struct channel
{
    uint16_t dataIn[MAX_DATA] = {0};   // Analog reading data [0-2^12]
    uint16_t dataFilt[MAX_DATA] = {0}; // Filtered data
    uint16_t dataOut[SEND_DATA] = {0}; // Processed data
    uint16_t triggerVal = 1000;        // Value to trigger
    uint16_t dataIndex = 0;            // Actual data index
    int peak[MAX_DATA] = {0};          // If data is increasing returns 1. If data is decreasing returns -1
    bool newData = false;              // TRUE when timer triggers
    bool trigger = false;              // TRUE if trigger is enabled
    PeakDetection peakDetection;       // Peak detector
    RCFilter lpfRC;                    // LowPass Filter
} ch1;

enum eventsList
{
    SAMPLE,
    PROCESS,
    SEND
};

uint8_t event = SAMPLE;

void Timer0IntHandler();

/////////////////////////////////////////////////
/////////////////// PROTOTYPES //////////////////
/////////////////////////////////////////////////

void Timer0IntHandler();
void initTimer(unsigned Hz);

void setup()
{
    Serial.begin(115200); // Initiallize Serial port @ 115200 baud/s
    pinMode(PE_0, INPUT); // Cannel 0 INPUT

    RCFilter_Init(&ch1.lpfRC, CUTTOF_FREQ, SAMPLE_TIME); // Init RC filter
    ch1.peakDetection.begin(1, 1, 1);                    // peakdetection.begin(lag,threshold,influence); 0.8

    initTimer(TIMER_FREQ);
}

void loop()
{
    switch (event)
    {
    case SAMPLE: // Get MAX_DATA samples
        if (ch1.newData)
        {
            ch1.dataIndex++;
            if (ch1.dataIndex == MAX_DATA)
            {
                ch1.dataIndex = 0;
                event = PROCESS;
            }
            else
            {
                ch1.newData = false;
            }
        }
        break;

    case PROCESS:

        // Filter data
        while (ch1.dataIndex < MAX_DATA)
        {
            ch1.dataFilt[ch1.dataIndex] = RCFilter_Update(&ch1.lpfRC, ch1.dataIn[ch1.dataIndex]); // Add data point to filter
            ch1.peakDetection.add(ch1.dataFilt[ch1.dataIndex]);                                   // Add filtered data point to peak detector
            ch1.peak[ch1.dataIndex] = ch1.peakDetection.getPeak();                                // Store peaks
            ch1.peak[ch1.dataIndex] = map(ch1.peak[ch1.dataIndex], -1, 1, 0, 3);
            ch1.dataIndex++; // Update data Index
        }
        ch1.dataIndex = 0;

        //Prepare data to send SEND_DATA values
        while (ch1.dataIndex < SEND_DATA)
        {
            
        }
        ch1.dataIndex = 0;

        event = SEND;
        break;

    case SEND:
        for (size_t i = 0; i < SEND_DATA; i++)
        {
            Serial.print(ch1.dataFilt[i]);
            Serial.print(",");
            Serial.println(ch1.peak[i]);
        }
        ch1.newData = true; // Unblock storing data for sampling
        event = SAMPLE;
        break;

    default:
        break;
    }
    /*
        if (ch1.newData)
        {
            RCFilter_Update(&ch1.lpfRC, (float)ch1.dataIn[ch1.dataIndex]); // Add data point to filter
            ch1.peakDetection.add(ch1.lpfRC.out[ch1.dataIndex1]);          // Add filtered data point to peak detector
            ch1.peak[ch1.dataIndex] = ch1.peakDetection.getPeak();

            ch1.peak[ch1.dataIndex] = map(ch1.peak[ch1.dataIndex], -1, 1, 0, 3700);
            ch1.newData = false;
            ch1.dataIndex++;

            if (((ch1.lpfRC.out[0] <= (ch1.triggerVal + INTERVAL)) && (ch1.lpfRC.out[0] >= (ch1.triggerVal - INTERVAL))) && (ch1.peak[0] == 1)) // Check for the trigger value to start saving data
            {
                ch1.trigger = true;
            }

            if (ch1.trigger)
            {
                Serial.print(ch1.peak[ch1.dataIndex]);
                Serial.print(",");
                Serial.println(ch1.dataIn[ch1.dataIndex]);
                ch1.newData = false;
            }

            for (uint16_t i = 0; i < MAX_DATA; i++)
            {
                Serial.print(ch1.peak[i]);
                Serial.print(",");
                Serial.println(ch1.dataIn[i]);
            }
        }
    */
}

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

void Timer0IntHandler()
{
    // Clear the timer interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    if (!ch1.newData)
    {
        ch1.newData = true;
        ch1.dataIn[ch1.dataIndex] = analogRead(PE_0);
    }
}