#if !defined (EEPROM_H)
#define EEPROM_H


#define USE_EEPROM

#if defined (USE_EEPROM)
	#define EE_SIZE							E2END+1	// EEPROM didys
	#define EE_DATA_OK						0xAA	// EEPROM kontrolinis baitas
	
	/* EEPROM adresai */
	#define EE_ADDR_INIT					0x3EF		// kontrolinio baito adresas (1 baitas)
	#define EE_START						(5u)
	#define EE_ADDR_MBADDR					(EE_START + 0u)		// Modbus adreso lasteles adresas (1 baitas)
	#define EE_ADDR_MBBAUD					(EE_START + 6u)		// Modbus baudreito adresas (2 baitai)
	#define EE_ADDR_MBPARITY 				(EE_START + 8u)		// Modbus Parity adresas (1 baitas)

	#define EE_ADDR_MAX_RPM					(EE_START + 11u)		// MAX RPM eeprom adresas (1 baitas)
	#define EE_ADDR_MIN_RPM					(EE_START + 12u)		// MIN RPM eeprom adresas (1 baitas)
	#define EE_ADDR_RPM						(EE_START + 13u)		// 

	#define EE_ADDR_SCROLL_TIMEOUT			(EE_START + 14u)		// Scroll timeout/minutes (1 baitas)
	#define EE_ADDR_SCROLL_PRC				(EE_START + 15u)		// Skroll greitis procentais (1 baitas)
	#define EE_ADDR_SCROLL_ROTATIONS		(EE_START + 16u)		// Scroll apsisukimu skaicius (2 baitai)
	#define EE_ADDR_HALL_SENSOR_TIMEOUT		(EE_START + 18u)		// Hall Sensor timeout/sekundes (2 baitai)

	#define EE_ADDR_MOTOR_WTIME_COUNTER_LO	(EE_START + 20u)		// Stepperio darbo laiko kounteris LOW int (2 baitai)
	#define EE_ADDR_MOTOR_WTIME_COUNTER_HI	(EE_START + 22u)		// Stepperio darbo laiko kounteris HI int (2 baitai)
	#define EE_ADDR_MOTOR_WTIME_DAYS		(EE_START + 24u)		// Stepperio darbo laikas/dienom (2 baitai)

	#define EE_ADDR_MOTOR_WTIME_COUNTER		EE_ADDR_MOTOR_WTIME_COUNTER_LO	// Stepperio darbo laiko kounteris (4 baitai) - kai reikia uint32

	#define EE_ADDR_STEPS_PER_ROT			(EE_START + 26u)		// Variklio zingsniu kiekis per apsisukima (1 baitas)
#endif


char EEPROM_GetChar(uint16_t);
void EEPROM_PutChar(uint16_t, uint8_t);
uint16_t EEPROM_GetWord(uint16_t);
void EEPROM_PutWord(uint16_t, uint16_t);
uint32_t EEPROM_GetDWord(uint16_t);
void EEPROM_PutDWord(uint16_t, uint32_t);

#endif	/* EEPROM_H */