/*
 * Module : stm32f3_velocity
 * File   : stm32f3_velocity.cpp
 * Version: 01.00
 * Author : hajime
 * 
 * Created on 2019/06/19, 09:38
 */

#include <math.h>
#include "stm32f3_velocity.hpp"
#include "main.h"

#ifndef MAX_ENCODER_COUNT
    #error MAX_ENCODER_COUNTが定義されていません．\
           CubeMXの[User Constants]でこのマクロ定\
           数を定義してください．
#endif

Stm32f3Velocity::Stm32f3Velocity(const TIM_HandleTypeDef *htim) {
    this->htim = htim;
    this->velocity = 0;
    this->past_encoder_count = htim->Instance->CNT;
    
    // ハードウェアスタート
    HAL_TIM_Encoder_Start((TIM_HandleTypeDef *)htim, TIM_CHANNEL_ALL);
}

Stm32f3Velocity::Stm32f3Velocity(const TIM_HandleTypeDef *htim, int is_reverse_turn) {
    this->htim = htim;
    this->velocity = 0;
    this->past_encoder_count = htim->Instance->CNT;

    if(is_reverse_turn) {
        this->velocity_sign = -1;
    }
    else {
        this->velocity_sign = 1;
    }

    // ハードウェアスタート
    HAL_TIM_Encoder_Start((TIM_HandleTypeDef *)htim, TIM_CHANNEL_ALL);
}

Stm32f3Velocity::~Stm32f3Velocity() {

}

int Stm32f3Velocity::periodic_calculate_velocity(void) {
    int present_encoder_count = htim->Instance->CNT;
    
    velocity = present_encoder_count - past_encoder_count;

    if(velocity > MAX_ENCODER_COUNT / 2) {
        velocity -= (MAX_ENCODER_COUNT + 1);
    }
    else if(velocity < -(MAX_ENCODER_COUNT / 2)) {
        velocity += (MAX_ENCODER_COUNT + 1);
    }

    past_encoder_count = present_encoder_count;

    return velocity;
}

int Stm32f3Velocity::get_velocity(void) {
    return velocity;
}