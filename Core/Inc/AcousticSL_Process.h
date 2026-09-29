#ifndef INC_ACOUSTICSL_PROCESS_H_
#define INC_ACOUSTICSL_PROCESS_H_

#include "main.h"

#define AUDIO_IN_CHANNELS 					4
#define DIAGONAL 							424 				//직선 30mm // 42//원래 대각선 42.42mm
#define AUDIO_IN_SAMPLING_FREQUENCY 		16000
#define ACOUSTIC_SL_SAMPLES_TO_PROCESS 		512

//extern volatile int32_t result[4];
//extern float g_ch_level_db[4];

extern uint16_t PCM_Buffer_L[(AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY / 1000)];
extern uint16_t PCM_Buffer_R[(AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY / 1000)];

extern int32_t L_result[4];
extern int32_t R_result[4];
extern int32_t L_angle;
extern int32_t R_angle;

void Audio_Libraries_L_Init(void);
void Audio_Libraries_R_Init(void);
void AudioProcess_L(void);
void AudioProcess_R(void);
void SW_Task_L_Start(void);
void SW_Task_R_Start(void);
void SW_Task_L_Callback(void);
void SW_Task_R_Callback(void);

void Audio_FillFromI2S(uint16_t *src, uint32_t active_channels, uint16_t *pcm);
int32_t apply_offset(int32_t angle, int32_t offset);

#endif /* INC_ACOUSTICSL_PROCESS_H_ */
