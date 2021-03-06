#ifndef _AWIA_CONSTANTS_INC_GUARDS_H_
#define _AWIA_CONSTANTS_INC_GUARDS_H_

#define STATION_NAME "JOHN DOE"

#define UNUSED -1

#define SDA_PIN 21
#define SCL_PIN 22
#define RX_RST_PIN 17
#define TX_RST_PIN 16

#define RX_MODE_PIN 33
#define TX_MODE_PIN 32

#define LEFT_ENC_SW_PIN 23
#define LEFT_ENC_PIN_A 18
#define LEFT_ENC_PIN_B 19
#define RIGHT_ENC_SW_PIN 14
#define RIGHT_ENC_PIN_A 12
#define RIGHT_ENC_PIN_B 13

#define JP_MINIMUM_FM_MHZ 761
#define JP_MAXIMUM_FM_MHZ 949
#define TX_POWER 115
#define RDS_TEXT_LENGTH 20
#define RDS_READING_TIMEOUT_MILLIS 3000
#define RDS_READING_PERIOD_MILLIS 15000

#define OLED_ADDR 0x3c
#define OLED_TEXT_LEN_BY_ONE_LINE 11
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define LOGO_WIDTH 128
#define LOGO_HEIGHT 32

#define MAIN_TASK_CPU_NO 1
#define READ_RDS_TASK_CPU_NO 0
#define MAIN_TASK_PRIORITY 1 | portPRIVILEGE_BIT
#define READ_RDS_TASK_PRIORITY 2 | portPRIVILEGE_BIT
#define TASK_READ_RDS_PERIODICALLY "read_rds_periodically"
#define TASK_MAIN "main"

#define EEPROM_LENGTH 1024
#define EEPROM_IS_TX_RDS_WROTE_ADDR 0
#define EEPROM_TX_RDS_TEXT_ADDR_BEGIN 1

#define EEPROM_IS_TX_RDS_WROTE_MARKER 0x0a

#endif //_AWIA_CONSTANTS_INC_GUARDS_H_
