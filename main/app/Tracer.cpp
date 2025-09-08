#include "Tracer.h"
#include "Clock.h"
#include "app.h"
#include <stdio.h>
#include <utility>

const uint32_t terminate_duration = 200 * 1000; // 処理間の待機時間
const uint32_t straight_duration = 1500 * 1000; // 走行体を直進させる時間
const uint32_t increment_duration = 300 * 1000; // 未使用の変数（なにこれ？）

const int bias = 0; // 誤差のバイアス値 (左右のモーターの個体差を埋めるもの、
                    // 右モーターが左よりも強い場合は正の値、逆の場合は負の値を設定する。
                    // 例：直進のはずが右に曲がってしまうときは、biasを負の値に設定することで、右モーターの出力を上げて左モーターの出力を下げる。)

Clock term_clock;            // 処理間隔を空けるためのタイマー
Clock blue_count_5_straight; // 直進時間を指定するタイマー
Clock speeddown_clock;

Tracer::Tracer() : leftWheel(EPort::PORT_B, Motor::EDirection::COUNTERCLOCKWISE, true), rightWheel(EPort::PORT_A, Motor::EDirection::CLOCKWISE, true), colorSensor(EPort::PORT_E) {
}

void Tracer::init() {

    // printf("Tracer\n");
}

void Tracer::terminate() {
    printf("Stopped.\n");
    leftWheel.stop();
    rightWheel.stop();
}

