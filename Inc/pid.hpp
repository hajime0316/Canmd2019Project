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
        //各誤差
        double p_error=0;
        double i_error=0;
        double d_error=0;

        //ゲイン
        double Kp=0;
        double Ki=0;
        double Kd=0;

        //目的のデータ
        double ideal_data=0;

        //エンコーダの値
        double enc=0;

        //目的とエンコーダの値の差
        double diff_data = ideal_data - enc;

        //１つ前のdiff_dataの値
        double prev_diff_data=0;

    public:
        //コンストラクタ
        Pid();

        //デストラクタ
        ~Pid();
        
        //ゲインの取得
        void set_gain(double, double, double);

        //エンコーダの値の取得
        void get_enc(double);

        //モーターコントロールデータの値の取得
        void get_ideal(double);
        
        //誤差の更新
        void update_errors();

        //PID処理
        double pid_cal();
};


#endif /* PID_HPP_ */