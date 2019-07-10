/* 
 * File:    stm32f3_antiphase_pwm.hpp
 * version: ver01.00 (2019/06/27)
 * Author:  SIVA
 *
 * Created on 2019/06/27/, 20:55
 */

#ifndef STM32F3_ANTIPHASE_PWM_HPP_
#define STM32F3_ANTIPHASE_PWM_HPP_

#include "stm32f3xx_hal.h"

class Stm32f3AntiphasePwm {
    private:
        const TIM_HandleTypeDef *htim;

    public:
        Stm32f3AntiphasePwm(const TIM_HandleTypeDef *htim);
        ~Stm32f3AntiphasePwm();

        void update_duty(double duty_rate);
};


#endif /* STM32F3_ANITPHASE_PWM_HPP_ */