void Tracer::run() {

    // ここから8/4記述
    //  障害物回避モードの場合
    if (avoid_mode == 1) {
        // 回避動作の継続時間(マイクロ秒)
        const uint32_t turn_time = 1000000;
        const uint32_t turn_time2 = 5500000;
        const uint32_t back_time = 1000000;

        int back_speed = -50;
        int turn_speed_weak = 5;
        int turn_speed_strong = 65;
        int turn_speed_weak2 = 30;
        int turn_speed_strong2 = 66;
        int turn_speed_weak3 = 5;
        int turn_speed_strong3 = 50;
        int slowdown_pwm = 15;

        if (mode_lr == -1) {
            // ↓ この命令が使えるようになります
            std::swap(turn_speed_weak, turn_speed_strong);
            std::swap(turn_speed_weak2, turn_speed_strong2);
        }

        // avoidance_timerの時間に応じて回避動作を段階的に制御
        if (avoidance_timer.now() < back_time) {
            leftWheel.setPower(back_speed + bias);
            rightWheel.setPower(back_speed - bias);
        } else if (avoidance_timer.now() < (back_time + turn_time)) {
            // 1度目の旋回
            leftWheel.setPower(turn_speed_strong + bias);
            rightWheel.setPower(turn_speed_weak - bias);
            is_black = false;

        } else if (avoidance_timer.now() < (back_time + turn_time) + turn_time2) {

            if (!is_black) {
                // フェーズ5: 2回目の旋回（例: 左に旋回し、元の向きに戻る）
                leftWheel.setPower(turn_speed_weak2 + bias);
                rightWheel.setPower(turn_speed_strong2 - bias);
            } else {
                leftWheel.stop();
                rightWheel.stop();
            }

        } else if (avoidance_timer.now() < (back_time + (turn_time * 2)) + turn_time2) {
            // フェーズ6: 元のラインに戻るための旋回
            leftWheel.setPower(turn_speed_strong3 + bias);
            rightWheel.setPower(turn_speed_weak3 - bias);
        } else {
            float turn = calc_prop_value();
            int pwm_l = pwm + turn - slowdown_pwm;
            int pwm_r = pwm - turn - slowdown_pwm;

            leftWheel.setPower(pwm_l);
            rightWheel.setPower(pwm_r);
        }

        return; // 回避モード中は以降のライン追従ロジックは実行しない
    }
    // ここまで

    if (straight_mode == 1) { // 直進モード
        leftWheel.setPower(90 + bias);
        rightWheel.setPower(90 - bias);
        return; // 直進モード中は以降のライン追従ロジックは実行しない
    }

    int blue = count_blue; // 青色の検知回数

    /*
      if(blue >= 1 && !pwm_dec){
        pwm = pwm -10;
        pwm_dec = true;

      }
        */

    int pwm_l;
    int pwm_r;
    // printf("Blue Count: %d\n", blue);

    if (blue == 5) { // 青い丸の部分を通過するときの処理

        // printf("Blue Count is 5 !\n");
        // float turn = calc_prop_value();

        if (blue_count_5_straight.now() <= straight_duration) {

            int pwm_l = pwm;
            int pwm_r = pwm + (1 * mode_lr);
            leftWheel.setSpeed(350);
            rightWheel.setSpeed(350 + (10 * mode_lr));

        } else {

            float turn = calc_prop_value();
            int pwm_l = pwm + turn;
            int pwm_r = pwm - turn;
            leftWheel.setPower(pwm_l);
            rightWheel.setPower(pwm_r);
        }
    } else {
        const uint32_t break_pwm = pwm - 38; // ダブルループを曲がるときの原則の基準値(pwm - ○○ の ○○の値を大きくすると、減速量が増える仕様です)

        float turn = calc_prop_value();
        if (blue == 0) { // 青色のカウントが0のときの処理

            /*if (leftWheel.getPower() == rightWheel.getPower() && leftWheel.getPower() == 0) {
                speeddown_clock.reset();
            }

            if (speeddown_clock.now() >= 11000000  && speeddown_clock.now() < 20000000 * 10) { // 11秒以上から20秒以内の直進しているときには、減速する
                pwm_l = pwm + turn - 20;
                pwm_r = pwm - turn - 20;
            } else {
                pwm_l = pwm + turn;
                pwm_r = pwm - turn;
            }*/
            pwm_l = pwm + turn;
            pwm_r = pwm - turn;

        } else if (blue % 2 == 0) {         // 青色のカウントが偶数のときの処理
            pwm_l = pwm + turn - break_pwm; // ここにおける、減算の値は、(pwm-35)としておく。(ライントレース時のパワーに関わらずにダブルループの時の速度を一定にするため)
            pwm_r = pwm - turn - break_pwm;
            if (turn_const) {
                pwm_l = pwm - (10 * mode_lr) - break_pwm;
                pwm_r = pwm + (10 * mode_lr) - break_pwm;
                printf("turn_2\n");
                turn_const = false;
                leftWheel.setPower(pwm_l);
                rightWheel.setPower(pwm_r);
                term_clock.sleep(500 * 1000);
            }
        } else if (blue == 1 || blue == 3) { // 青色のカウントが奇数のときの処理
            pwm_l = pwm - turn - break_pwm;
            pwm_r = pwm + turn - break_pwm;
            if (turn_const) {
                pwm_l = pwm + (10 * mode_lr) - break_pwm;
                pwm_r = pwm - (10 * mode_lr) - break_pwm;
                printf("turn_1\n");
                turn_const = false;
                leftWheel.setPower(pwm_l);
                rightWheel.setPower(pwm_r);
                term_clock.sleep(500 * 1000);
            }
        } else { // 青色のカウントが5以外のときの処理(通常は使用しない)
            pwm_l = pwm;
            pwm_r = pwm;
        }
        leftWheel.setPower(pwm_l);
        rightWheel.setPower(pwm_r);

        if (blue == 4) {
            blue_count_5_straight.reset();
        }
    }

    if (leftWheel.getSpeed() == 0 && rightWheel.getSpeed() == 0) {
        // printf("Wheels stopped. Setting power to 50.\n");
        leftWheel.setPower(50);
        rightWheel.setPower(50);
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
    const float Kp = 0.55;
    const int target = 55;
    const float Ki = 0.001;
    const float Kd = 0.20;
    int diff_D = 0;

    int reflection = colorSensor.getReflection();
    // printf("Reflection: %d\n", reflection);

    int diff_P = reflection - target;

    if (diff_P * prev_diff_P < 0) {
        integral = 0;
        // printf("---- Integral Reset! ----\n"); // デバッグ用に表示
    }

    integral += diff_P;

    if (target_D != -1)
        diff_D = reflection - target_D;
    // printf("diff_P: %f, diff_I: %f, diff_D: %f\n", (tracemode_lr * Kp * diff_P), (tracemode_lr * Ki * integral), (tracemode_lr * Kd *diff_D));
    target_D = reflection;
    prev_diff_P = diff_P;

    return (tracemode_lr * Kp * diff_P) + (tracemode_lr * Ki * integral) + (tracemode_lr * Kd * diff_D) + bias;
}
