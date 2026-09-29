#include "PCMD3180.h"
#include "i2c.h"
#include "gpio.h"
#include <stdlib.h>   // strtol 필요
#include <stdint.h>
#include "AcousticSL_Process.h"

#define four_ch

uint8_t read_buf[200];

HAL_StatusTypeDef PCMD3180_Write8(uint16_t reg, uint8_t val, uint8_t flag)
{
	if(flag == 0)
		return HAL_I2C_Mem_Write(&hi2c2, PCMD3180_L_I2C_ADDR_8B, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, &val, 1, PCMD3180_I2C_TIMEOUT_MS);
	else
		return HAL_I2C_Mem_Write(&hi2c4, PCMD3180_R_I2C_ADDR_8B, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, &val, 1, PCMD3180_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef PCMD3180_Read8(uint16_t reg, uint8_t *val, uint8_t flag)
{
	if(flag == 0)
		return HAL_I2C_Mem_Read(&hi2c2, PCMD3180_L_I2C_ADDR_8B, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, val, 1, PCMD3180_I2C_TIMEOUT_MS);
	else
		return HAL_I2C_Mem_Read(&hi2c4, PCMD3180_R_I2C_ADDR_8B, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, val, 1, PCMD3180_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef PCMD3180_WriteBytes(uint16_t reg, const uint8_t *buf, uint16_t len, uint8_t flag)
{
	if(flag == 0)
		return HAL_I2C_Mem_Write(&hi2c2, PCMD3180_L_I2C_ADDR_8B, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)buf, len, PCMD3180_I2C_TIMEOUT_MS);
	else
		return HAL_I2C_Mem_Write(&hi2c2, PCMD3180_R_I2C_ADDR_8B, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)buf, len, PCMD3180_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef PCMD3180_ReadBytes(uint16_t reg, uint8_t *buf, uint16_t len, uint8_t flag)
{
	if(flag == 0)
		return HAL_I2C_Mem_Read(&hi2c2, PCMD3180_L_I2C_ADDR_8B, reg, I2C_MEMADD_SIZE_8BIT, buf, len, PCMD3180_I2C_TIMEOUT_MS);
	else
		return HAL_I2C_Mem_Read(&hi2c2, PCMD3180_R_I2C_ADDR_8B, reg, I2C_MEMADD_SIZE_8BIT, buf, len, PCMD3180_I2C_TIMEOUT_MS);
}

void pcmd3180_init(void)
{

	HAL_GPIO_WritePin(GPIOG, RST_MIC_L_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOG, RST_MIC_L_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	PCMD3180_Write8(0x00, 0x00, 0);
	PCMD3180_Write8(0x02, 0x81, 0);
	HAL_Delay(1);

	PCMD3180_Write8(0x07, 0x00, 0);

	PCMD3180_Write8(0x0B, 0x00, 0); // CH1 -> L0
	PCMD3180_Write8(0x0C, 0x01, 0); // CH2 -> R0
	PCMD3180_Write8(0x0D, 0x02, 0); // CH3 -> L1
	PCMD3180_Write8(0x0E, 0x03, 0); // CH4 -> R1

	PCMD3180_Write8(0x3C, 0x40, 0);
	PCMD3180_Write8(0x41, 0x40, 0);
	PCMD3180_Write8(0x46, 0x40, 0);
	PCMD3180_Write8(0x4B, 0x40, 0);

	PCMD3180_Write8(0x3E, 0xEB, 0);
	PCMD3180_Write8(0x43, 0xEB, 0);
	PCMD3180_Write8(0x48, 0xEB, 0);
	PCMD3180_Write8(0x4D, 0xEB, 0);

	PCMD3180_Write8(0x22, 0x41, 0);
	PCMD3180_Write8(0x23, 0x41, 0);

	PCMD3180_Write8(0x2B, 0x45, 0);

	PCMD3180_Write8(0x73, 0xF0, 0);
	PCMD3180_Write8(0x74, 0xF0, 0);

	PCMD3180_Write8(0x75, 0xE0, 0);

	HAL_GPIO_WritePin(GPIOG, RST_MIC_R_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOG, RST_MIC_R_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	PCMD3180_Write8(0x00, 0x00, 1);
	PCMD3180_Write8(0x02, 0x81, 1);
	HAL_Delay(1);

	PCMD3180_Write8(0x07, 0x00, 1);

	PCMD3180_Write8(0x0B, 0x00, 1); // CH1 -> L0
	PCMD3180_Write8(0x0C, 0x01, 1); // CH2 -> R0
	PCMD3180_Write8(0x0D, 0x02, 1); // CH3 -> L1
	PCMD3180_Write8(0x0E, 0x03, 1); // CH4 -> R1

	PCMD3180_Write8(0x3C, 0x40, 1);
	PCMD3180_Write8(0x41, 0x40, 1);
	PCMD3180_Write8(0x46, 0x40, 1);
	PCMD3180_Write8(0x4B, 0x40, 1);

	PCMD3180_Write8(0x3E, 0xEB, 1);
	PCMD3180_Write8(0x43, 0xEB, 1);
	PCMD3180_Write8(0x48, 0xEB, 1);
	PCMD3180_Write8(0x4D, 0xEB, 1);

	PCMD3180_Write8(0x22, 0x41, 1);
	PCMD3180_Write8(0x23, 0x41, 1);

	PCMD3180_Write8(0x2B, 0x45, 1);

	PCMD3180_Write8(0x73, 0xF0, 1);
	PCMD3180_Write8(0x74, 0xF0, 1);

	PCMD3180_Write8(0x75, 0xE0, 1);
//	PCMD3180_ReadBytes(0x00, read_buf, 128);
//	for(int i=0; i<128; i++)
//		printf("reg : 0x%02X  data : 0x%02X \r\n",i, read_buf[i]);
}


//-----------------------------------cmd-------------------------------------------
#ifdef uart1_cli
void pcmd3180_dump(void)
{
	PCMD3180_ReadBytes(0x00, read_buf, 128, 0);
	for(int i=0; i<128; i++)
	{
		char msg[32];
		sprintf(msg, "0x%02X] = 0x%02X\r\n", i, read_buf[i]);
		uart_send_str(msg);
	}
}


int parse_int(const char *s, uint8_t *out)
{
  char *endptr;
  long val = strtol(s, &endptr, 0);  // base=0 → 0x, 0, 10진 자동 인식

  if (s == endptr) return -1;       // 변환 실패
  if (val < 0 || val > 0xFF) return -1;  // 1바이트 범위 체크

  *out = (uint8_t)val;
  return 0;
}

void cmd_pcmdw(char *arg1, char *arg2)
{
  uint8_t reg, data;
  uint8_t buf[1];

  if (parse_int(arg1, &reg) != 0 || parse_int(arg2, &data) != 0)
  {
    uart_send_str("pcmdw usage: pcmdw <addr> <value>\r\n");
    uart_send_str("ex) pcmdw 0x00 0x01\r\n");
    return;
  }

  buf[0] = data;

  if (HAL_I2C_Mem_Write(&hi2c2, PCMD3180_L_I2C_ADDR_8B, reg, I2C_MEMADD_SIZE_8BIT, buf, 1, 100) == HAL_OK)
  {
    uart_send_str("I2C WRITE OK: [");
    char msg[32];
    sprintf(msg, "0x%02X] = 0x%02X\r\n", reg, data);
    uart_send_str(msg);
  }
  else
  {
    uart_send_str("I2C WRITE FAIL\r\n");
  }
}

void cmd_pcmdr(char *arg1)
{
  uint8_t reg;
  uint8_t buf[1];

  if (parse_int(arg1, &reg) != 0)
  {
    uart_send_str("pcmdr usage: pcmdr <addr>\r\n");
    uart_send_str("ex) pcmdr 0x00\r\n");
    return;
  }

  if (HAL_I2C_Mem_Read(&hi2c2, PCMD3180_L_I2C_ADDR_8B, reg, I2C_MEMADD_SIZE_8BIT, buf, 1, 100) == HAL_OK)
  {
    char msg[32];
    uart_send_str("I2C READ OK: [");
    sprintf(msg, "0x%02X] = 0x%02X\r\n", reg, buf[0]);
    uart_send_str(msg);
  }
  else
  {
    uart_send_str("I2C READ FAIL\r\n");
  }
}

void sanitize_backspace(char *cmd)
{
    int w = 0;  // write index

    for (int r = 0; cmd[r] != '\0'; r++)
    {
        uint8_t c = (uint8_t)cmd[r];

        // Backspace(0x08) 또는 DEL(0x7F)
        if (c == '\b' || c == 0x7F)
        {
            if (w > 0) w--;  // 한 글자 지우기
        }
        else if (c == '\r' || c == '\n')
        {
            // 개행은 커맨드 끝으로 취급
            break;
        }
        else
        {
            cmd[w++] = (char)c;
        }
    }

    cmd[w] = '\0';
}

void handle_command(char *cmd)
{
	sanitize_backspace(cmd);
  // 공백 기준으로 토큰 분리
  char *argv[4];
  int argc = 0;

  char *token = strtok(cmd, " ");
  while (token != NULL && argc < 4)
  {
    argv[argc++] = token;
    token = strtok(NULL, " ");
  }

  if (argc == 0)
  {
    return; // 빈 줄
  }

  // ====== 명령어 분기 시작 ======
  if (strcmp(argv[0], "help") == 0)
  {
    uart_send_str("Available commands:\r\n");
//    uart_send_str("  help                 - show this message\r\n");
//    uart_send_str("  ping                 - test response\r\n");
//    uart_send_str("  led on               - turn LED on\r\n");
//    uart_send_str("  led off              - turn LED off\r\n");
    uart_send_str("  pcmdw <addr> <val>   - I2C write\r\n");
    uart_send_str("  pcmdr <addr>         - I2C read\r\n");
    uart_send_str("  dump                 - I2C dump\r\n");
    uart_send_str("  deg                  - AcousticSL Deg\r\n");

  }
  else if(strcmp(argv[0], "deg") == 0)
  {
	  char msg[32];
	  sprintf(msg, "degree : %ld\r\n", L_result[0]);
	  uart_send_str(msg);
  }
//  else if (strcmp(argv[0], "ping") == 0)
//  {
//    uart_send_str("pong\r\n");
//  }
//  else if (strcmp(argv[0], "led") == 0)
//  {
//    if (argc < 2)
//    {
//      uart_send_str("Usage: led on|off\r\n");
//    }
//    else if (strcmp(argv[1], "on") == 0)
//    {
////      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//      uart_send_str("LED ON\r\n");
//    }
//    else if (strcmp(argv[1], "off") == 0)
//    {
////      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//      uart_send_str("LED OFF\r\n");
//    }
//    else
//    {
//      uart_send_str("Usage: led on|off\r\n");
//    }
//  }
  else if (strcmp(argv[0], "pcmdw") == 0)
  {
    if (argc < 3)
    {
      uart_send_str("pcmdw usage: pcmdw <addr> <value>\r\n");
      uart_send_str("ex) pcmdw 0x00 0x01\r\n");
    }
    else
    {
      cmd_pcmdw(argv[1], argv[2]);
    }
  }
  else if (strcmp(argv[0], "pcmdr") == 0)
  {
    if (argc < 2)
    {
      uart_send_str("pcmdr usage: pcmdr <addr>\r\n");
      uart_send_str("ex) pcmdr 0x00\r\n");
    }
    else
    {
      cmd_pcmdr(argv[1]);
    }
  }
  else if (strcmp(argv[0], "dump") == 0)
  {
		  pcmd3180_dump();
  }
  else
  {
    uart_send_str("Unknown command: ");
    uart_send_str(argv[0]);
    uart_send_str("\r\nType 'help' for list.\r\n");
  }
}
#endif
