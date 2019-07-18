# Canmd2019Project
CAN通信でアクセス出来て，モーター速度をPID制御できるMDのプロジェクト

---

2019/07/18 written by SIVA
## HOW TO USE `Pid` CLASS
### How functions are used

 function | how to move 
--|:--
`Pid()` | コンストラクタの引数は無し
`~Pid()` | デストラクタの引数は無し
`set_gain(Kp,Ki,Kd)` | 引数の順番でゲインを取得
`get_enc(enc)` | 引数の値をエンコーダの値として取得
`get_ideal(ideal_data)` | 引数の値をモーターコントロールデータ（コントローラ）の値として取得
`update_errors()` | 入力データとエンコーダの値の誤差を計算
`pid_cal()` | 最新の誤差でモーターコントロールデータをPID処理した値を返す

> TODO? : classの変数の値を表示する関数
