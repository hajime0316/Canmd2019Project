/* 
 * File:    stm32f3_easy_can.c
 * version: ver02.00 (2019/04/01)
 * Author:  hajime
 *
 * Created on 2019/03/26, 22:33
 */

#include "stm32f3_easy_can.h"

#define CAN_RX_FIFO CAN_RX_FIFO0

//*********************************************************
//    �O���[�o���ϐ�
//*********************************************************
static CAN_HandleTypeDef *h_easy_can_p;
static int internal_receive_id;
static int internal_receive_dlc;
static unsigned char internal_receive_message[8];

//*********************************************************
//    stm32f3_easy_can_init
//*********************************************************
//  [�T�v]
//    ���W���[���̏�����
//  [����]
//    hcan              : CubeMX�Ő��������CAN���W���[���̃n���h��
//    acceptance_filter : �A�N�Z�v�^���X�t�B���^(0X000�`0X7FF)
//    filter_mask       : �t�B���^�}�X�N(0X000�`0X7FF)
//  [�߂�l]
//    ����
//  [�g�p�O���[�o���ϐ�]
//    h_easy_can_p (W)
//    internal_receive_id (W)
//    internal_receive_dlc (W)
//    internal_receive_message (W)
//  [���l]
//    ���ɖ���
//--------------------------------------------------------
void stm32f3_easy_can_init(CAN_HandleTypeDef *hcan, int acceptance_filter, int filter_mask)
{
	CAN_FilterTypeDef sFilterConfig;
	int i = 0;

	// �����ϐ��̏�����
	h_easy_can_p = hcan;
	internal_receive_id = 0;
	internal_receive_dlc = 0;
	for(i = 0; i < 8; i++) {
		internal_receive_message[i] = 0;
	}

	// �t�B���^�ݒ�
	sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
    sFilterConfig.FilterIdHigh = acceptance_filter << 5;
    sFilterConfig.FilterIdLow = acceptance_filter << 5;
    sFilterConfig.FilterMaskIdHigh = filter_mask << 5;
    sFilterConfig.FilterMaskIdLow = filter_mask << 5;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(h_easy_can_p,&sFilterConfig);

    // ��M���荞�ݗL����
    HAL_CAN_ActivateNotification(h_easy_can_p, CAN_IT_RX_FIFO0_MSG_PENDING);

    // CAN���W���[���X�^�[�g
    HAL_CAN_Start(h_easy_can_p);

    return;
}

//*********************************************************
//    stm32f3_easy_can_get_receive_message
//*********************************************************
//  [�T�v]
//    ��M�f�[�^�擾
//  [����]
//    receive_id     : ��M�f�[�^��ID���i�[����ϐ��̃|�C���^
//    receive_dlc    : ��M�f�[�^�̒������i�[����ϐ��̃|�C���^
//    receive_message: ��M�f�[�^���i�[����z��
//  [�߂�l]
//    ����
//  [�g�p�O���[�o���ϐ�]
//    internal_receive_id (R)
//    internal_receive_dlc (R)
//    internal_receive_message (R)
//  [���l]
//    ��x��M���āC���ɂ܂���M����O�ɂ��̊֐����R�[������K
//    �v������D�������Ȃ���΁C�O�̃f�[�^�͔j�������D
//--------------------------------------------------------
void stm32f3_easy_can_get_receive_message(int *receive_id, int *receive_dlc, unsigned char receive_message[])
{
    int i = 0;

	*receive_id = internal_receive_id;
    *receive_dlc = internal_receive_dlc;
    for(i = 0; i < internal_receive_dlc; i++) {
        receive_message[i] = internal_receive_message[i];
    }

    return;
}

//*********************************************************
//    stm32f3_easy_can_transmit_message
//*********************************************************
//  [�T�v]
//    �f�[�^���M
//  [����]
//    transmit_id     : ���M�f�[�^��ID
//    transmit_dlc    : ��M�f�[�^�̒���
//    transmit_message: ��M�f�[�^
//  [�߂�l]
//    0 : ����I��
//    -1: ���M�o�b�t�@�������ς��ő��M�ł��Ȃ�
//  [�g�p�O���[�o���ϐ�]
//    h_easy_can_p (R)
//  [���l]
//    ���ɂȂ�
//--------------------------------------------------------
int stm32f3_easy_can_transmit_message(int transmit_id, int transmit_dlc, const unsigned char transmit_message[])
{
    static uint32_t TxMailbox = CAN_TX_MAILBOX0;
    CAN_TxHeaderTypeDef TxHeader;
    unsigned char message[8];
    int i = 0;

    if(HAL_CAN_IsTxMessagePending(h_easy_can_p, TxMailbox)) return -1;

    TxHeader.StdId = transmit_id;
    TxHeader.ExtId = 0;
    TxHeader.IDE = 0;
    TxHeader.RTR = 0;
    TxHeader.DLC = transmit_dlc;
    for(i = 0; i < transmit_dlc; i++) {
    	message[i] = transmit_message[i];
    }

    HAL_CAN_AddTxMessage(h_easy_can_p, &TxHeader, message, &TxMailbox);

    return 0;
}

//*********************************************************
//    stm32f3_easy_can_interrupt_handler
//*********************************************************
//  [�T�v]
//    ��M���荞�݃n���h���D�f�t�H���g�ł͉������Ȃ��D
//  [����]
//    ����
//  [�߂�l]
//    ����
//  [�g�p�O���[�o���ϐ�]
//    h_easy_can_p (R)
//  [���l]
//    ��M���荞�ݔ������̏����́C���̊֐���ʂ̏ꏊ�Œ�`���C
//    ���̒��ŏ����D
//--------------------------------------------------------
__weak void stm32f3_easy_can_interrupt_handler(void)
{
	// �f�t�H���g�ł͉������Ȃ�
    return;
}

//*********************************************************
//    HAL�h���C�o�̊��荞�݊֐���`
//*********************************************************
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if(hcan->Instance == h_easy_can_p->Instance)
    {
        CAN_RxHeaderTypeDef RxHeader;
        unsigned char message[8];
        int i = 0;

        // HAL�h���C�o����f�[�^�擾
        HAL_CAN_GetRxMessage(h_easy_can_p, CAN_RX_FIFO, &RxHeader, message);

        // �f�[�^������ϐ��ɓn��
        internal_receive_id = RxHeader.StdId;
        internal_receive_dlc = RxHeader.DLC;
        for(i = 0; i < 8; i++) {
            internal_receive_message[i] = message[i];
        }

        // ��M���荞�݃n���h���̃R�[��
        stm32f3_easy_can_interrupt_handler();
    }
}
