/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "simpleserial.h"
#define HARDWARE
#define SOFTWARE



#define PIPO64_128
//#define PIPO64_256


#define ROUND 13
#define SIZE 2				//64 = 32 * 2
#define INT_NUM 2			//64 = 32 * 2
#define MASTER_KEY_SIZE 2	//128 = 64 * 2


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void ROUND_KEY_GEN(void);
u32 MASTER_KEY[MASTER_KEY_SIZE * INT_NUM] = { 0, };
u32 ROUND_KEY[(ROUND + 1) * INT_NUM] = { 0, };
u32 PLAIN_TEXT[SIZE] = { 0, };
u32 CIPHER_TEXT[SIZE] = { 0, };
u8 Sbox[256] = { 0x5E, 0xF9, 0xFC, 0x00, 0x3F, 0x85, 0xBA, 0x5B, 0x18, 0x37, 0xB2, 0xC6, 0x71, 0xC3, 0x74, 0x9D, 0xA7, 0x94, 0x0D, 0xE1, 0xCA, 0x68, 0x53, 0x2E, 0x49, 0x62, 0xEB, 0x97, 0xA4, 0x0E, 0x2D, 0xD0, 0x16, 0x25, 0xAC, 0x48, 0x63, 0xD1, 0xEA, 0x8F, 0xF7, 0x40, 0x45, 0xB1, 0x9E, 0x34, 0x1B, 0xF2, 0xB9, 0x86, 0x03, 0x7F, 0xD8, 0x7A, 0xDD, 0x3C, 0xE0, 0xCB, 0x52, 0x26, 0x15, 0xAF, 0x8C, 0x69, 0xC2, 0x75, 0x70, 0x1C, 0x33, 0x99, 0xB6, 0xC7, 0x04, 0x3B, 0xBE, 0x5A, 0xFD, 0x5F, 0xF8, 0x81, 0x93, 0xA0, 0x29, 0x4D, 0x66, 0xD4, 0xEF, 0x0A, 0xE5, 0xCE, 0x57, 0xA3, 0x90, 0x2A, 0x09, 0x6C, 0x22, 0x11, 0x88, 0xE4, 0xCF, 0x6D, 0x56, 0xAB, 0x7B, 0xDC, 0xD9, 0xBD, 0x82, 0x38, 0x07, 0x7E, 0xB5, 0x9A, 0x1F, 0xF3, 0x44, 0xF6, 0x41, 0x30, 0x4C, 0x67, 0xEE, 0x12, 0x21, 0x8B, 0xA8, 0xD5, 0x55, 0x6E, 0xE7, 0x0B, 0x28, 0x92, 0xA1, 0xCC, 0x2B, 0x08, 0x91, 0xED, 0xD6, 0x64, 0x4F, 0xA2, 0xBC, 0x83, 0x06, 0xFA, 0x5D, 0xFF, 0x58, 0x39, 0x72, 0xC5, 0xC0, 0xB4, 0x9B, 0x31, 0x1E, 0x77, 0x01, 0x3E, 0xBB, 0xDF, 0x78, 0xDA, 0x7D, 0x84, 0x50, 0x6B, 0xE2, 0x8E, 0xAD, 0x17, 0x24, 0xC9, 0xAE, 0x8D, 0x14, 0xE8, 0xD3, 0x61, 0x4A, 0x27, 0x47, 0xF0, 0xF5, 0x19, 0x36, 0x9C, 0xB3, 0x42, 0x1D, 0x32, 0xB7, 0x43, 0xF4, 0x46, 0xF1, 0x98, 0xEC, 0xD7, 0x4E, 0xAA, 0x89, 0x23, 0x10, 0x65, 0x8A, 0xA9, 0x20, 0x54, 0x6F, 0xCD, 0xE6, 0x13, 0xDB, 0x7C, 0x79, 0x05, 0x3A, 0x80, 0xBF, 0xDE, 0xE9, 0xD2, 0x4B, 0x2F, 0x0C, 0xA6, 0x95, 0x60, 0x0F, 0x2C, 0xA5, 0x51, 0x6A, 0xC8, 0xE3, 0x96, 0xB0, 0x9F, 0x1A, 0x76, 0xC1, 0x73, 0xC4, 0x35, 0xFE, 0x59, 0x5C, 0xB8, 0x87, 0x3D, 0x02, 0xFB};


