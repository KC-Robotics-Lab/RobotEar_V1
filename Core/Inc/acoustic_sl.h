/**
******************************************************************************
* @file    acoustic_sl.h
* @author  SRA
* @brief   This file contains Acoustic Sound Source Localization library definitions.
******************************************************************************
* @attention
*
* Copyright (c) 2022 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*                        
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ACOUSTIC_SL_H
#define __ACOUSTIC_SL_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "arm_math.h"

/** @addtogroup MIDDLEWARES
* @{
*/

/** @defgroup ACOUSTIC_SL ACOUSTIC_SL
* @{
*/

/* Exported constants --------------------------------------------------------*/

/** @defgroup Acoustic_SL_Exported_Constants AcousticSL Exported Constants
* @{
*/  

/** @defgroup Acoustic_SL_algorithm_type
* @brief    Source Localization algorithm type 
* @{
*/ 
#define ACOUSTIC_SL_ALGORITHM_XCORR                	((uint32_t)0x00000001)
#define ACOUSTIC_SL_ALGORITHM_GCCP                 	((uint32_t)0x00000002)
#define ACOUSTIC_SL_ALGORITHM_BMPH                 	((uint32_t)0x00000004)
/**
* @}
*/

/** @defgroup Acoustic_SL_errors
* @brief    Source Localization errors 
* @{
*/ 
#define ACOUSTIC_SL_ALGORITHM_ERROR                	((uint32_t)0x00000001)
#define ACOUSTIC_SL_PTR_CHANNELS_ERROR             	((uint32_t)0x00000002)
#define ACOUSTIC_SL_CHANNEL_NUMBER_ERROR           	((uint32_t)0x00000004)
#define ACOUSTIC_SL_SAMPLING_FREQ_ERROR            	((uint32_t)0x00000008)
#define ACOUSTIC_SL_RESOLUTION_ERROR               	((uint32_t)0x00000010)
#define ACOUSTIC_SL_THRESHOLD_ERROR                	((uint32_t)0x00000020)
#define ACOUSTIC_SL_DISTANCE_ERROR                 	((uint32_t)0x00000040)
#define ACOUSTIC_SL_NUM_OF_SAMPLES_ERROR           	((uint32_t)0x00000080)
#define ACOUSTIC_SL_PROCESSING_ERROR               	((uint32_t)0x00000100)

#ifndef ACOUSTIC_LOCK_ERROR
#define ACOUSTIC_LOCK_ERROR                      	((uint32_t)0x10000000)
#endif 
/**
* @}
*/  
#define ACOUSTIC_SL_NO_AUDIO_DETECTED              -100  
/**
* @}
*/

/* Exported types ------------------------------------------------------------*/

/** @defgroup Acoustic_SL_Exported_Types AcousticSL Exported Types
* @{
*/
/**
* @brief  Library handler. It keeps track of the static parameters
*         and it handles the internal state of the algorithm.
*/
typedef struct
{
  uint32_t algorithm;                           /*!< Specifies the algorithm to be used between XCORR and GCC-PHAT. This parameter can be a value of @ref 
  Acoustic_SL_algorithm_type. Default value is ACOUSTIC_BF_TYPE_CARDIOID_BASIC */
  
  uint32_t sampling_frequency;                  /*!< Specifies the sampling frequency - for future use */
  
  uint32_t channel_number;                      /*!< Specifies the number of channels, can be 2 for 180� estimation, 4 for 360� estimation. Default value is 
  2. */  
  uint8_t ptr_M1_channels;                      /*!< Number of channels in the stream of Microphone 1. Deafult value is 1. */  
  uint8_t ptr_M2_channels;                      /*!< Number of channels in the stream of Microphone 2. Deafult value is 1. */ 
  uint8_t ptr_M3_channels;                      /*!< Number of channels in the stream of Microphone 3. Deafult value is 1. */
  uint8_t ptr_M4_channels;                      /*!< Number of channels in the stream of Microphone 4. Deafult value is 1. */
  
  uint16_t M12_distance;                        /*!< Distance between Mic1 and Mic2 in decimals of a millimeter. Deafult value is 150. */
  uint16_t M34_distance;                        /*!< Distance between Mic3 and Mic4 in decimals of a millimeter. Deafult value is 150. */
  
  uint32_t internal_memory_size;                /*!< Keeps track of the amount of memory required for the current setup.
  It's filled by the AcousticSL_getMemorySize() function and must
  be used to allocate the right amount of RAM */
  uint32_t * pInternalMemory;                   /*!< Pointer to the memory allocated by the user */
  int16_t samples_to_process;                   /*!< Specifies the number of samples to be processed at a time */      
  
} AcousticSL_Handler_t;

