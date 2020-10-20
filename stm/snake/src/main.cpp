#include <mbed.h>
#include "serial.h"

#define SG_PERIOD 20     /* 20[msec] */
#define SG_NEUTRAL 1450  /* 90° 1450[usec] */

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

ServoSg90  snake_joint[9] = {D1, D2, D3, D4, D5, D6, D7, D8, D9};
Serial pc(USBTX, USBRX, 115200);

int main() {
  float snake_joint_deg_ref[9];
  snake::serialReceive(snake_joint_deg_ref, pc);
  // put your setup code here, to run once:
/* init servo */
  for(int i = 0; i < 9; ++i){
    snake_joint[i].init();
  }
  while(1) {
    for(int i = 0; i < 9; ++i){
      snake_joint[i].roll(snake_joint_deg_ref[i]);
    }
  }
}