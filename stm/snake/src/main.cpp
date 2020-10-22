#include <mbed.h>
#include "serial.h"

#define SG_PERIOD 20     /* 20[msec] */
#define SG_NEUTRAL 1450  /* 90° 1450[usec] */

constexpr int joint_num = 9;
constexpr double amplitude = 1.0; //振幅(位相)
constexpr double max_speed = 1.0;  //[m/s]

double SerpenoidCurve(double angular_frequency_, double timer_, double offset_theta_, double turn_theta_){
    return (amplitude * sin(angular_frequency_ * timer_ + (offset_theta_ * (M_PI / 180)) + turn_theta_) * (180 / M_PI)); 
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

ServoSg90  snake_joint[9] = {D9, D10, D2, D3, D4, D11, D6, D7, D8};
//Serial pc(USBTX, USBRX, 115200);

int main(){
  float snake_joint_deg_ref[9];
/* init servo */
  for(int i = 0; i < 9; ++i){
    snake_joint[i].init();
  }
  constexpr std::array<double, joint_num> offset_theta{0, 60, 120, 180, 240, 300, 360, 420, 480}; //前回角度
  std::array<double, joint_num> servo_theta;
  double turn_theta = 0;
  Timer tim;
  tim.start();
  tim.reset();
  double timer{};
  float write_data[joint_num]{};
  while(1){
    timer = tim.read();
    for(int i = 0; i < joint_num; ++i){
      servo_theta[i] = SerpenoidCurve(max_speed, timer, offset_theta[i], turn_theta);
      write_data[i] = servo_theta[i];
      snake_joint_deg_ref[i] = servo_theta[i];
    }
    for(int i = 0; i < 9; ++i){
      snake_joint[i].roll(snake_joint_deg_ref[i]);
    }
  }
}