/**
* @brief  Library dynamic configuration handler. It contains dynamic parameters.
*/
typedef struct
{
  uint16_t threshold;                           /*!< Specifies a value related to a voice-activity score. With values below the threshold, the algorithm does not act. The threshold value ranges from 0 to 1000 and the default value is 24. */
  uint32_t resolution;                          /*!< Angle resolution for the algorithms. Ignored if XCORR is used. Deafult value is 4. */
} AcousticSL_Config_t;


/**
* @}
*/

struct _libSoundSourceLoc_Handler_Internal;

typedef struct {
  float32_t  (*SourceLocFunction)(struct _libSoundSourceLoc_Handler_Internal *SLocInternal, int32_t * out_angles);
  uint32_t  (*CheckEventFunction)(struct _libSoundSourceLoc_Handler_Internal *SLocInternal);
}libSoundSourceLoc_Handler_Callbacks;

typedef struct _libSoundSourceLoc_Handler_Internal{
  uint32_t sampling_frequency;
  uint32_t Type;
  uint32_t Buffer_State;
  uint32_t Sample_Number_To_Process;
  uint16_t Sample_Number_Each_ms;
  uint16_t Input_Counter;
  uint16_t EVENT_THRESHOLD;
  uint16_t Mic_Number;
  uint8_t ptr_M1_channels;
  uint8_t ptr_M2_channels;
  uint8_t ptr_M3_channels;
  uint8_t ptr_M4_channels;
  float32_t M12_distance;
  float32_t M34_distance;
  int32_t M12_TAUD;
  int32_t M34_TAUD;
  libSoundSourceLoc_Handler_Callbacks Callbacks;
  int32_t Estimated_Angle_12;
  int32_t Estimated_Angle_34;
  uint32_t resolution;
  void * M1_Data;
  void * M2_Data;
  void * M3_Data;
  void * M4_Data;
  arm_rfft_fast_instance_f32 * SFast;
  float32_t * FFT_Out;
  float32_t * PowerSpectrum;
  float32_t * Phase;
  float32_t * sumArray;
  int32_t * SourceLocFilterArray;
  int32_t * SourceLocFilterArray1;
  float32_t * window;

  //dz
  uint32_t        Sample_Number_To_Store;
  uint8_t *       remap_mic; // 4
  uint16_t         num_of_freq;
  uint16_t         num_of_angles;
  uint16_t        freq_range_min;
  uint16_t        freq_range_max;
  float32_t *     mics_distance; // AUDIO_CHANNELS
  float32_t *     mics_angle; // AUDIO_CHANNELS
  float32_t *     theta; // NUM_ANGLES
  float32_t      phi;//
  uint16_t        mics_write_offset; //starting point in every mic \in {0,16,32,...} \subseteq [0 MIC_LEN)
  uint16_t        mics_read_offset; //starting point in every mic \in 0,MIC_LEN
  int16_t *       score_theta;// RESOLUTION
  uint8_t         reactivity;
  uint8_t         stability;
  uint8_t         array_type;// CIRCULAR_ARRAY
  uint8_t         local_stabilizer;
  float32_t *     mic_tmp;//[DFT_LEN]
  float32_t *     ptr_freq_energies;// ptr
  float32_t *     s;//[2*AUDIO_CHANNELS*NUMOF_FREQ]
  int16_t *       frequencies_under_analysis; // NUM OF FREQ
  int16_t *       sources; // 2* OUTPUT SOURCES
  float32_t *     steering_tau;

} libSoundSourceLoc_Handler_Internal;

#define FACTOR_INDEX_2_HZ               (uint16_t)(SLocInternal->sampling_frequency/SLocInternal->Sample_Number_To_Process)

