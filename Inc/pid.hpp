/* 
 * File:    pid.hpp
 * version: ver01.00 (2019/07/18)
 * Author:  SIVA
 *
 * Created on 2019/07/11/
 */

#ifndef PID_HPP_
#define PID_HPP_

#include "stm32f3xx_hal.h"

class Pid {
    private:

    public:
        Pid();
        ~Pid();
        //ゲインの宣言
        double Kp=0,Ki=0,Kd=0;

        //ゲインの取得
        void set_gain(double, double, double);

        //エンコーダの値の取得
        void get_enc(double);
        
        //PID処理
        double pid_cal(double);
};


#endif /* PID_HPP_ */