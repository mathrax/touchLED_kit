/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

#define _XTAL_FREQ      32000000
unsigned int CPS_data[2]; // 容量検知用データ


float BaseLine;
float Average;

/*******************************************************************************
 *  CPS_Init()                                                                  *
 *    容量検知モジュールの初期値を読み込む処理                                  *
 *******************************************************************************/
void CPS_Init() {
    unsigned int cap;
    int i;

    for (i = 0; i < 16; i++) {

        CPSCON1 = 2; // 読み込むチャンネルを設定する
        TMR1H = 0; // タイマー1の初期化
        TMR1L = 0;
        CPSON = 1; // 容量検知モジュール開始
        __delay_us(5000); // 5msの間タイマー1をカウントさせる
        // 容量検知モジュールの値を読み込む
        CPSON = 0; // 容量検知モジュール停止
        CPS_data[0] = (TMR1H * 256) + TMR1L;
        CPS_data[1] = 0;

        cap = (TMR1H * 256) + TMR1L;
        Average = Average * 15.0 / 16.0 + cap / 16.0;

        if (cap <= (Average - 100)) {
            BaseLine = BaseLine + Average / 15.0;

            if (BaseLine > Average) {
                BaseLine = Average;
            }

        } else {
            BaseLine *= 0.9;
        }
    }
}

/*******************************************************************************
 *  CPS_ScanRobet()                                                             *
 *    容量検知モジュールに接続されている電極の現在値を読み込む処理              *
 *******************************************************************************/

unsigned int cap;

void CPS_ScanRobe() {
    int i;

    CPSCON1 = 2; // 読み込むチャンネルを設定する
    TMR1H = 0; // タイマー1の初期化
    TMR1L = 0;
    CPSON = 1; // 容量検知モジュール開始

    __delay_us(5000); // 5msの間タイマー1をカウントさせる
    // 容量検知モジュールの値を読み込む

    CPSON = 0; // 容量検知モジュール停止
    cap = (TMR1H * 256) + TMR1L;

    Average = Average * 3.0 / 4.0 + cap / 4.0;

    if (cap <= (Average - 100)) {
        BaseLine = BaseLine + Average / 4.0;

        if (BaseLine > Average) {
            BaseLine = Average;
        }

    } else {
        BaseLine = 0;
    }


    if (Average - BaseLine >= 200) {
        CPS_data[1] = cap; // ONとする
    } else {
        CPS_data[1] = 0; // OFFとする
        CPS_data[0] = cap;
    }




}

/*******************************************************************************
 *  ans = CPS_StateRobe(num)                                                    *
 *    容量検知モジュールに接続されている電極の状態を調べる処理                  *
 *                                                                              *
 *    num : 調べる電極の番号を指定する                                          *
 *    ans : １＝触れている　０＝触れていない　－１＝電極の番号を指定エラー      *
 *******************************************************************************/
int CPS_StateRobe() {
    if (CPS_data[1] == 0) return ( 0); // 電極に触れていない
    else return ( 1); // 電極に触れている
}

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

int switchStatus;
int toggleLED;
signed int Bright;

int toggleDelayCount;

void main(void) {
    /* Configure the oscillator for the device */

    OSCCON = 0b01110010; // 内部クロックは8ＭＨｚとする

    ANSELA = 0b00000100; // アナログはAN2のみ
    TRISA = 0b00000100; // ピンRA2(AN2)を入力、残りは出力に割当てる
    PORTA = 0b00000000; // 出力ピンの初期化(全てLOWにする)

    CCP1SEL = 1; // (RA5)をCCP1ピンとして出力
    CCP1CON = 0b00001100; // PWM機能(シングル)を使用する
    T2CON = 0b00000000; // TMR2プリスケーラ値を１６倍に設定
    CCPR1L = 0; // デューティ値は０で初期化
    CCPR1H = 0;
    TMR2 = 0; // タイマー２カウンターを初期化
    PR2 = 124; // PWMの周期を設定（1000Hzで設定）
    TMR2ON = 1; // TMR2(PWM)スタート

    // 容量検知モジュール(ＣＰＳＭ)の設定
    CPSCON0 = 0b00001100; // オシレータは高範囲(高速の発信周波数)で利用する
    // タイマー１の設定
    T1CON = 0b11000001; // 容量検知オシレータでTIMER1をｶｳﾝﾄする、ﾌﾟﾘｽｹｰﾗｶｳﾝﾄ値 1:1
    TMR1H = 0; // タイマー1の初期化
    TMR1L = 0;
    PEIE = 1; // 周辺装置割り込みを許可する
    GIE = 1; // 全割り込み処理を許可する

    // 容量検知モジュールの初期値を読み込む
    CPS_Init();

    switchStatus = 0;
    toggleLED = 1;
    toggleDelayCount = 5;

    while (1) {

        CPS_ScanRobe();

        if (CPS_StateRobe() == 1) {
            if (switchStatus == 0) {
                switchStatus = 1;

                toggleLED = !toggleLED;
                toggleDelayCount = 5;
            }
        } else {
            if (toggleDelayCount > 0) {
                toggleDelayCount--;
            } else {
                switchStatus = 0;
            }
        }


        if (toggleLED) {

            Bright += 5;
            if (Bright > 255) {
                Bright = 255;
            }

        } else {

            Bright = 0;
        }
        CCPR1L = Bright;

    }

}

