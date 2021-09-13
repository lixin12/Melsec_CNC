#ifndef _ALSA_CONTROL_H_
#define _ALSA_CONTROL_H_

typedef enum {
    AUDIO_RECORD_STOP,
    AUDIO_RECORD_START_LOCAL,
    AUDIO_RECORD_START_REMOTE,
    AUDIO_RECORD_START_LOCAL_REMOTE,
    AUDIO_RECORD_MAX
}simcom_audio_record_type_e;

typedef enum {
    AUDIO_RECORD_STATE_IDLE  = 0,
    AUDIO_RECORD_STATE_BUSY = 1,
    AUDIO_RECORD_STATEMAX
}simcom_audio_record_state_e;



/*****************************************************************************
* Function Name : set_clvl_value
* Description	: select the volume of the internal loudspeaker audio 
* Input 		: integer value represent loudspeaker volume level    (0-5)
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: lhy
* Date		: 2018.08.21
*****************************************************************************/	
int set_clvl_value(int clvl_value);

/*****************************************************************************
* Function Name : get_clvl_value
* Description	: get  the volume of the internal loudspeaker audio 
* Input 		: None
* Output		: None
* Return		: when success renturn the clvl value, when fail return -1 
* Auther		: lhy
* Date		: 2018.08.21
*****************************************************************************/	
int get_clvl_value(void);



/*****************************************************************************
* Function Name : set_micgain_value
* Description	: adjust mic gain 
* Input 		: integer value represent gain value (0-8)
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: lhy
* Date		: 2018.08.21
*****************************************************************************/	

int set_micgain_value(int micgain_value);


/*****************************************************************************
* Function Name : get_micgain_value
* Description	: get  the mic gain value 
* Input 		: None
* Output		: None
* Return		: when success renturn the mic gain  value, when fail return -1 
* Auther		: lhy
* Date	        : 2018.08.21
*****************************************************************************/	

int get_micgain_value(void);



/*****************************************************************************
* Function Name : set_csdvc_value
* Description	: switch voice channel device 
* Input 		: integer value  1 : handset  3: speaker phone
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: lhy
* Date		: 2018.08.21
*****************************************************************************/	

int set_csdvc_value(int csdvc_value);

/*****************************************************************************
* Function Name : get_csdvc_value
* Description	: get voice channel device 
* Input 		: None
* Output		: None
* Return		: when success renturn the value represent voice channel device , when fail return -1 
* Auther		: lhy
* Date		: 2018.08.21
*****************************************************************************/	

int get_csdvc_value(void);

/*****************************************************************************
* Function Name : audio_play_start
* Description	: audio play 
* Input 		: filename, repeat times, play path
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: zxs
* Date		: 2018.12.05
*****************************************************************************/	

int audio_play_start(char *filename, int repeat_value, int path_value);

/*****************************************************************************
* Function Name : audio_play_stop
* Description	: stop play
* Input 		: None
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: zxs
* Date		: 2018.12.05
*****************************************************************************/	

int audio_play_stop();

/*****************************************************************************
* Function Name : audio_get_state
* Description	: get audio state

* Input 		: None
* Output		: None
* Return		: 0: stop	 1 :play    -1:fail
* Auther		: zy
* Date		    : 2019.05.09

*****************************************************************************/

int audio_get_state();

/*****************************************************************************
* Function Name : set_codec_ctrl
* Description	: codec control
* Input 		: 0 : mode 0  1: mode 1 
* Output		: None

* Return		: 0: succes	 -1: Fail
* Auther		: zy
* Date		    : 2019.05.09
*****************************************************************************/	

int set_codec_ctrl(int val_ctl);

/*****************************************************************************

* Function Name : get_codec_ctrl
* Description	: get codec state

* Input 		: None
* Output		: None

* Return		: -1 :fail   0: mode 0  1: mode 1
* Auther		: zy
* Date		    : 2019.05.09

*****************************************************************************/

int get_codec_ctrl(void);



/*****************************************************************************
* Function Name : simcom_audio_start_record
* Description	: start audio record
* Input 		: record_type
* Input 		: record_path path part must be rw
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: hyc
* Date		    : 2019.05.23
*****************************************************************************/	
int simcom_audio_start_record(simcom_audio_record_type_e record_type, char *record_path);


/*****************************************************************************
* Function Name : audio_play_stop
* Description	: stop adio play
* Input 		: None
* Output		: None
* Return		: 0 succes	 -1 Fail
* Auther		: hyc
* Date		    : 2019.05.23
*****************************************************************************/	
int simcom_audio_stop_record();

/*****************************************************************************
* Function Name : simcom_audio_get_record_state
* Description	: get audio reord state
* Input 		: None
* Output		: None
* Return		: 1 recording 0 no recoding -1 Fail
* Auther		: hyc
* Date		    : 2019.05.23
*****************************************************************************/	
int simcom_audio_get_record_state();


#endif



