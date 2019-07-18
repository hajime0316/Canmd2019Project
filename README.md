# Canmd2019Project
CAN通信でアクセス出来て，モーター速度をPID制御できるMDのプロジェクト

---

2019/07/18 written by SIVA
## HOW TO USE `Pid` CLASS
### How functions are used
To PID-control certain data.

Please use these functions like 
`InstanceOfPid.function` .

 function | remarks
--|:--
`Pid()` | コンストラクタの引数は無し
`~Pid()` | デストラクタの引数は無し
`void set_gain(double Kp,double Ki,double Kd)` | 左の引数の順番でゲインを取得
`void get_enc(double)` | 引数の値をエンコーダの値として取得
`void get_ideal(double)` | 引数の値をモーターコントロールデータ（コントローラ）の値として取得
`void update_errors()` | 入力データとエンコーダの値の誤差を計算
`double pid_cal()` | 最新の誤差でモーターコントロールデータをPID処理した値を返す

> TODO? : classの変数の値を表示する

## HOW TO USE `Stm32f3AntiphasePwm` CLASS
### How functions are used
To make antiphase pwm from control data(dualshock or simple controler)

Please use these functions like 
`InstanceOfStm32f3AntiphasePwm.function` .

 function | remarks
--|:--
`AntiphasePwm(const TIM_HandleTypeDef *htim)` | `TIM_HandleTypeDef *htim`を引数に取る(ただし引数は`&htim1`のようにする)
`~AntiphasePwm()` | デストラクタの引数は無し
`void set_gain(double)` | 引数にコントロールデータの比率を取り、デューティー比を計算
