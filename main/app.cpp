#include "app.h"
#include "Clock.h"
#include <stdio.h>

#include "ForceSensor.h"
#include "Tracer.h"

volatile int avoid_mode = 0;       // 0: 通常走行, 1: 障害物回避モード
volatile bool is_avoiding = false; // 回避モードへの遷移を一度だけ行うためのフラグ
Clock avoidance_timer;             // 障害物回避の動作時間調整用タイマー
volatile bool is_black = false;

volatile bool turn_const = false; // ダブルループ侵入時一回だけ旋回を調整する

volatile int straight_mode = 0; // 0: 通常走行, 1: 直進モード

Tracer tracer;
Clock clock;
Clock cooltime; // 青の回数カウントのクールタイムのためのタイマー

Clock task_timer;

#ifdef __cplusplus
extern "C" {
#endif

volatile int count_blue = 0; // 青色の検知回数
#define LEFT 1
#define RIGHT -1
volatile int mode_lr = LEFT; // モード変数

volatile int target_D = -1; // D制御用の変数、微分のための前回の値を保持する変数
volatile int integral = 0;
volatile int prev_diff_P = 0;

#ifdef __cplusplus
}
#endif

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

    task_timer.reset();
    tracer.run();
    printf("tracer_task time: %d\n", task_timer.now());
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

    // ここから8/4記述(微調整対象)
    const int OBSTACLE_DISTANCE = 25;             // 障害物と判断する距離(mm) 150?
    const uint32_t AVOIDANCE_DURATION = 10250000; // 回避動作の継続時間(マイクロ秒)
    // ここまで

    ForceSensor forceSensor(EPort::PORT_D);

    ColorSensor::RGB rgb;
    double all = 0;
    double r_rate = 0;
    double g_rate = 0;
    double b_rate = 0;

    // フォースセンサーが押されるまで待機
    while (!forceSensor.isTouched()) {
        clock.sleep(duration);
    }

    timer_start();
    clock.sleep(waitTime);
    cooltime.reset();
    tracer.init();

    // avoidance_timer.reset(); //デバッグ用

    sta_cyc(TRACER_CYC);

    while (1) {
        clock.sleep(duration);
        update_color_info(rgb, all, r_rate, g_rate, b_rate);
        // printf("R:%d G:%d B:%d\n", rgb.r, rgb.g, rgb.b);

        timer_gettime();
        // printf("timer is %lf\n",((float)cooltime.now() / 1000000));

        // printf("MotorSpeed: L: %d, R: %d\n", tracer.leftWheel.getSpeed(), tracer.rightWheel.getSpeed());

        if ((tracer.leftWheel.getSpeed() + tracer.rightWheel.getSpeed()) <= 600 && clock.now() > 1 * 1000 * 1000 && !is_avoiding) {
            printf("Obstacle detected! Switching to avoidance mode.\n");
            // 回避モードをON
            avoid_mode = 1;
            straight_mode = 0; // 直進モードをOFF、デバッグ用
            // 回避モードへの遷移を一度だけにする
            is_avoiding = true;
            // 回避動作のタイマーをリセット
            avoidance_timer.reset();
        }

        // 回避モードで一定時間経過したら、通常モードに戻る
        if (is_avoiding && avoidance_timer.now() > AVOIDANCE_DURATION && avoid_mode == 1) {
            printf("Avoidance completed! Switching back to normal mode.\n");
            avoid_mode = 0;
        }

        if (avoid_mode == 1 && b_rate <= 0.40 && rgb.b <= 300 && all <= 900 && !is_black) { // 障害物回避モード中に黒を踏んだことを検知する

            is_black = true;
        }

        if (b_rate >= 0.40 && rgb.b >= 50 && cooltime.now() > blue_coolTime) { // 青を検知する機能

            if (count_blue >= 4) {
                count_blue++;
                cooltime.reset();
                printf("Blue Count: %d\n", count_blue);
            }

            if (count_blue >= end_blue_count) {
                printf("Blue = %d: end \n", count_blue);
                break;
            }

            while (b_rate >= 0.40 && rgb.b >= 50 || all >= 2100) { // 青の検知回数が4回を下回る時、青を検知してから黒を検知したときにカウントを増加させるようにしている
                update_color_info(rgb, all, r_rate, g_rate, b_rate);
                clock.sleep(duration);
            }
            if (count_blue < 4) {
                count_blue++;
                cooltime.reset();
                printf("Blue Count: %d\n", count_blue);
                turn_const = true;
            }

        } else if (b_rate >= 0.50 && rgb.b >= 50 && cooltime.now() <= blue_coolTime) { // クールタイムが済んでいない時に青を検知した際の処理
            printf("Blue detected! Count: \n");
        }
    }

    stp_cyc(TRACER_CYC);

    clock.sleep(duration);
    tracer.terminate();

    ext_tsk();
}