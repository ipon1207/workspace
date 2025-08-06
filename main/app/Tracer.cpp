#include "Tracer.h" 
#include <stdio.h>
#include "app.h"  
#include "Clock.h"

const uint32_t terminate_duration = 100 * 1000; // 処理間の待機時間
const uint32_t straight_duration = 3000 * 1000; // 走行体を直進させる時間
const uint32_t increment_duration = 300 * 1000; // 未使用の変数（なにこれ？）

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
    const uint32_t reverse_time = 500000; 
    const int reverse_speed = -50;
    const int turn_speed = 50;

    // avoidance_timerの時間に応じて回避動作を段階的に制御
    if (avoidance_timer.now() < reverse_time) {
     // 後退
      leftWheel.setPower(reverse_speed);
      rightWheel.setPower(reverse_speed);
     } else if (avoidance_timer.now() < reverse_time + turn_time) {
     // 右に旋回
         leftWheel.setPower(turn_speed);
         rightWheel.setPower(-turn_speed);
     } else if (avoidance_timer.now() < 3500000) {
    // 回避動作完了後、直進
         leftWheel.setPower(pwm);
         rightWheel.setPower(pwm);
     } else if (avoidance_timer.now() < 4000000) {
     // フェーズ1: 停止
         leftWheel.setPower(0);
         rightWheel.setPower(0);
     }else if(avoidance_timer.now() < 5000000 + 1000000 ){
      // フェーズ5: 2回目の旋回（例: 左に旋回し、元の向きに戻る）
         leftWheel.setPower(-turn_speed);
         rightWheel.setPower(turn_speed);
     } else {
        // フェーズ6: 元のラインに戻るための直進
         leftWheel.setPower(pwm);
         rightWheel.setPower(pwm);
     }      
     return; // 回避モード中は以降のライン追従ロジックは実行しない
  }
  //ここまで

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

float Tracer::calc_prop_value() {

  const int tracemode_lr = mode_lr; // app.cppを参照
  const float Kp = 0.60;
  const int target = 50;
  int diff = colorSensor.getReflection() - target;

  return tracemode_lr * Kp * diff;
}
