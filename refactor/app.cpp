/* 宣言ファイル */
#include "app.h"
/* 標準ライブラリ */
#include <stdio.h>
/* RasPikeART用ライブラリ */
#include "Clock.h"
#include "ForceSensor.h"
#include "UltrasonicSensor.h"
/* カスタムクラス */
#include "Starter.h"
#include "Tracer.h"

volatile int avoid_mode = 0;       // 0: 通常モード, 1: 障害物回避モード
volatile int straight_mode = 0;    // 0: 通常走行, 1: 直進モード
volatile bool is_avoiding = false; // 回避モードへの遷移を一度だけ行うためのフラグ
volatile bool turn_const = false;  // 一回だけ旋回を調整するやつ
volatile int count_blue = 0;       // 青色の検知回数
volatile int mode_lr = 1;          // モード変数（-1: 右寄り, 1: 左寄り）
volatile int target_D = -1;        // D制御用の変数、微分のための前回の値を保持する変数
volatile int integral = 0;
volatile int prev_diff_P = 0;

Tracer tracer;
Starter stater;
Clock clock;
Clock cooltime;        // 青の回数カウントのクールタイムのためのタイマー
Clock avoidance_timer; // 障害物回避の動作時間調整用タイマー
UltrasonicSensor ultrasonicSensor(EPort::PORT_F);

using namespace spikeapi;

void update_color_info(ColorSensor::RGB &rgb, double &all, double &r_rate, double &g_rate, double &b_rate) {
    tracer.colorSensor.getRGB(rgb);
    all = rgb.r + rgb.g + rgb.b;
    r_rate = rgb.r / all;
    g_rate = rgb.g / all;
    b_rate = rgb.b / all;
    // printf("R:%lf G:%lf B:%lf\n", r_rate, g_rate, b_rate);
}

void tracer_task(intptr_t exinf) {
    tracer.run();
    ext_tsk();
}

void timer_start() {
    clock.reset();
    Clock();
}

void timer_gettime() {
    int timer;
    timer = clock.now();
    // printf("timer is %lf\n", (float) timer / 1000000);
}

void main_task(intptr_t unused) {
    const uint32_t duration = 100 * 1000;           // 処理間の待機時間
    const uint32_t waitTime = 5 * 1000 * 1000 / 10; // 起動時の待機時間
    const uint32_t blue_coolTime = 2000 * 1000;     // 青色検知の処理間隔
    const uint32_t end_blue_count = 6;              // 走行体の停止合図
    const int OBSTACLE_DISTANCE = 10;               // 障害物と判断する距離(mm) 150?
    const uint32_t AVOIDANCE_DURATION = 4000000;    // 回避動作の継続時間(マイクロ秒)
    ForceSensor forceSensor(EPort::PORT_D);
    ColorSensor::RGB rgb;
    double all = 0;
    double r_rate = 0;
    double g_rate = 0;
    double b_rate = 0;

    // フォースセンサーが押されるまで待機
    stater.waitStart();

    timer_start();
    clock.sleep(waitTime);
    cooltime.reset();
    tracer.init();
    // avoidance_timer.reset();

    sta_cyc(TRACER_CYC);

    while (1) {
        clock.sleep(duration);
        update_color_info(rgb, all, r_rate, g_rate, b_rate);
        // printf("b: %d", rgb.b);
        timer_gettime();
        // printf("timer is %lf\n",((float)cooltime.now() / 1000000));
        int distance = ultrasonicSensor.getDistance(); // 超音波センサーで障害物を検知
        // printf("distance:%d\n", distance);

        // 障害物を検知し、まだ回避モードになっていない場合
        if (distance > 0 && distance < OBSTACLE_DISTANCE && !is_avoiding) {
            printf("Obstacle detected! Switching to avoidance mode.\n");
            avoid_mode = 1;
            straight_mode = 0;
            is_avoiding = true; // 回避モードへの遷移を一度だけにする
            avoidance_timer.reset();
        }

        // 秒数で回避モード強制発動、使用するかは超音波センサーがちゃんと動くかどうかで決めてください
        if (((float)cooltime.now() / 1000000) >= 120 && !is_avoiding) {
            printf("avoidance mode.\n");
            avoid_mode = 1;
            straight_mode = 0;
            is_avoiding = true; // 回避モードへの遷移を一度だけにする
            avoidance_timer.reset();
        }

        // 回避モードで一定時間経過したら、通常モードに戻る
        if (is_avoiding && avoidance_timer.now() > AVOIDANCE_DURATION &&
            avoid_mode == 1) {
            printf("Avoidance completed! Switching back to normal mode.\n");
            avoid_mode = 0;
            // is_avoiding = false;  // フラグをリセット
        }

        if (b_rate >= 0.40 && rgb.b >= 50 && cooltime.now() > blue_coolTime) {

            while (b_rate >= 0.40 && rgb.b >= 50 || all >= 2100) {
                update_color_info(rgb, all, r_rate, g_rate, b_rate);
                clock.sleep(duration);
            }

            count_blue++;
            cooltime.reset();
            printf("Blue Count: %d\n", count_blue);
            turn_const = true;

        } else if (b_rate >= 0.50 && rgb.b >= 50 && cooltime.now() <= blue_coolTime) {
            printf("Blue detected! Count: \n");
        }

        if (count_blue >= end_blue_count) {
            printf("Blue = 6: end \n");
            break;
        }
    }

    stp_cyc(TRACER_CYC);

    tracer.terminate();

    ext_tsk();
}