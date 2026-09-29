#include "AcousticSL_Process.h"
#include "acoustic_sl.h"
#include "LED.h"
#include <math.h>
#include "usbd_cdc_if.h"

//float g_ch_level_db[4] = {0.0f, };
//float g_ch_dbfs_min[4] = {0};
//float g_ch_dbfs_max[4] = {0};
//volatile uint8_t g_sw_task1_pending = 0;

#define SL_threshold			8

AcousticSL_Handler_t libSoundSourceLoc_Handler_Instance_L;
AcousticSL_Config_t  libSoundSourceLoc_Config_Instance_L;
AcousticSL_Handler_t libSoundSourceLoc_Handler_Instance_R;
AcousticSL_Config_t  libSoundSourceLoc_Config_Instance_R;

uint16_t PCM_Buffer_L[(AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY / 1000)];
uint16_t PCM_Buffer_R[(AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY / 1000)];

int32_t L_result[4];
int32_t R_result[4];
int32_t L_angle = 0;
int32_t R_angle = 0;


void Audio_Libraries_L_Init(void)
{
	__IO uint32_t error_value = 0;
	/* Enable CRC peripheral to unlock the library */
	__CRC_CLK_ENABLE();

	/*Setup Source Localization static parameters*/
	libSoundSourceLoc_Handler_Instance_L.channel_number = AUDIO_IN_CHANNELS;
	libSoundSourceLoc_Handler_Instance_L.M12_distance = DIAGONAL;
	libSoundSourceLoc_Handler_Instance_L.M34_distance = DIAGONAL;
	libSoundSourceLoc_Handler_Instance_L.sampling_frequency = AUDIO_IN_SAMPLING_FREQUENCY;
	libSoundSourceLoc_Handler_Instance_L.algorithm = ACOUSTIC_SL_ALGORITHM_GCCP;
	libSoundSourceLoc_Handler_Instance_L.ptr_M1_channels = 4;
	libSoundSourceLoc_Handler_Instance_L.ptr_M2_channels = 4;
	libSoundSourceLoc_Handler_Instance_L.ptr_M3_channels = 4;
	libSoundSourceLoc_Handler_Instance_L.ptr_M4_channels = 4;
	libSoundSourceLoc_Handler_Instance_L.samples_to_process = ACOUSTIC_SL_SAMPLES_TO_PROCESS;
	(void)AcousticSL_getMemorySize(&libSoundSourceLoc_Handler_Instance_L);
	libSoundSourceLoc_Handler_Instance_L.pInternalMemory = (uint32_t *)malloc(libSoundSourceLoc_Handler_Instance_L.internal_memory_size);
	error_value += AcousticSL_Init(&libSoundSourceLoc_Handler_Instance_L);

	/*Setup Source Localization dynamic parameters*/
	libSoundSourceLoc_Config_Instance_L.resolution = 5;
	libSoundSourceLoc_Config_Instance_L.threshold = SL_threshold;
	error_value += AcousticSL_setConfig(&libSoundSourceLoc_Handler_Instance_L, &libSoundSourceLoc_Config_Instance_L);

	/*Error Management*/
	if (error_value != 0U)
	{
		Error_Handler();
	}

	/*Malloc Failure*/
	if (libSoundSourceLoc_Handler_Instance_L.pInternalMemory == NULL)
	{
		Error_Handler();
	}
}

void Audio_Libraries_R_Init(void)
{
	__IO uint32_t error_value = 0;
	/* Enable CRC peripheral to unlock the library */
	__CRC_CLK_ENABLE();

	/*Setup Source Localization static parameters*/
	libSoundSourceLoc_Handler_Instance_R.channel_number = AUDIO_IN_CHANNELS;
	libSoundSourceLoc_Handler_Instance_R.M12_distance = DIAGONAL;
	libSoundSourceLoc_Handler_Instance_R.M34_distance = DIAGONAL;
	libSoundSourceLoc_Handler_Instance_R.sampling_frequency = AUDIO_IN_SAMPLING_FREQUENCY;
	libSoundSourceLoc_Handler_Instance_R.algorithm = ACOUSTIC_SL_ALGORITHM_GCCP;
	libSoundSourceLoc_Handler_Instance_R.ptr_M1_channels = 4;
	libSoundSourceLoc_Handler_Instance_R.ptr_M2_channels = 4;
	libSoundSourceLoc_Handler_Instance_R.ptr_M3_channels = 4;
	libSoundSourceLoc_Handler_Instance_R.ptr_M4_channels = 4;
	libSoundSourceLoc_Handler_Instance_R.samples_to_process = ACOUSTIC_SL_SAMPLES_TO_PROCESS;
	(void)AcousticSL_getMemorySize(&libSoundSourceLoc_Handler_Instance_R);
	libSoundSourceLoc_Handler_Instance_R.pInternalMemory = (uint32_t *)malloc(libSoundSourceLoc_Handler_Instance_R.internal_memory_size);
	error_value += AcousticSL_Init(&libSoundSourceLoc_Handler_Instance_R);

	/*Setup Source Localization dynamic parameters*/
	libSoundSourceLoc_Config_Instance_R.resolution = 5;
	libSoundSourceLoc_Config_Instance_R.threshold = SL_threshold;
	error_value += AcousticSL_setConfig(&libSoundSourceLoc_Handler_Instance_R, &libSoundSourceLoc_Config_Instance_R);

	/*Error Management*/
	if (error_value != 0U)
	{
		Error_Handler();
	}

	/*Malloc Failure*/
	if (libSoundSourceLoc_Handler_Instance_R.pInternalMemory == NULL)
	{
		Error_Handler();
	}
}

