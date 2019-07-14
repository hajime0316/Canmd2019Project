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

typedef enum tag_MdDataType {
    MD_DATA_TYPE_PID_GAIN_0 = 0b00,
    MD_DATA_TYPE_PID_GAIN_1 = 0b01,
    MD_DATA_TYPE_TIME_PARAM = 0b10,
    MD_DATA_TYPE_CONTROL_DATA = 0b11,
} MdDataType;

typedef enum tag_MotorControlMode {
    UNDEFINED,
    DUTY_RATE_MODE,
    PID_MODE,
}MotorControlMode;

typedef struct tag_MotorSetupData {
    MotorControlMode control_mode;  // PIDコントローラを有効化するかどうか
    unsigned char kp, ki, kd;       // PID制御時のゲイン(0 ～ 255)
} MotorSetupData;

#ifdef __cplusplus
extern "C" {
#endif

void canmd_manager_init(void);
MdDataType canmd_manager_set_can_receive_data(const unsigned char receive_data[], int receive_data_len);
void canmd_manager_get_motor_setup_data(MotorSetupData md_init_data[2]);
void canmd_manager_get_motor_control_data(int motor_control_data[]);
int canmd_manager_get_control_data_receive_flg(void);
int canmd_manager_time_out_check(void);
int canmd_manager_set_velocity_data(void);

#ifdef __cplusplus
};
#endif

#endif /* CANMD_MANAGER_H */
