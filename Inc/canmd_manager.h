/* 
 * File:    canmd_manager.h
 * version: ver01.01 (2019/03/31)
 * Author:  hajime
 *
 * Created on 2019/03/25, 21:22
 */
#ifndef CANMD_MANAGER_H
#define CANMD_MANAGER_H

#define MOTOR_CONTROL_DATA_MAX 1023

typedef struct tag_MdInitData {
    double kp[2], ki[2], kd[2];     // 送られてきたデータを10で割る
    int control_loop_time_scale;    // 
    int pwm_period_scale;           // 
    int enable_pid_controller_flg;  // PIDコントローラを有効化するかどうか
} MdInitData;

typedef enum tag_MdDataType {
    MD_DATA_TYPE_PID_GAIN_0 = 0b00,
    MD_DATA_TYPE_PID_GAIN_1 = 0b01,
    MD_DATA_TYPE_TIME_PARAM = 0b10,
    MD_DATA_TYPE_CONTROL_DATA = 0b11,
} MdDataType;

void canmd_manager_init(void);
MdDataType canmd_manager_set_can_receive_data(const unsigned char receive_data[], int receive_data_len);
void canmd_manager_get_md_init_data(MdInitData *md_init_data);
void canmd_manager_get_motor_control_data(int motor_control_data[]);
int canmd_manager_get_control_data_receive_flg(void);
int canmd_manager_time_out_check(void);
int canmd_manager_set_velocity_data(void);

#endif /* CANMD_MANAGER_H */