void AudioProcess_L(void)
{
	if (AcousticSL_Data_Input((int16_t *)&PCM_Buffer_L[1], (int16_t *)&PCM_Buffer_L[2],
			(int16_t *)&PCM_Buffer_L[0], (int16_t *)&PCM_Buffer_L[3], &libSoundSourceLoc_Handler_Instance_L) == 1U)
	{
		SW_Task_L_Start();
	}
}

void AudioProcess_R(void)
{
	if (AcousticSL_Data_Input((int16_t *)&PCM_Buffer_R[1], (int16_t *)&PCM_Buffer_R[2],
			(int16_t *)&PCM_Buffer_R[0], (int16_t *)&PCM_Buffer_R[3], &libSoundSourceLoc_Handler_Instance_R) == 1U)
	{
		SW_Task_R_Start();
	}
}

void SW_Task_L_Start(void)
{
//	HAL_NVIC_SetPendingIRQ(EXTI0_IRQn);
	SW_Task_L_Callback();
}

void SW_Task_R_Start(void)
{
//	HAL_NVIC_SetPendingIRQ(EXTI1_IRQn);
	SW_Task_R_Callback();
}

void SW_Task_L_Callback(void)
{
//	char msg[128];
//	int len;
	(void)AcousticSL_Process((int32_t *)&L_result, &libSoundSourceLoc_Handler_Instance_L);
	if (L_result[0] == ACOUSTIC_SL_NO_AUDIO_DETECTED)
	{
		L_result[0] = -1;
		L_angle = 0;
//		len = snprintf(msg, sizeof(msg),
//					   "%.1f, %.1f, %.1f, %.1f,   %.1f, %.1f, %.1f, %.1f,  %.1f, %.1f, %.1f, %.1f,   %ld\r\n",
//					   (double)g_ch_dbfs_min[0],
//					   (double)g_ch_dbfs_min[1],
//					   (double)g_ch_dbfs_min[2],
//					   (double)g_ch_dbfs_min[3],
//					   (double)g_ch_dbfs_max[0],
//					   (double)g_ch_dbfs_max[1],
//					   (double)g_ch_dbfs_max[2],
//					   (double)g_ch_dbfs_max[3],
//					   (double)g_ch_level_db[0],
//					   (double)g_ch_level_db[1],
//					   (double)g_ch_level_db[2],
//					   (double)g_ch_level_db[3],
//					   (long)angle);
//		if (len > 0 && len <= (int)sizeof(msg))
//		{
//			CDC_Transmit_HS_Blocking((uint8_t *)msg, (uint16_t)len, 10);
//		}
	}

	if (L_result[0] != -1)
	{
		L_angle = apply_offset(L_result[0], 225);
//		len = snprintf(msg, sizeof(msg),
//					   "%.1f, %.1f, %.1f, %.1f,   %.1f, %.1f, %.1f, %.1f,  %.1f, %.1f, %.1f, %.1f,   %ld\r\n",
//					   (double)g_ch_dbfs_min[0],
//					   (double)g_ch_dbfs_min[1],
//					   (double)g_ch_dbfs_min[2],
//					   (double)g_ch_dbfs_min[3],
//					   (double)g_ch_dbfs_max[0],
//					   (double)g_ch_dbfs_max[1],
//					   (double)g_ch_dbfs_max[2],
//					   (double)g_ch_dbfs_max[3],
//					   (double)g_ch_level_db[0],
//					   (double)g_ch_level_db[1],
//					   (double)g_ch_level_db[2],
//					   (double)g_ch_level_db[3],
//					   (long)angle);
//		if (len > 0 && len <= (int)sizeof(msg))
//		{
//			CDC_Transmit_HS_Blocking((uint8_t *)msg, (uint16_t)len, 10);
//		}
		LED_L_op(L_angle);
	}
}

void SW_Task_R_Callback(void)
{
	(void)AcousticSL_Process((int32_t *)&R_result, &libSoundSourceLoc_Handler_Instance_R);
	if (R_result[0] == ACOUSTIC_SL_NO_AUDIO_DETECTED)
	{
		R_result[0] = -1;
		R_angle = 0;
	}

	if (R_result[0] != -1)
	{
		R_angle = apply_offset(R_result[0], 45);
		LED_R_op(R_angle);
	}
}

