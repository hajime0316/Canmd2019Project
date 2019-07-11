/* 
 * File:    ps3.c
 * version: ver02.01 (2019/04/01)
 * Author:  hajime
 *
 * Created on 2018/11/19, 5:09
 */

#include "ps3.h"

#define PS3_BUFF_SIZE 16

static Ps3 internal_ps3_data;
static unsigned char ps3_buff[PS3_BUFF_SIZE];
static int ps3_buff_cnt;
static int time_out_check_flg;

void ps3_clear(void)
{
	internal_ps3_data.up = 0;
	internal_ps3_data.down = 0;
	internal_ps3_data.right = 0;
	internal_ps3_data.left = 0;
	internal_ps3_data.sankaku = 0;
	internal_ps3_data.batsu = 0;
	internal_ps3_data.maru = 0;
	internal_ps3_data.shikaku = 0;
	internal_ps3_data.L1 = 0;
	internal_ps3_data.L2 = 0;
	internal_ps3_data.R1 = 0;
	internal_ps3_data.R2 = 0;
	internal_ps3_data.start = 0;
	internal_ps3_data.select = 0;
	internal_ps3_data.left_analog_rightleft = 64;
	internal_ps3_data.left_analog_updown = 64;
	internal_ps3_data.right_analog_rightleft = 64;
	internal_ps3_data.right_analog_updown = 64;

	return;
}

void ps3_init()
{
	ps3_clear();
	time_out_check_flg = 1;
	ps3_buff_cnt = 0;
	return;
}

void ps3_uart_interrupt_routine(unsigned char uart_buff[], int data_size)
{
	int i = 0;
	int check_sum = 0;

	// uartバッファにデータがある間ループ
	for(i = 0; i < data_size; i++) {
		// スタートテキスト(ST)検出
		if(uart_buff[i] == 0x80){
			ps3_buff_cnt = 0;
		}

		// uartバッファから，ps3バッファへデータを移動
		ps3_buff[ps3_buff_cnt] = uart_buff[i];

		// 1フレーム受信したことを確認
		if(ps3_buff_cnt == 7) {
			// チェックサムの計算
			check_sum = 0;
			for(i = 1; i < 7; i++) {
				check_sum = (check_sum + ps3_buff[i]) & 0X7F;
			}

			if(ps3_buff[7] == check_sum) {		// チェックサムがあっていれば，データ更新
				// コントローラの値クリア
				ps3_clear();

				// 上ボタン，下ボタン，スタートボタンの更新
				if(((ps3_buff[2] >> 1) & 1) && (ps3_buff[2] & 1)) {
					internal_ps3_data.start = 1;
				}
				else if(ps3_buff[2] & 1) {
					internal_ps3_data.up = 1;
				}
				else if((ps3_buff[2] >> 1) & 1) {
					internal_ps3_data.down = 1;
				}

				// 右ボタン，左ボタン，セレクトボタンの更新
				if(((ps3_buff[2] >> 2) & 1) && ((ps3_buff[2] >> 3) & 1)){
					internal_ps3_data.select = 1;
				}else if((ps3_buff[2] >> 2) & 1){
					internal_ps3_data.right = 1;
				}else if((ps3_buff[2] >> 3) & 1){
					internal_ps3_data.left = 1;
				}

				// サンカク，シカク，マル，バツの更新
				if((ps3_buff[2] >> 4) & 1){
					internal_ps3_data.sankaku = 1;
				}
				if((ps3_buff[2] >> 5) & 1){
					internal_ps3_data.batsu = 1;
				}
				if((ps3_buff[2] >> 6) & 1){
					internal_ps3_data.maru = 1;
				}
				if(ps3_buff[1] & 1){
					internal_ps3_data.shikaku = 1;
				}

				// L1, L2, R1, R2の更新
				if((ps3_buff[1] >> 1)& 1){
					internal_ps3_data.L1 = 1;
				}
				if((ps3_buff[1] >> 2)& 1){
					internal_ps3_data.L2 = 1;
				}
				if((ps3_buff[1] >> 3)& 1){
					internal_ps3_data.R1 = 1;
				}
				if((ps3_buff[1] >> 4)& 1){
					internal_ps3_data.R2 = 1;
				}

				// スティックの更新
				internal_ps3_data.left_analog_updown = ps3_buff[4];
				internal_ps3_data.left_analog_rightleft = ps3_buff[3];
				internal_ps3_data.right_analog_updown = ps3_buff[6];
				internal_ps3_data.right_analog_rightleft = ps3_buff[5];
                
                // タイムアウトチェックフラグクリア
                time_out_check_flg = 0;
			}
		}
		else {		// チェックサムが間違っていた場合，何もしない．このフレームは破棄される．
			;
		}

		// ps3バッファのカウントをインクリメント(8以上にならないようにする)
		if(ps3_buff_cnt < 8) {
			ps3_buff_cnt++;
		}
	}
    return;
}

void ps3_get_data(Ps3 *ps3_data)
{
	*ps3_data = internal_ps3_data;
    return;
}

int ps3_time_out_check(void) {
    int res = 0;
    
    if(time_out_check_flg == 1) {   // タイムアウト処理
        ps3_clear();
        res = 1;
    }
    
    time_out_check_flg = 1;
    
    return res;
}
