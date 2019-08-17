/* 
 * File:    pid.cpp
 * version: ver01.00 (2019/07/18)
 * Author:  SIVA
 *
 * Created on 2019/07/11
 */

#include "pid.hpp"
#include "main.h"
#include "canmd_manager.h"

Pid::Pid(double Kp, double Ki, double Kd){
    Kp_ = Kp;
    Ki_ = Ki;
    Kd_ = Kd;
}

Pid::~Pid() {
}

void Pid::set_enc(int enc){
    enc_ = enc;
}

void Pid::set_control_data(int control_data){
    control_data_ = control_data;
}

void Pid::update_errors(){
    int diff_data = control_data_ - enc_;
    d_error_ = diff_data - prev_diff_data_;
    p_error_ = diff_data;
    i_error_ += diff_data;

    prev_diff_data_ = diff_data;
}

int Pid::check_value(){
    if(control_data_ < 0 || control_data_ > MOTOR_CONTROL_DATA_MAX)   return -1;
    if(enc_ < -MAX_ENCODER_COUNT || enc_ > MAX_ENCODER_COUNT) return -2;

    //正常
    return 0;
}

double Pid::pid_calc(int enc, int control_data){
    double total_error;

    set_enc(enc);
    set_control_data(control_data);

    //変数の値が異常なら処理をしない
    if(check_value() < 0) return 0;

    update_errors();
    
    total_error = Kp_*p_error_ + Ki_*i_error_*loop_time_ + Kd_*d_error_;
    velocity_ = velocity_ + total_error*loop_time_;
    return velocity_;
}