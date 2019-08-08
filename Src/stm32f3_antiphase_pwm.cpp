/* 
 * File:    stm32f3_antiphase_pwm.cpp
 * version: ver01.00 (2019/06/27)
 * Author:  SIVA
 *
 * Created on 2019/06/27, 21:20
 */

#include "stm32f3_antiphase_pwm.hpp"
#include "main.h"

Stm32f3AntiphasePwm::Stm32f3AntiphasePwm(const TIM_HandleTypeDef *htim) {
    this->htim = htim;
    htim->Instance->CCR1 = PWM_DUTY_ZERO;
    htim->Instance->CCR2 = PWM_DUTY_ZERO;
}
Stm32f3AntiphasePwm::~Stm32f3AntiphasePwm() {

}

int Stm32f3AntiphasePwm::update_duty(double duty_rate){
    if(duty_rate > 1) return 1;
    
    double difference;
    difference = PWM_DUTY_MAX * duty_rate;
    htim->Instance->CCR1 = PWM_DUTY_ZERO + difference;
    htim->Instance->CCR2 = PWM_DUTY_ZERO - difference;

    return 0;
}