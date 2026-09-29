#ifndef INC_PCMD3180_H_
#define INC_PCMD3180_H_

#include "main.h"

#define PCMD3180_L_I2C_ADDR_7B   	0x4C
#define PCMD3180_R_I2C_ADDR_7B   	0x4D
#define PCMD3180_L_I2C_ADDR_8B   	(PCMD3180_L_I2C_ADDR_7B << 1)
#define PCMD3180_R_I2C_ADDR_8B   	(PCMD3180_R_I2C_ADDR_7B << 1)

#define PCMD3180_I2C_TIMEOUT_MS  100

HAL_StatusTypeDef PCMD3180_Write8(uint16_t reg, uint8_t val, uint8_t flag);
HAL_StatusTypeDef PCMD3180_Read8(uint16_t reg, uint8_t *val, uint8_t flag);
HAL_StatusTypeDef PCMD3180_WriteBytes(uint16_t reg, const uint8_t *buf, uint16_t len, uint8_t flag);
HAL_StatusTypeDef PCMD3180_ReadBytes(uint16_t reg, uint8_t *buf, uint16_t len, uint8_t flag);
void pcmd3180_init(void);
uint8_t PCMD3180_ReadDeviceID(void);
uint8_t PCMD3180_ReadGPIMON(void);
uint8_t PCMD3180_ReadPDM_STS(void);
uint8_t PCMD3180_ReadDEV_STS1(void);

void pcmd3180_dump(void);
int parse_int(const char *s, uint8_t *out);
void cmd_pcmdw(char *arg1, char *arg2);
void cmd_pcmdr(char *arg1);
void sanitize_backspace(char *cmd);
void handle_command(char *cmd);

#endif /* INC_PCMD3180_H_ */