void Audio_FillFromI2S(uint16_t *src, uint32_t active_channels, uint16_t *pcm)
{
	const uint32_t frame_len = (AUDIO_IN_SAMPLING_FREQUENCY / 1000U); // 16
	const uint32_t total_ch  = AUDIO_IN_CHANNELS;                    // 4

//	double sumsq[4] = {0};
//
//	// dBFS min/max를 위한 |s| 최소/최대
//	uint16_t abs_min[4] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
//	uint16_t abs_max[4] = { 0, 0, 0, 0 };

	for (uint32_t n = 0; n < frame_len; n++)
	{
	    uint32_t base = n * total_ch;

	    uint16_t ch1 = 0, ch2 = 0, ch3 = 0, ch4 = 0;

	    if (active_channels >= 1) ch1 = src[base + 0];
	    if (active_channels >= 2) ch2 = src[base + 1];
	    if (active_channels >= 3) ch3 = src[base + 2];
	    if (active_channels >= 4) ch4 = src[base + 3];

	    pcm[base + 0] = ch1;
	    pcm[base + 1] = ch2;
	    pcm[base + 2] = ch3;
	    pcm[base + 3] = ch4;

//	    // RMS 누적 + |s| min/max 갱신
//	    if (active_channels >= 1) {
//	        int16_t s = (int16_t)ch1;
//	        sumsq[0] += (double)s * (double)s;
//	        uint16_t a = (uint16_t)((s < 0) ? -(int32_t)s : s);
//	        if (a < abs_min[0]) abs_min[0] = a;
//	        if (a > abs_max[0]) abs_max[0] = a;
//	    }
//	    if (active_channels >= 2) {
//	        int16_t s = (int16_t)ch2;
//	        sumsq[1] += (double)s * (double)s;
//	        uint16_t a = (uint16_t)((s < 0) ? -(int32_t)s : s);
//	        if (a < abs_min[1]) abs_min[1] = a;
//	        if (a > abs_max[1]) abs_max[1] = a;
//	    }
//	    if (active_channels >= 3) {
//	        int16_t s = (int16_t)ch3;
//	        sumsq[2] += (double)s * (double)s;
//	        uint16_t a = (uint16_t)((s < 0) ? -(int32_t)s : s);
//	        if (a < abs_min[2]) abs_min[2] = a;
//	        if (a > abs_max[2]) abs_max[2] = a;
//	    }
//	    if (active_channels >= 4) {
//	        int16_t s = (int16_t)ch4;
//	        sumsq[3] += (double)s * (double)s;
//	        uint16_t a = (uint16_t)((s < 0) ? -(int32_t)s : s);
//	        if (a < abs_min[3]) abs_min[3] = a;
//	        if (a > abs_max[3]) abs_max[3] = a;
//	    }
	}

//	// --- 기존 RMS dBFS + dBFS(min/max/mid) 저장 ---
//	for (uint32_t ch = 0; ch < active_channels; ch++)
//	{
//	    // 1) 1ms 전체 RMS dBFS (기존)
//	    double rms = sqrt(sumsq[ch] / (double)frame_len);
//	    if (rms < 1.0) rms = 1.0;
//	    g_ch_level_db[ch] = 20.0f * log10f((float)(rms / 32768.0f));
//
//	    // 2) dBFS의 min/max (프레임 내 dBFS 값 기준)
//	    //    RMS 코드 형태 적용: rms_x = sqrt(x^2/1) = x
//	    double rms_abs_min = sqrt(((double)abs_min[ch] * (double)abs_min[ch]) / 1.0);
//	    if (rms_abs_min < 1.0) rms_abs_min = 1.0;     // 0 방지 (바닥 -90.31dBFS)
//
//	    double rms_abs_max = sqrt(((double)abs_max[ch] * (double)abs_max[ch]) / 1.0);
//	    if (rms_abs_max < 1.0) rms_abs_max = 1.0;
//
//	    float min_db = 20.0f * log10f((float)(rms_abs_min / 32768.0));
//	    float max_db = 20.0f * log10f((float)(rms_abs_max / 32768.0));
//
//	    g_ch_dbfs_min[ch] = min_db;                  // dBFS 최소(가장 조용한 샘플)
//	    g_ch_dbfs_max[ch] = max_db;                  // dBFS 최대(피크)
////	    g_ch_dbfs_mid[ch] = 0.5f * (min_db + max_db);
//	}

}

int32_t apply_offset(int32_t angle, int32_t offset)
{
    int16_t out = (int16_t)angle + offset;
    while (out < 0)     out += 360;
    while (out >= 360) 	out -= 360;
    return (uint16_t)out;
}









