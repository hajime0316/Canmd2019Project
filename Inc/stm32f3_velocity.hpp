/*
 * Module : stm32f3_velocity
 * File   : stm32f3_velocity.cpp
 * Version: 01.00
 * Author : hajime
 * 
 * Created on 2019/06/19, 09:39
 */

#ifndef STM32F3_VELOCITY_HPP_
#define STM32F3_VELOCITY_HPP_

#include "stm32f3xx_hal.h"

class Stm32f3Velocity {
private:
    const TIM_HandleTypeDef *htim;
    int velocity;
    int past_encoder_count;
    int velocity_sign;
public:
    Stm32f3Velocity(const TIM_HandleTypeDef *htim);
    Stm32f3Velocity(const TIM_HandleTypeDef *htim, int is_reverse_turn);
    ~Stm32f3Velocity();
    
    int periodic_calculate_velocity(void);
    int get_velocity(void);
    void reverse_rotation();
};

#endif  /* STM32F3_VELOCITY_HPP */