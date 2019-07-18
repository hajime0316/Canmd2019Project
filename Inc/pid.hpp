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
        //目的のデータの宣言
        double ideal_data=0;
        //エンコーダの値を保持する変数の宣言
        double enc=0;

        //ゲインの取得
        void set_gain(double, double, double);

        //エンコーダの値の取得
        void get_enc(double);

        //モーターコントロールデータの値の取得
        void get_ideal(double);
        
        //PID処理
        double pid_cal();
};


#endif /* PID_HPP_ */