uint8_t get_key(uint8_t* k)
{
	// Load key here
	return 0x00;
}
void ROUND_KEY_GEN(void) {
	u32 i, j;
	u32 RCON = 0;
	srand(time(NULL));
	



	
	for (i = 0; i < MASTER_KEY_SIZE; i++) 
		for (j = 0; j < INT_NUM; j++) 
			MASTER_KEY[INT_NUM*i + j] = rand() | (rand() << 16);	
	
	//PIPO-64/128 test vector
	MASTER_KEY[0] = 0x2E152297;
	MASTER_KEY[1] = 0x7E1D20AD;
	MASTER_KEY[2] = 0x779428D2;
	MASTER_KEY[3] = 0x6DC416DD;


	////PIPO-64/256 test vector
	//MASTER_KEY[7] = 0x009A3AA4;
	//MASTER_KEY[6] = 0x76A96DB5;
	//MASTER_KEY[5] = 0x54A71206;
	//MASTER_KEY[4] = 0x26D15633;
	//MASTER_KEY[3] = 0x6DC416DD;
	//MASTER_KEY[2] = 0x779428D2;
	//MASTER_KEY[1] = 0x7E1D20AD;
	//MASTER_KEY[0] = 0x2E152297;
	



	for (i = 0; i < ROUND + 1; i++) {
		for (j = 0; j < INT_NUM; j++) 
			ROUND_KEY[INT_NUM*i + j] = MASTER_KEY[(INT_NUM*i + j) % (MASTER_KEY_SIZE*INT_NUM)];		
		ROUND_KEY[INT_NUM*i] ^= RCON;
		RCON++;
		
	}	
}

void keyadd(u8* val, u8* rk)
{
	val[0] ^= rk[0];
	val[1] ^= rk[1];
	val[2] ^= rk[2];
	val[3] ^= rk[3];
	val[4] ^= rk[4];
	val[5] ^= rk[5];
	val[6] ^= rk[6];
	val[7] ^= rk[7];
}

void convert(u8* X)
{
	int i,j;
	u8 T[8] = { 0, };
	for (i = 0; i < 8; i++) 
		for (j = 0; j < 8; j++) 
			T[i] |= (((X[j] & (1<<i))>>i) << j);
	
	for (i = 0; i < 8; i++)
		X[i] = T[i];	
}

void sbox_TLU(u8 *X) {
	int i;
	convert(X);
	for (i = 0; i < 8; i++)
		X[i] = Sbox[X[i]];
    
    
	convert(X);
}
void pbox(u8* X)
{
	X[1] = ((X[1] << 7)) | ((X[1] >> 1));
	X[2] = ((X[2] << 4)) | ((X[2] >> 4));
	X[3] = ((X[3] << 3)) | ((X[3] >> 5));
	X[4] = ((X[4] << 6)) | ((X[4] >> 2));
	X[5] = ((X[5] << 5)) | ((X[5] >> 3));
	X[6] = ((X[6] << 1)) | ((X[6] >> 7));
	X[7] = ((X[7] << 2)) | ((X[7] >> 6));

}
void ENC_TLU(u32* PLAIN_TEXT, u32* ROUND_KEY, u32* CIPHER_TEXT) {
	int i = 0;
	u8* P = (u8*)PLAIN_TEXT;
	u8* RK = (u8*)ROUND_KEY;

	keyadd(P, RK);

    
	for (i = 1; i < ROUND + 1; i++)
	{
	
		sbox_TLU(P);

		pbox(P);

		keyadd(P, RK + (i * 8));


	}
}
#if SS_VER == SS_VER_2_0
uint8_t get_pt(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t* pt)
#else
uint8_t get_pt(uint8_t* pt)
#endif

{
    int i = 0;
	u8* P = (u8*)PLAIN_TEXT;
    u8* RK = (u8*)ROUND_KEY;
    PLAIN_TEXT[0] = (pt[0] << 24) ^ (pt[1] << 16) ^ (pt[2] << 8) ^ (pt[3]);
    PLAIN_TEXT[1] = (pt[4] << 24) ^ (pt[5] << 16) ^ (pt[6] << 8) ^ (pt[7]);
    ROUND_KEY_GEN();
	trigger_high();
    
    keyadd(P, RK);


	for (i = 1; i < 4; i++)
	{
	
		sbox_TLU(P); // 

		pbox(P); //  

		keyadd(P, RK + (i * 8));


	}
    

	trigger_low();
    for (i = 4; i < ROUND+1; i++)
	{
	
		sbox_TLU(P);

		pbox(P);

		keyadd(P, RK + (i * 8));


	}
    
    pt[0] = PLAIN_TEXT[1] >> 24;
    pt[1] = PLAIN_TEXT[1] >> 16;
    pt[2] = PLAIN_TEXT[1] >> 8;
    pt[3] = PLAIN_TEXT[1] >> 0;
    pt[4] = PLAIN_TEXT[0] >> 24;
    pt[5] = PLAIN_TEXT[0] >> 16;
    pt[6] = PLAIN_TEXT[0] >> 8;
    pt[7] = PLAIN_TEXT[0] >> 0;
    

    
	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t reset(uint8_t* x)
{
	// Reset key here if needed
	return 0x00;
}


int main(void)
{
    platform_init();
	init_uart();	
	trigger_setup();
	
 	/* Uncomment this to get a HELLO message for debug */
	/*
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	*/
		
	simpleserial_init();		
	simpleserial_addcmd('p',16, get_pt);
#if SS_VER != SS_VER_2_0
	simpleserial_addcmd('k', 16, get_key);
	simpleserial_addcmd('x', 0, reset);
#endif
	while(1)
		simpleserial_get();
}
