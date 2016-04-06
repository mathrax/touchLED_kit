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
unsigned int CPS_data[2]; // �e�ʌ��m�p�f�[�^


float BaseLine;
float Average;

/*******************************************************************************
 *  CPS_Init()                                                                  *
 *    �e�ʌ��m���W���[���̏����l��ǂݍ��ޏ���                                  *
 *******************************************************************************/
void CPS_Init() {
    unsigned int cap;
    int i;

    for (i = 0; i < 16; i++) {

        CPSCON1 = 2; // �ǂݍ��ރ`�����l����ݒ肷��
        TMR1H = 0; // �^�C�}�[1�̏�����
        TMR1L = 0;
        CPSON = 1; // �e�ʌ��m���W���[���J�n
        __delay_us(5000); // 5ms�̊ԃ^�C�}�[1���J�E���g������
        // �e�ʌ��m���W���[���̒l��ǂݍ���
        CPSON = 0; // �e�ʌ��m���W���[����~
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
 *    �e�ʌ��m���W���[���ɐڑ�����Ă���d�ɂ̌��ݒl��ǂݍ��ޏ���              *
 *******************************************************************************/

unsigned int cap;

void CPS_ScanRobe() {
    int i;

    CPSCON1 = 2; // �ǂݍ��ރ`�����l����ݒ肷��
    TMR1H = 0; // �^�C�}�[1�̏�����
    TMR1L = 0;
    CPSON = 1; // �e�ʌ��m���W���[���J�n

    __delay_us(5000); // 5ms�̊ԃ^�C�}�[1���J�E���g������
    // �e�ʌ��m���W���[���̒l��ǂݍ���

    CPSON = 0; // �e�ʌ��m���W���[����~
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
        CPS_data[1] = cap; // ON�Ƃ���
    } else {
        CPS_data[1] = 0; // OFF�Ƃ���
        CPS_data[0] = cap;
    }




}

/*******************************************************************************
 *  ans = CPS_StateRobe(num)                                                    *
 *    �e�ʌ��m���W���[���ɐڑ�����Ă���d�ɂ̏�Ԃ𒲂ׂ鏈��                  *
 *                                                                              *
 *    num : ���ׂ�d�ɂ̔ԍ����w�肷��                                          *
 *    ans : �P���G��Ă���@�O���G��Ă��Ȃ��@�|�P���d�ɂ̔ԍ����w��G���[      *
 *******************************************************************************/
int CPS_StateRobe() {
    if (CPS_data[1] == 0) return ( 0); // �d�ɂɐG��Ă��Ȃ�
    else return ( 1); // �d�ɂɐG��Ă���
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

    OSCCON = 0b01110010; // �����N���b�N��8�l�g���Ƃ���

    ANSELA = 0b00000100; // �A�i���O��AN2�̂�
    TRISA = 0b00000100; // �s��RA2(AN2)����́A�c��͏o�͂Ɋ����Ă�
    PORTA = 0b00000000; // �o�̓s���̏�����(�S��LOW�ɂ���)

    CCP1SEL = 1; // (RA5)��CCP1�s���Ƃ��ďo��
    CCP1CON = 0b00001100; // PWM�@�\(�V���O��)���g�p����
    T2CON = 0b00000000; // TMR2�v���X�P�[���l���P�U�{�ɐݒ�
    CCPR1L = 0; // �f���[�e�B�l�͂O�ŏ�����
    CCPR1H = 0;
    TMR2 = 0; // �^�C�}�[�Q�J�E���^�[��������
    PR2 = 124; // PWM�̎�����ݒ�i1000Hz�Őݒ�j
    TMR2ON = 1; // TMR2(PWM)�X�^�[�g

    // �e�ʌ��m���W���[��(�b�o�r�l)�̐ݒ�
    CPSCON0 = 0b00001100; // �I�V���[�^�͍��͈�(�����̔��M���g��)�ŗ��p����
    // �^�C�}�[�P�̐ݒ�
    T1CON = 0b11000001; // �e�ʌ��m�I�V���[�^��TIMER1���Ă���A��ؽ��׶��Ēl 1:1
    TMR1H = 0; // �^�C�}�[1�̏�����
    TMR1L = 0;
    PEIE = 1; // ���ӑ��u���荞�݂�������
    GIE = 1; // �S���荞�ݏ�����������

    // �e�ʌ��m���W���[���̏����l��ǂݍ���
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

