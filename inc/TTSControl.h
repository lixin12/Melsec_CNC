#ifndef _TTS_CONTROL_H_
#define _TTS_CONTROL_H_


typedef unsigned char        uint8;
typedef unsigned short       uint16;
typedef  unsigned int        uint32;

typedef enum {
  TTS_PLAY_UTF16LE,   
  TTS_PLAY_GBK,   
  TTS_MAX_AT_CMD,          
}TTS_CMD;

typedef enum {
  TTS_NONE,
  TTS_PLAY,              //正在播放
  TTS_STOP,              //停止播放
}TTS_STATE;

int simcom_play_tts(TTS_CMD tts_action, uint8 *tts_text);
void simcom_stop_tts();
TTS_STATE simcom_get_tts_state();

int set_tts_parameter(int volume,int sysvolume,int digitmode,int pitch,int speed);
int get_tts_parameter(int* pvolume,int* psysvolume,int* pdigitmode,int* ppitch,int* pspeed);

/*************************************************************
 *  funciton simcom_get_cdtam()  get local or remote tts play
 *  input 
 *      none  
 * ret
 *      0: local path
 *      1: remote path
 *************************************************************/
int simcom_get_cdtam();

/*************************************************************
 *  funciton simcom_set_cdtam()  set local or remote tts play
 *  input :
 *  1. nstatus
 *      0: local path
 *      1: remote path
 * ret
 *      0: set success
 *      -1: set failed
 *************************************************************/
int simcom_set_cdtam(int nstatus);

/*************************************************************
 *  funciton simcom_init_tts()  init the tts function
 *  input :
 *      NONE
 * ret
 *        0:  success
 *      -1:  failed
 *************************************************************/
int simcom_init_tts();

/*************************************************************
 *  funciton simcom_deinit_tts()  release the tts resource
 *  input :
 *      none
 * ret
 *      0:  success      
 *************************************************************/
int simcom_deinit_tts();    
 
#endif // _TTS_CONTROL_H_
