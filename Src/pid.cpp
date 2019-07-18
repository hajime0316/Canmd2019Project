/* 
 * File:    pid.cpp
 * version: ver01.00 (2019/07/18)
 * Author:  SIVA
 *
 * Created on 2019/07/11
 */

#include "pid.hpp"
#include "main.h"

Pid::Pid(){
}
Pid::~Pid() {
}

void Pid::set_gain(double Kp_, double Ki_, double Kd_){
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;
}

void Pid::get_enc(double enc_){
    enc = enc_;
}

void Pid::get_ideal(double ideal_data_){
    ideal_data = ideal_data_;
}

void Pid::update_errors(){
    d_error = ideal_data - enc;
    p_error = enc;
    i_error += enc;
}

double Pid::pid_cal(){
    double total_error;
    total_error = Kp*p_error + Ki*i_error + Kd*d_error;
    return ideal_data + total_error;
}