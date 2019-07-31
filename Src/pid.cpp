/* 
 * File:    pid.cpp
 * version: ver01.00 (2019/07/18)
 * Author:  SIVA
 *
 * Created on 2019/07/11
 */

#include "pid.hpp"
#include "main.h"

Pid::Pid(double Kp_, double Ki_, double Kd_){
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;
}

Pid::~Pid() {
}

void Pid::get_enc(int enc_){
    enc = enc_;
}

void Pid::get_ideal(int ideal_data_){
    ideal_data = ideal_data_;
}

void Pid::update_errors(){
    int diff_data = ideal_data - enc;
    d_error = diff_data - prev_diff_data;
    p_error = diff_data;
    i_error += diff_data;

    prev_diff_data = diff_data;
}

double Pid::pid_cal(){
    double total_error;
    total_error = Kp*p_error + Ki*i_error*loop_time + Kd*d_error;
    velocity = velocity + total_error*loop_time;
    return velocity;
}