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
//    グローバル変数
//*********************************************************
static CAN_HandleTypeDef *h_easy_can_p;
static int internal_receive_id;
static int internal_receive_dlc;
static unsigned char internal_receive_message[8];

//*********************************************************
//    stm32f3_easy_can_init
//*********************************************************
//  [概要]
//    モジュールの初期化
//  [引数]
//    hcan              : CubeMXで生成されるCANモジュールのハンドル
//    acceptance_filter : アクセプタンスフィルタ(0X000～0X7FF)
//    filter_mask       : フィルタマスク(0X000～0X7FF)
//  [戻り値]
//    無し
//  [使用グローバル変数]
//    h_easy_can_p (W)
//    internal_receive_id (W)
//    internal_receive_dlc (W)
//    internal_receive_message (W)
//  [備考]
//    特に無し
//--------------------------------------------------------
void stm32f3_easy_can_init(CAN_HandleTypeDef *hcan, int acceptance_filter, int filter_mask)
{
	CAN_FilterTypeDef sFilterConfig;
	int i = 0;

	// 内部変数の初期化
	h_easy_can_p = hcan;
	internal_receive_id = 0;
	internal_receive_dlc = 0;
	for(i = 0; i < 8; i++) {
		internal_receive_message[i] = 0;
	}

	// フィルタ設定
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

    // 受信割り込み有効化
    HAL_CAN_ActivateNotification(h_easy_can_p, CAN_IT_RX_FIFO0_MSG_PENDING);

    // CANモジュールスタート
    HAL_CAN_Start(h_easy_can_p);

    return;
}

//*********************************************************
//    stm32f3_easy_can_get_receive_message
//*********************************************************
//  [概要]
//    受信データ取得
//  [引数]
//    receive_id     : 受信データのIDを格納する変数のポインタ
//    receive_dlc    : 受信データの長さを格納する変数のポインタ
//    receive_message: 受信データを格納する配列
//  [戻り値]
//    無し
//  [使用グローバル変数]
//    internal_receive_id (R)
//    internal_receive_dlc (R)
//    internal_receive_message (R)
//  [備考]
//    一度受信して，次にまた受信する前にこの関数をコールする必
//    要がある．そうしなければ，前のデータは破棄される．
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
//  [概要]
//    データ送信
//  [引数]
//    transmit_id     : 送信データのID
//    transmit_dlc    : 受信データの長さ
//    transmit_message: 受信データ
//  [戻り値]
//    0 : 正常終了
//    -1: 送信バッファがいっぱいで送信できない
//  [使用グローバル変数]
//    h_easy_can_p (R)
//  [備考]
//    特になし
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
//  [概要]
//    受信割り込みハンドラ．デフォルトでは何もしない．
//  [引数]
//    無し
//  [戻り値]
//    無し
//  [使用グローバル変数]
//    h_easy_can_p (R)
//  [備考]
//    受信割り込み発生時の処理は，この関数を別の場所で定義し，
//    その中で書く．
//--------------------------------------------------------
__weak void stm32f3_easy_can_interrupt_handler(void)
{
	// デフォルトでは何もしない
    return;
}

//*********************************************************
//    HALドライバの割り込み関数定義
//*********************************************************
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if(hcan->Instance == h_easy_can_p->Instance)
    {
        CAN_RxHeaderTypeDef RxHeader;
        unsigned char message[8];
        int i = 0;

        // HALドライバからデータ取得
        HAL_CAN_GetRxMessage(h_easy_can_p, CAN_RX_FIFO, &RxHeader, message);

        // データを内部変数に渡す
        internal_receive_id = RxHeader.StdId;
        internal_receive_dlc = RxHeader.DLC;
        for(i = 0; i < 8; i++) {
            internal_receive_message[i] = message[i];
        }

        // 受信割り込みハンドラのコール
        stm32f3_easy_can_interrupt_handler();
    }
}
