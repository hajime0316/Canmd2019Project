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

#define DEFAULT_LOOP_TIME 0.005
#define PID_OUTPUT_MAX 100

class Pid {
    private:
        //各誤差
        int p_error_=0;
        int i_error_=0;
        int d_error_=0;

        //ゲイン
        double Kp_=0;
        double Ki_=0;
        double Kd_=0;

        //目的のデータ
        int control_data_=0;

        //エンコーダの値
        int enc_=0;

        //１つ前のdiff_dataの値
        int prev_diff_data_=0;

        //モーターの速度
        int velocity_=0;

        //ループタイム 
        double loop_time_ = DEFAULT_LOOP_TIME;

        //エンコーダの値の取得
        void set_enc(int);

        //モーターコントロールデータの値の取得
        void set_control_data(int);
        
        //誤差の更新
        void update_errors();

    public:
        //コンストラクタ
        Pid(double, double, double);

        //デストラクタ
        ~Pid();

        //PID処理
        double pid_calc(int, int);
};


#endif /* PID_HPP_ */