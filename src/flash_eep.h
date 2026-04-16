/******************************************************************************
 * FileName: flash_eep.h
 * Description: FLASH
 * Alternate SDK 
 * Author: pvvx
 ******************************************************************************/
#ifndef __FLASH_EEP_H_
#define __FLASH_EEP_H_

#ifdef __cplusplus
extern "C" {
#endif

// EEPROM IDs
enum {
	EEP_ID_VER		= 0, // EEP ID blk: unsigned int = minimum supported version
	EEP_ID_DEV_CFG	= 0x01, // EEP ID device config
	EEP_ID_DEV_NAME	= 0x02, // EEP ID device name (max 20 char)
	EEP_ID_PINCODE	= 0x03, // EEP ID pincode
	EEP_ID_TEMP_CFG	= 0x10, // EEP ID temperature config
	EEP_ID_BKEY		= 0x20, // EEP ID bindkey
	EEP_ID_DMAC		= 0x30  // EEP ID devs mac
}EEP_ID_e;
//-----------------------------------------------------------------------------
#define FLASH_BASE_ADDR			0x00000000
#define FLASH_SECTOR_SIZE		4096
#define FMEMORY_EEP_BANKS_SHL 	2 // кол-во секторов для работы 2,4,8,..
#define FMEMORY_EEP_BANKS_SIZE	(FLASH_SECTOR_SIZE << FMEMORY_EEP_BANKS_SHL) // размер FMEMORY
#define FMEMORY_EEP_BASE_ADDR1	(0x10000 - (FMEMORY_EEP_BANKS_SIZE)) // 0x0C000
#define FMEMORY_EEP_BASE_ADDR2	(FMEMORY_EEP_BASE_ADDR1 + FMEMORY_EEP_BANKS_SIZE) // 0x0C000

//--Option---------------------------------------------------------------------
#define USE_EEP_BANKS	0
#define MAX_FOBJ_SIZE	32 // максимальный размер сохраняемых объeктов (32..512)
//-----------------------------------------------------------------------------
typedef enum {
	FMEM_NOT_FOUND = -1,	//  -1 - не найден
	FMEM_SIZE_ERR  = -2,	//  -2 - задан неверный размер
	FMEM_OVERFLOW  = -3		//  -3 - переполнение банка
} fmemory_errors_t;
//-----------------------------------------------------------------------------
#if USE_EEP_BANKS
s32 flash_read_cfg(void *ptr, u8 nv, u8 id, u8 maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
s32 flash_write_cfg(void *ptr, u8 nv, u8 id, u8 size);
#else
s32 flash_read_cfg(void *ptr, u8 id, u8 maxsize); // возврат: размер объекта последнего сохранения, -1 - не найден, -2 - error
s32 flash_write_cfg(void *ptr, u8 id, u8 size);
#endif
/* ver = (SW version << 16) | (HW ) version */
bool flash_supported_eep_ver(u32 min_ver, u32 new_ver);
void flash_erase_all_cfg(void);
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif /* __FLASH_EEP_H_ */