#define MIN_RESOLUTION                  4U
#define MAX_NUM_OF_ANGLES               (360U/MIN_RESOLUTION)
#define MAX_NUM_OF_FREQUENCIES          16U
#define MAX_AUDIO_CHANNELS              8U
#define MAX_THRESHOLD                   1000U
#define NUM_OUTPUT_SOURCES              4U
#define CIRCULAR_ARRAY                  1U
#define LINEAR_ARRAY                    2U

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @defgroup Acoustic_SL_Exported_Functions AcousticSL Exported Functions
* @{
*/
/**
* @brief  Fills the "internal_memory_size" of the pHandler parameter passed as argument with a value representing the
*         right amount of memory needed by the library, depending on the specific static parameters adopted.
* @param  pHandler: AcousticSL_Handler_t filled with desired parameters.
* @retval 0 if everything is fine.
*/
uint32_t AcousticSL_getMemorySize(AcousticSL_Handler_t * pHandler);

/**
* @brief  Library initialization.
* @param  pHandler: AcousticSL_Handler_t filled with desired parameters.
* @retval 0 if everything is fine.
*         different from 0 if erroneous parameters have been passed to the Init function and the default value has been used.
*         The specific error can be recognized by checking the relative bit in the returned word.
*/
uint32_t AcousticSL_Init(AcousticSL_Handler_t * pHandler);

/**
* @brief  Library data input
* @param  pM1: pointer to an array that contains PCM samples (16 bit signed int)
*         representing 1 ms of data acquired by the first channel.
* @param  pM2: pointer to an array that contains PCM samples (16 bit signed int)
*         representing 1 ms of data acquired by the second channel.
* @param  pM3: pointer to an array that contains PCM samples (16 bit signed int)
*         representing 1 ms of data acquired by the third channel.
* @param  pM4: pointer to an array that contains PCM samples (16 bit signed int)
*         representing 1 ms of data acquired by the fourth channel.
* @param  pHandler: pointer to the handler of the curent Source Localization instance running.
* @retval 1 if data collection is finished and libSoundSourceLoc_Process must be called, 0 otherwise.
* @note   Input function reads samples skipping the required number of values depending on the Ptr_Mx_Channels configuration.
* @note   pM3 and pM4 are ignored in the case the library is setup for using 2 channels.
*/
uint32_t AcousticSL_Data_Input(void *pM1, void *pM2, void *pM3, void *pM4, AcousticSL_Handler_t * pHandler);

/**
* @brief  Library run function, performs audio analysis when all required data has been collected.
* @param  Estimated_Angle: pointer to the int32_t variable that will contain the computed value.
* @param  pHandler: pointer to the handler of the current Source Localization instance running.
* @retval 0 if everything is ok, 1 otherwise
*/
uint32_t AcousticSL_Process(int32_t * Estimated_Angle, AcousticSL_Handler_t * pHandler);

/**
* @brief  Library setup function, it sets the values for threshold and resolution. It can be called at runtime to change
*         dynamic parameters.
* @note   Only the threshold and resolution are evaluated by the SetConfig function.
* @retval 0 if everything is fine.
*         different from 0 if erroneous parameters have been passed to the Init function and the default value has been used.
*         The specific error can be recognized by checking the relative bit in the returned word.
*/
uint32_t AcousticSL_setConfig(AcousticSL_Handler_t * pHandler, AcousticSL_Config_t * pConfig);

/**
* @brief  Fills the pConfig structure with the actual dynamic parameters as they are currently used inside the library.
* @param  pHandler: pointer to the handler of the current Source Localization instance running.
* @param  pConfig: pointer to the dynamic parameters handler that will be filled with the current library configuration
* @retval 0 if everything is fine.
*/
uint32_t AcousticSL_getConfig(AcousticSL_Handler_t * pHandler, AcousticSL_Config_t * pConfig);

/**
* @brief  To be used to retrieve version information.
* @param  version char array to be filled with the current library version
* @retval 0 if everything is fine.
*/
uint32_t AcousticSL_GetLibVersion(char *version);


/**
* @}
*/
/**
* @}
*/

/**
* @}
*/
#endif /* __ACOUSTIC_SL_H */

