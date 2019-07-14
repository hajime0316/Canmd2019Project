/* 
 * File:    canmd_manager.c
 * version: ver01.01 (2019/03/31)
 * Author:  hajime
 *
 * Created on 2019/03/25, 21:22
 */

#include "canmd_manager.h"

//*********************************************************
//    グローバル変数
//*********************************************************
static MotorSetupData internal_motor_setup_data[2]; // モーターセットアップデータ
static int internal_motor_control_data[2];          // モーターコントロールデータ
static int motor_control_data_receive_flg;          // モーターコントロールデータ受信フラグ

//*********************************************************
//    canmd_manager_init
//*********************************************************
//  [概要]
//    モジュールの初期化
//  [引数]
//    無し
//  [戻り値]
//    無し
//  [使用グローバル変数]
//    internal_motor_setup_data (W)
//    internal_motor_control_data (W)
//    motor_control_data_receive_flg (W)
//  [備考]
//    特になし
//--------------------------------------------------------
void canmd_manager_init(void)
{
    int i = 0;
    
    for(i = 0; i < 2; i++) {
        internal_motor_setup_data[i].control_mode = UNDEFINED_CONTROL_MODE;
        internal_motor_setup_data[i].kp = UNDEFINED_PID_GAIN;
        internal_motor_setup_data[i].ki = UNDEFINED_PID_GAIN;
        internal_motor_setup_data[i].kd = UNDEFINED_PID_GAIN;
    }
    
    for(i = 0; i < 2; i++) {
        internal_motor_control_data[i] = 0;
    }
    
    motor_control_data_receive_flg = 0;    // MDコントロールデータ受信フラグクリア

    return;
}

//*********************************************************
//    canmd_manager_set_can_receive_data
//*********************************************************
//  [概要]
//    CAN通信で受信したデータをセット
//  [引数]
//    receive_data    : 受信データ
//    receive_data_len: 受信データの長さ
//  [戻り値]
//    MD_DATA_TYPE_MOTOR_SETUP_DATA_0   : 受信データはMOTOR_SETUP_DATA_0
//    MD_DATA_TYPE_MOTOR_SETUP_DATA_1   : 受信データはMOTOR_SETUP_DATA_1
//    MD_DATA_TYPE_TIME_PARAM           : 受信データはPID_PARAM
//    MD_DATA_TYPE_CONTROL_DATA         : 受信データはCONTROL_DATA
//  [使用グローバル変数]
//    internal_motor_setup_data (W)
//    internal_motor_control_data (W)
//    motor_control_data_receive_flg (W)
//  [備考]
//    特になし
//--------------------------------------------------------
MdDataType canmd_manager_set_can_receive_data(const unsigned char receive_data[], int receive_data_len)
{
    MdDataType md_data_type = 0;
    
    md_data_type = (receive_data[0] >> 6) & 0b11;
    
    switch(md_data_type) {
        case MD_DATA_TYPE_MOTOR_SETUP_DATA_0:
            if(receive_data_len == 4) {
                internal_motor_setup_data[0].kp = receive_data[1];
                internal_motor_setup_data[0].ki = receive_data[2];
                internal_motor_setup_data[0].kd = receive_data[3];
            }
            
            break;
            
        case MD_DATA_TYPE_MOTOR_SETUP_DATA_1:
            if(receive_data_len == 4) {
                internal_motor_setup_data[1].kp = receive_data[1];
                internal_motor_setup_data[1].ki = receive_data[2];
                internal_motor_setup_data[1].kd = receive_data[3];
            }

            break;
            
        case MD_DATA_TYPE_TIME_PARAM:
            
            break;
            
        case MD_DATA_TYPE_CONTROL_DATA:
            if(receive_data_len == 3) {
                
                if((receive_data[0] >> 5 & 1) == 0) {
                    internal_motor_control_data[0] = ((receive_data[0] & 0b111000) << 5) | receive_data[1];
                }
                else {
                    internal_motor_control_data[0] = (-1 << 11) | (((receive_data[0] & 0b111000) << 5) | receive_data[1]);
                }
                
                if((receive_data[0] >> 2 & 1) == 0) {
                    internal_motor_control_data[1] = ((receive_data[0] & 0b111) << 8) | receive_data[2];
                }
                else {
                    internal_motor_control_data[1] = (-1 << 11) | (((receive_data[0] & 0b111) << 8) | receive_data[2]);
                }
                
                motor_control_data_receive_flg = 1;        // MDコントロールデータ受信フラグセット
            }
            
            break;
    }
    
    return md_data_type;
}

//*********************************************************
//    canmd_manager_get_motor_setup_data
//*********************************************************
//  [概要]
//    モーターセットアップデータを取得
//  [引数]
//    motor_setup_data: モーターセットアップデータを格納するた
//                      めの配列のポインタ
//  [戻り値]
//    無し
//  [使用グローバル変数]
//    internal_motor_setup_data (R)
//  [備考]
//    特になし
//--------------------------------------------------------
void canmd_manager_get_motor_setup_data(MotorSetupData motor_setup_data[2])
{
    for(int i = 0; i < 2; i++) {
        motor_setup_data[i] = internal_motor_setup_data[i];
    }
    
    return;
}

//*********************************************************
//    canmd_manager_get_motor_control_data
//*********************************************************
//  [概要]
//    モーターコントロールデータの取得
//  [引数]
//    md_control_data: モーターコントロールデータを格納する配列のポインタ
//  [戻り値]
//    無し
//  [使用グローバル変数]
//    internal_motor_control_data (R)
//  [備考]
//    特になし
//--------------------------------------------------------
void canmd_manager_get_motor_control_data(int motor_control_data[])
{
    motor_control_data[0] = internal_motor_control_data[0];
    motor_control_data[1] = internal_motor_control_data[1];
    
    return;
}

//*********************************************************
//    canmd_manager_get_control_data_receive_flg
//*********************************************************
//  [概要]
//    モーターコントロールデータ受信フラグの取得
//  [引数]
//    無し
//  [戻り値]
//    0: モーターコントロールデータ受信フラグは0(受信してない)
//    1: モーターコントロールデータ受信フラグは1(受信した)
//  [使用グローバル変数]
//    motor_control_data_receive_flg (R)
//  [備考]
//    特になし
//--------------------------------------------------------
int canmd_manager_get_control_data_receive_flg(void)
{
    return motor_control_data_receive_flg;
}

//*********************************************************
//    canmd_manager_time_out_check
//*********************************************************
//  [概要]
//    モーターコントロールデータ受信のタイムアウトチェック
//  [引数]
//    無し
//  [戻り値]
//    0: 正常終了
//    1: タイムアウト
//  [使用グローバル変数]
//    motor_control_data_receive_flg (R/W)
//    internal_motor_control_data(W)
//  [備考]
//    100ms間隔で定期的にコールする
//--------------------------------------------------------
int canmd_manager_time_out_check(void)
{
    if(motor_control_data_receive_flg == 1) {
        motor_control_data_receive_flg = 0;
        return 0;
    }
    else {
        internal_motor_control_data[0] = 0;
        internal_motor_control_data[1] = 0;
        return 1;
    }
}
