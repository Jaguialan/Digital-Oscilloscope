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

#include "libs/peak/PeakDetection.hpp"
#include "libs/peak/PeakDetection.cpp"

/////////////////////////////////////////////////
////////////////// DEFINITIONS //////////////////
/////////////////////////////////////////////////

#define SYSTEM_CLOCK 120000000 // System clock speed Hz

#define DEBUG 1
#define TIVA_WEB 0
#define BASYS 1

#define SERIAL_SPEED_USB 115200
#define SERIAL_SPEED_BASYS 100000
#define SERIAL_SPEED_WEB 115200

#define TIMER_FREQ 4096 // Frequency of the timer ISR
#define INTERVAL 10     // Trigger +/- interval
#define MAX_DATA 2000   // Maximun data to store
#define SEND_DATA 640   // Maximun data to send

#define CUTTOF_FREQ 6000.0f  // LPF cutoff frequency
#define SAMPLE_TIME 0.00001f // LPF sample time

/////////////////////////////////////////////////
/////////////////// VARIABLES ///////////////////
/////////////////////////////////////////////////

struct channel
{

    uint16_t dataIn[MAX_DATA] = {0};   // Analog reading data [0-2^12]
    uint16_t dataFilt[MAX_DATA] = {0}; // Filtered data
    uint8_t dataOut[SEND_DATA] = {0};  // Processed data
    uint16_t triggerVal = 2000;        // Value to trigger
    uint16_t dataIndex = 0;            // Actual data index
    uint16_t dataOutIndex = 0;         // Data to be sent index
    int peak[MAX_DATA] = {0};          // If data is increasing returns 1. If data is decreasing returns -1
    bool newData = false;              // TRUE when timer triggers
    bool trigger = false;              // TRUE if trigger is enabled
    PeakDetection peakDetection;       // Peak detector
    RCFilter lpfRC;                    // LowPass Filter

    uint16_t max_val = 0;
    uint16_t min_val = 0;
    uint16_t vpp = 0;
    float freq[SEND_DATA] = {0};
    float freq_total = 0;
    uint16_t freqIndex = 0;

    // frequency detection variables
    uint16_t aux = 0;
    uint16_t aux2 = 0;
    bool firstPeak = false;

} ch1;

enum eventsList
{
    SAMPLE,
    PROCESS,
    SEND
};

uint8_t event = SAMPLE;

/////////////////////////////////////////////////
/////////////////// PROTOTYPES //////////////////
/////////////////////////////////////////////////

void Timer0IntHandler();
void initTimer(unsigned Hz);

void setup()
{
    Serial.begin(SERIAL_SPEED_USB);    // Initiallize Serial port
    Serial6.begin(SERIAL_SPEED_WEB);   // Initiallize Serial port
    Serial7.begin(SERIAL_SPEED_BASYS); // Initiallize Serial port
    pinMode(PE_0, INPUT);              // Cannel 0 INPUT

    RCFilter_Init(&ch1.lpfRC, CUTTOF_FREQ, SAMPLE_TIME); // Init RC filter
    ch1.peakDetection.begin(1, 1, 1);                    // peakdetection.begin(lag,threshold,influence); 0.8

    initTimer(TIMER_FREQ);
}

