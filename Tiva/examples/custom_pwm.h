
#ifndef custom_pwm_h
#define custom_pwm_h

#define CLK_CTE 120000000   //Clock frequency
#define MAX_PRECISION 10000

void PMWPeriod_TimerIntHandler();
void PMWTon_TimerIntHandler();
void custom_PWM_Write(uint8_t pin, uint32_t value_max, uint32_t value, unsigned int freq);
void custom_PWM_set_noise(float noise_max_duty); //noise max 0-MAX_PRECISION duty cycle
void custom_PWM_enable_noise();
void custom_PWM_disable_noise();
void custom_PWM_stop();
#endif