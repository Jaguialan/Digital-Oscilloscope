#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "wiring_private.h" // library to access the PWM with configurable frequency
#include "./custom_pwm.h"
#include <stdbool.h>
#include <stdint.h>

static uint8_t pin_pwm=0;
static uint8_t flag_change_duty = false;
static uint8_t PWM_Enable = false;
static uint32_t prescaler;
static uint32_t defined_freq=1;
static uint8_t flag_noise=false; 
static uint32_t max_prescaler_noise;
static uint8_t flag_print_info = false;
static int log_prescaler = 0;
static int log_prescaler_2=0;

void PWMPeriod_TimerIntHandler() {
  MAP_TimerIntClear(TIMER7_BASE, TIMER_TIMA_TIMEOUT);
  digitalWrite(pin_pwm, 1); 
  
  if(flag_change_duty){  
    flag_print_info = true; 

    //MAP_TimerDisable(TIMER7_BASE, TIMER_A);
    MAP_TimerLoadSet(TIMER6_BASE, TIMER_A, prescaler);    
    //MAP_TimerIntEnable(TIMER6_BASE,  TIMER_TIMA_TIMEOUT); 
    //MAP_TimerEnable(TIMER7_BASE, TIMER_A);  
    flag_change_duty = false; 
  }
  if(flag_noise){
    uint32_t prescaler_noise;
    int added_noise = random(-(int)max_prescaler_noise,(int)max_prescaler_noise);
    log_prescaler = added_noise;
    if (added_noise <= -(int)prescaler){
      prescaler_noise = 0; 
    }else{
      prescaler_noise = (uint32_t)((int)prescaler+added_noise);
      // prescaler_noise = max(prescaler_noise, uint32_t(CLK_CTE)/ defined_freq);
      if(prescaler_noise > ((uint32_t)CLK_CTE)/defined_freq ){
        prescaler_noise =  ((uint32_t)CLK_CTE)/defined_freq;
      }
    }
    MAP_TimerLoadSet ( TIMER6_BASE, TIMER_A, prescaler_noise);
    flag_print_info=true;
    log_prescaler_2 = prescaler_noise;
  }

  MAP_TimerEnable ( TIMER6_BASE, TIMER_A );
  //State=!State;
}

void PWMTon_TimerIntHandler() {  
  MAP_TimerIntClear(TIMER6_BASE, TIMER_TIMA_TIMEOUT);
  digitalWrite(pin_pwm, 0);
  MAP_TimerDisable(TIMER6_BASE, TIMER_A);
}

void custom_PWM_Write(uint8_t pin, uint32_t value_max, uint32_t value, unsigned int freq){
    
    float duty=(float)value/(float)value_max;
    /*Serial.println("Duty float: ");
    Serial.println(duty);*/
  

    uint32_t prescaler_temp= (uint32_t)(CLK_CTE * duty/ freq);
   //uint32_t prescaler_temp= (((uint32_t)CLK_CTE) * value)  / (value_max * freq);
     
    if (!PWM_Enable || pin_pwm!=pin || freq!=defined_freq) {
        pin_pwm=pin;
        defined_freq=freq;
        pinMode(pin_pwm, OUTPUT); 

        MAP_SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER6);
        MAP_SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER7);

        MAP_TimerConfigure (TIMER7_BASE, TIMER_CFG_PERIODIC);
        MAP_TimerConfigure (TIMER6_BASE, TIMER_CFG_PERIODIC);
        

        MAP_TimerLoadSet ( TIMER7_BASE, TIMER_A, (uint32_t)CLK_CTE/defined_freq);         
        MAP_TimerLoadSet ( TIMER6_BASE, TIMER_A, prescaler);

        TimerIntRegister ( TIMER7_BASE, TIMER_A, &PWMPeriod_TimerIntHandler);
        TimerIntRegister ( TIMER6_BASE, TIMER_A, &PWMTon_TimerIntHandler);
        
        MAP_TimerIntEnable ( TIMER7_BASE, TIMER_TIMA_TIMEOUT); 
        
        MAP_TimerIntEnable ( TIMER6_BASE,  TIMER_TIMA_TIMEOUT); 
        
        MAP_TimerEnable ( TIMER7_BASE, TIMER_A );  
        PWM_Enable = true;
    }
    if (prescaler_temp !=prescaler) {
        flag_change_duty = true;
        /*Serial.print("Duty:   ");
        Serial.println(duty);
        Serial.print("Periodo:  ");
        Serial.println((uint32_t)CLK_CTE/defined_freq);
        //Serial.println(prescaler); */
        prescaler=prescaler_temp; 
        if( value>=value_max){ 
          MAP_TimerIntDisable ( TIMER6_BASE,  TIMER_TIMA_TIMEOUT); 
        }


    }
  

  //flag_change_duty = true;
  //MIrar cambio de frecuencia y duty 100
  //cte=uint32_t(float(CLK_CTE/(freq*100)*duty));
  //Serial.println(uint32_t(float(CLK_CTE/(freq*100)*duty)));
}

void custom_PWM_set_noise(float noise_max_duty){ //noise max 0-MAX_PRECISION duty cycle
  max_prescaler_noise= (uint32_t)((CLK_CTE * noise_max_duty)/ defined_freq);  
    
}; 

void custom_PWM_enable_noise(){
  pinMode(PE_0,OUTPUT);
  srand(analogRead(PE_0)); //Esto probablemente no funcione
  flag_noise = true;

};

void custom_PWM_disable_noise(){
  flag_noise = false;
};

void custom_PWM_stop(){

  MAP_TimerIntDisable ( TIMER7_BASE, TIMER_TIMA_TIMEOUT); 
        
  MAP_TimerIntDisable ( TIMER6_BASE,  TIMER_TIMA_TIMEOUT);

  digitalWrite(pin_pwm, 0);
};