void loop()
{
    switch (event)
    {
    case SAMPLE: // Get MAX_DATA samples
    {
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
    }
    case PROCESS:
    {
        // Filter data

        while (ch1.dataIndex < MAX_DATA)
        {
            ch1.dataFilt[ch1.dataIndex] = RCFilter_Update(&ch1.lpfRC, ch1.dataIn[ch1.dataIndex]); // Add data point to filter
            ch1.peakDetection.add(ch1.dataFilt[ch1.dataIndex]);                                   // Add filtered data point to peak detector
            ch1.peak[ch1.dataIndex] = ch1.peakDetection.getPeak();                                // Store peaks
            ch1.max_val = max(ch1.max_val, ch1.dataFilt[ch1.dataIndex]);                          // Store maximum value
            ch1.min_val = min(ch1.min_val, ch1.dataFilt[ch1.dataIndex]);                          // Store minimum value
            ch1.dataIndex++;                                                                      // Update data Index
        }
        ch1.dataIndex = 0;

        ch1.vpp = ch1.max_val - ch1.min_val;

        // Prepare data to send SEND_DATA values

        ch1.dataOutIndex = 0;

        while (ch1.dataIndex < MAX_DATA) // SEND_DATA is the max allowed lenght
        {
            if (((ch1.dataFilt[ch1.dataIndex] <= (ch1.triggerVal + INTERVAL)) && (ch1.dataFilt[ch1.dataIndex] >= (ch1.triggerVal - INTERVAL))) && (ch1.peak[ch1.dataIndex] == 1))
            {
                ch1.trigger = true;
            }

            if (ch1.trigger)
            {
                ch1.dataOut[ch1.dataOutIndex] = map(ch1.dataFilt[ch1.dataIndex], 0, 4095, 0, 254); // Save data to out vector
                ch1.dataOutIndex++;
                if (ch1.dataOutIndex == SEND_DATA) // Stop saving and compute freq
                {
                    ch1.trigger = false;
                    int lastPeak = ch1.peak[0];

                    for (ch1.dataOutIndex = 0; ch1.dataOutIndex < SEND_DATA; ch1.dataOutIndex++)
                    {
                        if (ch1.peak[ch1.dataOutIndex] == -1) // Decreasing
                        {
                            if (lastPeak == 1) // Increasing
                            {
                                if (!ch1.firstPeak)
                                {
                                    ch1.aux = ch1.dataOutIndex;
                                    ch1.firstPeak = true;
                                }
                                else
                                {
                                    ch1.aux2 = ch1.dataOutIndex;
                                    if ((ch1.aux2 - ch1.aux) >= 3)
                                    {
                                        ch1.firstPeak = false;
                                        ch1.freq[ch1.freqIndex] = ((float)(TIMER_FREQ)) / ((float)(ch1.aux2 - ch1.aux));
                                        ch1.freqIndex++;
                                        if (ch1.freqIndex == 4)
                                        {
                                            ch1.freqIndex = 0;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        lastPeak = ch1.peak[ch1.dataOutIndex];
                    }

                    ch1.freq_total = 0; // Clean frequency
                    ch1.freq_total = ch1.freq[2];

                    // vpp = max_val - min_val;

                    break;
                }
            }
            ch1.dataIndex++;
        }

        event = SEND;
        break;
    }
    case SEND:
    {

#if DEBUG
        for (ch1.dataOutIndex = 0; ch1.dataOutIndex < SEND_DATA; ch1.dataOutIndex++)
        {
            Serial.print(ch1.dataIn[ch1.dataOutIndex]);
            Serial.print(",");
            Serial.print(ch1.dataFilt[ch1.dataOutIndex]);
            Serial.print(",");
            Serial.print(ch1.dataOut[ch1.dataOutIndex]);
            Serial.print(",");
            Serial.print(ch1.peak[ch1.dataOutIndex]);
            Serial.print(",");
            Serial.print(ch1.freq_total);
            Serial.print(",");
            Serial.println(ch1.vpp);
        }
#endif

#if BASYS

        Serial7.write(0b11111111); // Starting header
        for (ch1.dataOutIndex = 0; ch1.dataOutIndex < SEND_DATA; ch1.dataOutIndex++)
        {
            Serial7.write(255 - ch1.dataOut[ch1.dataOutIndex]); // Data to Basys
        }
        Serial7.write(127); // Offset
        Serial7.write(5); //
#endif

        // Clean variables

        ch1.aux = 0;
        ch1.aux2 = 0;
        ch1.dataIndex = 0;
        ch1.firstPeak = false;
        ch1.min_val = 255;
        ch1.max_val = 0;
        ch1.vpp = 0;

        // Convert variables for webpage

        uint8_t freqInt, freqDec;
        int freqmHz;

        freqmHz = ch1.freq_total * 100;
        freqInt = freqmHz / 100; // Get integer part
        freqDec = freqmHz % 100; // Get decimal part

#if TIVA_WEB
        Serial6.write(0b11111111); // 255 Starting header
        Serial6.write(ch1.vpp);
        Serial6.write(freqInt);
        Serial6.write(freqDec);
#endif

        ch1.newData = false; // Unlock storing data for sampling
        event = SAMPLE;
        break;
    }
    default:
        break;
    }
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