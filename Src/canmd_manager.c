/* 
 * File:    canmd_manager.c
 * version: ver01.01 (2019/03/31)
 * Author:  hajime
 *
 * Created on 2019/03/25, 21:22
 */

#include "canmd_manager.h"

//*********************************************************
//    �O���[�o���ϐ�
//*********************************************************
static MdInitData internal_md_init_data;            // MD�������f�[�^
static int internal_motor_control_data[2];          // ���[�^�[�R���g���[���f�[�^
static int motor_control_data_receive_flg;          // ���[�^�[�R���g���[���f�[�^��M�t���O

//*********************************************************
//    canmd_manager_init
//*********************************************************
//  [�T�v]
//    ���W���[���̏�����
//  [����]
//    ����
//  [�߂�l]
//    ����
//  [�g�p�O���[�o���ϐ�]
//    internal_md_init_data (W)
//    internal_motor_control_data (W)
//    motor_control_data_receive_flg (W)
//  [���l]
//    ���ɂȂ�
//--------------------------------------------------------
void canmd_manager_init(void)
{
    int i = 0;
    
    for(i = 0; i < 2; i++) {
        internal_md_init_data.kp[i] = -1;
        internal_md_init_data.ki[i] = -1;
        internal_md_init_data.kd[i] = -1;
    }
    internal_md_init_data.control_loop_time_scale = -1;
    internal_md_init_data.pwm_period_scale = -1;
    
    for(i = 0; i < 2; i++) {
        internal_motor_control_data[i] = 0;
    }
    
    motor_control_data_receive_flg = 0;    // MD�R���g���[���f�[�^��M�t���O�N���A

    return;
}

//*********************************************************
//    canmd_manager_get_can_receive_data
//*********************************************************
//  [�T�v]
//    CAN�ʐM�Ŏ�M�����f�[�^���Z�b�g
//  [����]
//    receive_data    : ��M�f�[�^
//    receive_data_len: ��M�f�[�^�̒���
//  [�߂�l]
//    MD_DATA_TYPE_PID_GAIN_0   : ��M�f�[�^��PID_GAIN_0
//    MD_DATA_TYPE_PID_GAIN_1   : ��M�f�[�^��PID_GAIN_1
//    MD_DATA_TYPE_TIME_PARAM   : ��M�f�[�^��PID_PARAM
//    MD_DATA_TYPE_CONTROL_DATA: ��M�f�[�^��PID_CONTROL_DATA
//  [�g�p�O���[�o���ϐ�]
//    internal_md_init_data (W)
//    internal_motor_control_data (W)
//    motor_control_data_receive_flg (W)
//  [���l]
//    ���ɂȂ�
//--------------------------------------------------------
MdDataType canmd_manager_set_can_receive_data(const unsigned char receive_data[], int receive_data_len)
{
    MdDataType md_data_type = 0;
    
    md_data_type = (receive_data[0] >> 6) & 0b11;
    
    switch(md_data_type) {
        case MD_DATA_TYPE_PID_GAIN_0:
            if(receive_data_len == 4) {
                internal_md_init_data.kp[0] = receive_data[1] / 10.0;
                internal_md_init_data.ki[0] = receive_data[2] / 10.0;
                internal_md_init_data.kd[0] = receive_data[3] / 10.0;
            }
            
            break;
            
        case MD_DATA_TYPE_PID_GAIN_1:
            if(receive_data_len == 4) {
                internal_md_init_data.kp[1] = receive_data[1] / 10.0;
                internal_md_init_data.ki[1] = receive_data[2] / 10.0;
                internal_md_init_data.kd[1] = receive_data[3] / 10.0;
            }

            break;
            
        case MD_DATA_TYPE_TIME_PARAM:
            if(receive_data_len == 3) {
                internal_md_init_data.control_loop_time_scale = receive_data[1] / 10.0;
                internal_md_init_data.pwm_period_scale = receive_data[2];
            }
            
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
                
                motor_control_data_receive_flg = 1;        // MD�R���g���[���f�[�^��M�t���O�Z�b�g
            }
            
            break;
    }
    
    return md_data_type;
}

//*********************************************************
//    canmd_manager_get_md_init_data
//*********************************************************
//  [�T�v]
//    MD�������f�[�^�̎擾
//  [����]
//    md_init_data: MD�������f�[�^���i�[����\���̂̃|�C���^ 
//  [�߂�l]
//    ����
//  [�g�p�O���[�o���ϐ�]
//    internal_md_init_data (R)
//  [���l]
//    ���ɂȂ�
//--------------------------------------------------------
void canmd_manager_get_md_init_data(MdInitData *md_init_data)
{
    *md_init_data = internal_md_init_data;
    
    return;
}

//*********************************************************
//    canmd_manager_get_motor_control_data
//*********************************************************
//  [�T�v]
//    ���[�^�[�R���g���[���f�[�^�̎擾
//  [����]
//    md_control_data: ���[�^�[�R���g���[���f�[�^���i�[����z��̃|�C���^
//  [�߂�l]
//    ����
//  [�g�p�O���[�o���ϐ�]
//    internal_motor_control_data (R)
//  [���l]
//    ���ɂȂ�
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
//  [�T�v]
//    ���[�^�[�R���g���[���f�[�^��M�t���O�̎擾
//  [����]
//    ����
//  [�߂�l]
//    0: ���[�^�[�R���g���[���f�[�^��M�t���O��0(��M���ĂȂ�)
//    1: ���[�^�[�R���g���[���f�[�^��M�t���O��1(��M����)
//  [�g�p�O���[�o���ϐ�]
//    motor_control_data_receive_flg (R)
//  [���l]
//    ���ɂȂ�
//--------------------------------------------------------
int canmd_manager_get_control_data_receive_flg(void)
{
    return motor_control_data_receive_flg;
}

//*********************************************************
//    canmd_manager_time_out_check
//*********************************************************
//  [�T�v]
//    ���[�^�[�R���g���[���f�[�^��M�̃^�C���A�E�g�`�F�b�N
//  [����]
//    ����
//  [�߂�l]
//    0: ����I��
//    1: �^�C���A�E�g
//  [�g�p�O���[�o���ϐ�]
//    motor_control_data_receive_flg (R/W)
//    internal_motor_control_data(W)
//  [���l]
//    100ms�Ԋu�Œ���I�ɃR�[������
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

//*********************************************************
//    canmd_manager_time_out_check
//*********************************************************
//  [�T�v]
//    ���[�^�[�R���g���[���f�[�^��M�̃^�C���A�E�g�`�F�b�N
//  [����]
//    ����
//  [�߂�l]
//    0: ����I��
//    1: �^�C���A�E�g
//  [�g�p�O���[�o���ϐ�]
//    motor_control_data_receive_flg (R/W)
//    internal_motor_control_data(W)
//  [���l]
//    100ms�Ԋu�Œ���I�ɃR�[������
//--------------------------------------------------------
int canmd_manager_set_velocity_data(void)
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
