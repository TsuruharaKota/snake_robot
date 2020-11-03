#include <mbed.h>
#include "serial.h"

#define SG_PERIOD 20     /* 20[msec] */
#define SG_NEUTRAL 1450  /* 90° 1450[usec] */

//最大偏角, 推進速度, 時間, 関節ごとのオフセット, 旋回角度
float SerpenoidCurve(float max_angle_, float angular_, float timer_, float offset_deg_, float turn_deg_){
    return (max_angle_ * sin((angular_ * timer_ * 100) + (offset_deg_ * (M_PI / 180))) + (turn_deg_ * (180 / M_PI))); 
}
float convOutput(float val){
  return val + 90;
}

class ServoSg90: public PwmOut
{
public:
    /* コンストラクタ */
    ServoSg90( PinName  port ):PwmOut( port ){}
    /* 初期化処理 */
    void init();
    /* 回転処理 */
    int  roll( unsigned int angle );
};
 /* 初期化処理 */
void ServoSg90::init(){
    /* 周期の設定を行う */
    period_ms( SG_PERIOD );
    /* 90°に角度調整 */
    pulsewidth_us( SG_NEUTRAL );
}
/* 回転処理 */
int ServoSg90::roll( unsigned int angle ){
    int lPulseWidth = 0;
    /* 回転する角度が90より小さい場合 */
    if( angle < 90 ){
        lPulseWidth = ( 90 - angle ) * 10;
        lPulseWidth = SG_NEUTRAL - lPulseWidth;
    /* 回転する角度が90より大きく180以下の場合 */
    } else if ( angle <= 180 ){
        lPulseWidth = ( angle - 90 ) * 10;
        lPulseWidth = SG_NEUTRAL + lPulseWidth;
    /* 上記以外はエラー */
    } else {
        return -1;
    }
    /* ポートに流すパルスの時間を設定する */
    pulsewidth_us( lPulseWidth );
    return 0;
}

ServoSg90  snake_joint[9] = {PC_6, D10, D2, D3, PC_8, PC_9, PB_7, PA_0, D8};
Serial pc(USBTX, USBRX, 9600);

int main(){
  constexpr int joint_num = 9;
  constexpr float max_deg = 60;
  float receive_data[2] = {}; //[0] = 推進速度, [1] = 旋回角度[deg]
  float send_data[2] = {};
/* init servo */
  for(int i = 0; i < joint_num; ++i){
    snake_joint[i].init();
  }
  constexpr std::array<float, joint_num> offset_theta{0, 60, 120, 180, 240, 300, 360, 420, 480}; //各関節ごとのオフセット
  std::array<float, joint_num> servo_theta;
  Timer tim;
  tim.start();
  tim.reset();
  float timer{};
  while(1){
    serialReceive(receive_data, pc);
    send_data[0] = receive_data[0];
    send_data[1] = receive_data[1];
    serialSend(send_data, pc);
    //receive_data[0] = 6;
    //receive_data[1] = 0;
    timer = (float)tim.read_ms() / 1000;
    for(int i = 0; i < joint_num; ++i){
      snake_joint[i].roll((unsigned int)convOutput(SerpenoidCurve(max_deg, receive_data[0], timer, offset_theta[i], receive_data[1])));
      wait(0.01);
    }
  }
}
    /*
    snake_joint[0].roll((unsigned int)convOutput(60 * sin( 6 * timer * 100        * (M_PI / 180))));
    wait(0.01);
    snake_joint[1].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 +  60) * (M_PI / 180))));
    wait(0.01);
    snake_joint[2].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 120) * (M_PI / 180))));
    wait(0.01);
    snake_joint[3].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 180) * (M_PI / 180))));
    wait(0.01);
    snake_joint[4].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 240) * (M_PI / 180))));
    wait(0.01);
    snake_joint[5].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 300) * (M_PI / 180))));
    wait(0.01);
    snake_joint[6].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 360) * (M_PI / 180))));
    wait(0.01);
    snake_joint[7].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 420) * (M_PI / 180))));
    wait(0.01);
    snake_joint[8].roll((unsigned int)convOutput(60 * sin((6 * timer * 100 + 480) * (M_PI / 180))));
    wait(0.01);
  }
}
*/