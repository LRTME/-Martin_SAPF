/****************************************************************
* FILENAME:     CRC_mitja.h             
* DESCRIPTION:  declarations of 16bit CRC for MODBUS protocol Functions.  
* AUTHOR:       Mitja Nemec, Thomas Pircher
*
****************************************************************/

#ifndef   __CRC_MITJA_H__
#define   __CRC_MITJA_H__


/**************************************************************
* Funckija ki inicializira inicializira izracun crc-ja
**************************************************************/
extern void CRC_init(void);

/**************************************************************
* Funckija ki izracuna delni CRC
**************************************************************/
extern void CRC_add_byte(int byte);

/**************************************************************
* Funckija ki vrne delni CRC
**************************************************************/
extern int CRC_get(void);

#endif  // __CRC_MITJA_H__

