/*
 * util.h
 *
 *  Created on: Aug 13, 2021
 *      Author: skpark
 */

#ifndef INC_CREDITCARD_H_
#define INC_CREDITCARD_H_


#include "stdio.h"
#include "stdlib.h"
#include "mhscpu.h"

//#include"sys_def.h"

//////////////////////////////////////////////////////////////
// size
#define		MAX_PACKET			1024		// ??? MAX PACKET SIZE
#define		MAX_AID_LIST		17			// ??? AID ?? ??
/////////////////////////////////////////////////////////////
#define		SUCCESS					0
#define		IC_FAILE				1

void sci_prt(void);
uint32_t SCI_Test(uint8_t* pdata);
void ParseBerTLVData(unsigned char *pData, long nDataLen, unsigned char *pResultData, long *pResultDataLen, unsigned char byTag, unsigned char bySecondTag , unsigned char byThirdTag );
int	ReadRecord(unsigned char P1, unsigned char P2, unsigned char Le);
void Util_Decompress(unsigned char *bySource, unsigned char *byDestination, long byCount);
#endif /* INC_UTIL_H_ */
