#include "Tracer.h" 
#include <stdio.h>
#include "app.h"  
#include "Clock.h"
#include <utility> 

const uint32_t terminate_duration = 100 * 1000; // 処理間の待機時間
const uint32_t straight_duration = 3000 * 1000; // 走行体を直進させる時間
const uint32_t increment_duration = 300 * 1000; // 未使用の変数（なにこれ？）


const int bias = -1; // 誤差のバイアス値 (左右のモーターの個体差を埋めるもの、
                        // 右モーターが左よりも強い場合は正の値、逆の場合は負の値を設定する。
                        //例：直進のはずが右に曲がってしまうときは、biasを負の値に設定することで、右モーターの出力を上げて左モーターの出力を下げる。)

Clock term_clock;            // 処理間隔を空けるためのタイマー
Clock blue_count_5_straight; // 直進時間を指定するタイマー

Tracer::Tracer() : leftWheel(EPort::PORT_B, Motor::EDirection::COUNTERCLOCKWISE, true), rightWheel(EPort::PORT_A, Motor::EDirection::CLOCKWISE, true), colorSensor(EPort::PORT_E) {                   

}

void Tracer::init() {

  // printf("Tracer\n");

}

void Tracer::terminate() {

  printf("Stopped.\n");
  term_clock.sleep(terminate_duration);
  leftWheel.stop(); 
  rightWheel.stop();

}

void Tracer::run() {
  //ここから8/4記述
  // 障害物回避モードの場合
  if (avoid_mode == 1) {
    // 回避動作の継続時間(マイクロ秒)
    const uint32_t turn_time = 1000000; 

    int turn_speed_weak = 30;
    int turn_speed_strong = 60;

        if (mode_lr == -1) {
        // ↓ この命令が使えるようになります
        std::swap(turn_speed_weak, turn_speed_strong); 

    }   

    // avoidance_timerの時間に応じて回避動作を段階的に制御
     if (avoidance_timer.now() < turn_time) {
     // 1度目の旋回
      leftWheel.setPower(turn_speed_weak + bias);
      rightWheel.setPower(turn_speed_strong - bias);
 
     }else if(avoidance_timer.now() < turn_time + 2000000 ){
      // フェーズ5: 2回目の旋回（例: 左に旋回し、元の向きに戻る）
         leftWheel.setPower(turn_speed_strong + bias);
         rightWheel.setPower(turn_speed_weak - bias);
     } else {
        // フェーズ6: 元のラインに戻るための旋回
        leftWheel.setPower(turn_speed_weak + bias);
         rightWheel.setPower(turn_speed_strong - bias);
     }     

     return; // 回避モード中は以降のライン追従ロジックは実行しない
  }
  //ここまで

  if(straight_mode == 1) { // 直進モード
    leftWheel.setPower(50 + bias);
    rightWheel.setPower(50 - bias + 1);
    return; // 直進モード中は以降のライン追従ロジックは実行しない
  }

  int blue = count_blue; // 青色の検知回数
  int pwm_l;
  int pwm_r;
  //printf("Blue Count: %d\n", blue);

  if (blue == 5) { // 青い丸の部分を通過するときの処理
    
    // printf("Blue Count is 5 !\n");
    // float turn = calc_prop_value();
    
    if (blue_count_5_straight.now() <= straight_duration ) {

      int pwm_l = pwm ;
      int pwm_r = pwm ;
      leftWheel.setPower(pwm_l);
      rightWheel.setPower(pwm_r);

    } else {

      float turn = calc_prop_value();
      int pwm_l = pwm - turn;
      int pwm_r = pwm + turn;
      leftWheel.setPower(pwm_l);
      rightWheel.setPower(pwm_r);

    }
  }
  else{

    float turn = calc_prop_value();
    if(blue % 2 == 0) { // 青色のカウントが偶数のときの処理
      pwm_l = pwm + turn;
      pwm_r = pwm - turn;   
    }
    else if(blue == 1 || blue == 3) { // 青色のカウントが奇数のときの処理
      pwm_l = pwm - turn;
      pwm_r = pwm + turn;
    }
    else{ // 青色のカウントが5以外のときの処理(通常は使用しない)
      pwm_l = pwm;
      pwm_r = pwm;
    }
    leftWheel.setPower(pwm_l);
    rightWheel.setPower(pwm_r);

    if (blue == 4) {
      blue_count_5_straight.reset();
    }

  }

}

/*
float Tracer::calc_prop_value() {

  const int tracemode_lr = mode_lr; // app.cppを参照
  const float Kp = 0.60;
  const int target = 50;

  int diff = colorSensor.getReflection() - target;

  return (tracemode_lr * Kp * diff) + bias;
}
  */

float Tracer::calc_prop_value() {

  const int tracemode_lr = mode_lr; // app.cppを参照
  const float Kp = 0.60;
  const int target = 50;
  const float Ki = 0.01;
  const float Kd = 0.30;
  int diff_D = 0;

  int diff_P = colorSensor.getReflection() - target;

    if (diff_P * prev_diff_P < 0) {
      integral = 0;
      printf("---- Integral Reset! ----\n"); // デバッグ用に表示
  }

  integral += diff_P;

  if(target_D != -1) diff_D = colorSensor.getReflection() - target_D;
  printf("diff_P: %d, diff_I: %d, diff_D: %d\n", diff_P, integral, diff_D);
  target_D = colorSensor.getReflection();
  prev_diff_P = diff_P;

  return (tracemode_lr * Kp * diff_P) + (tracemode_lr * Ki * integral) + (tracemode_lr * Kd *diff_D) + bias;
}
