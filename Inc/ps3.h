/* 
 * File:    ps3.h
 * version: ver02.01 (2019/04/01)
 * Author:  hajime
 *
 * Created on 2018/11/19, 5:09
 */

#ifndef PS3_H
#define PS3_H

typedef volatile struct{
	// コントローラの値格納
    char up;
    char down;
    char right;
    char left;
    char sankaku;
    char batsu;
    char maru;
    char shikaku;
    char L1;
    char L2;
    char R1;
    char R2;
    char start;
    char select;
    char right_analog_updown;
    char right_analog_rightleft;
    char left_analog_updown;
    char left_analog_rightleft;
    
}Ps3;

void ps3_init(void);                        // 初期化関数．最初にこの関数を実行しなければならない
void ps3_uart_interrupt_routine(unsigned char uart_buff[], int data_size);	// UART割り込み関数内でこれを実行．
void ps3_get_data(Ps3 *ps3_data);           // 現在のコントローラの値を取得
int ps3_time_out_check(void);               // タイムアウトチェック(100ms間隔ぐらいで実行)

#endif /* PS3_H */
