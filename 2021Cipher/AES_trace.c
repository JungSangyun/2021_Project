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

#include "simpleserial.h"
#define u8 unsigned char
#define u32 unsigned long
#define MUL2(a) (a << 1) ^ (a & 0x80 ? 0x1b : 0)
#define MUL3(a) (MUL2(a)) ^ (a)
#define MUL4(a) MUL2((MUL2(a)))
#define MUL8(a) MUL2((MUL2((MUL2(a)))))
#define MUL9(a) (MUL8(a)) ^ (a)
#define MULB(a) (MUL8(a)) ^ (MUL2(a)) ^ (a)
#define MULD(a) (MUL8(a)) ^ (MUL4(a)) ^ (a)
#define MULE(a) (MUL8(a)) ^ (MUL4(a)) ^ (MUL2(a))

static const u8 Sbox[256] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
void AddRoundKey(u8 S[], u8 RK[]){	// 입력받은 16bte블럭과 16byte라운드 키를 각 1byte끼리 xor 연산한다.
	S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3];
	S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7];
	S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11];
	S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];
}
u32 u4byte_in(u8 *x);
void AES_KeyWorldToByte(u32 W[], u8 RK[]);
void SubBytes(u8 S[]){
    S[0] = Sbox[S[0]];
    S[1] = Sbox[S[1]];
    S[2] = Sbox[S[2]];
    S[3] = Sbox[S[3]];
    S[4] = Sbox[S[4]];
    S[5] = Sbox[S[5]];
    S[6] = Sbox[S[6]];
    S[7] = Sbox[S[7]];
    S[8] = Sbox[S[8]];
    S[9] = Sbox[S[9]];
    S[10] = Sbox[S[10]];
    S[11] = Sbox[S[11]];
    S[12] = Sbox[S[12]];
    S[13] = Sbox[S[13]];
    S[14] = Sbox[S[14]];
    S[15] = Sbox[S[15]];
    
}

void ShiftRows(u8 S[]){
	u8 temp;
	temp = S[1];S[1] = S[5];S[5] = S[9];S[9] = S[13];S[13] = temp;
	temp = S[2];S[2] = S[10];S[10] = temp;
	temp = S[6];S[6] = S[14];S[14] = temp;
	temp = S[15];S[15] = S[11];S[11] = S[7];S[7] = S[3];S[3] = temp;
}

void MixColumns(u8 S[]){
	u8 temp[16];
	int i;
	for (i = 0; i < 16; i += 4){
		temp[i] = MUL2(S[i]) ^ MUL3(S[i + 1]) ^ S[i + 2] ^ S[i + 3];
		temp[i + 1] = S[i] ^ MUL2(S[i + 1]) ^ MUL3(S[i + 2]) ^ S[i + 3];
		temp[i + 2] = S[i] ^ S[i + 1] ^ MUL2(S[i + 2]) ^ MUL3(S[i + 3]);
		temp[i + 3] = MUL3(S[i]) ^ S[i + 1] ^ S[i + 2] ^ MUL2(S[i + 3]);
	}
	S[0] = temp[0];S[1] = temp[1];S[2] = temp[2];S[3] = temp[3];
	S[4] = temp[4];S[5] = temp[5];S[6] = temp[6];S[7] = temp[7];
	S[8] = temp[8];S[9] = temp[9];S[10] = temp[10];S[11] = temp[11];
	S[12] = temp[12];S[13] = temp[13];S[14] = temp[14];S[15] = temp[15];
}
void AES_ENC(u8 PT[], u8 RK[], u8 CT[], int keysize){
	int Nr = keysize / 32 + 6;
	int i;
	u8 temp[16];

	for (i = 0; i < 16; i++) temp[i] = PT[i];

	AddRoundKey(temp, RK);
	for (i = 0; i < Nr - 1; i++){
		SubBytes(temp);
		ShiftRows(temp);
		MixColumns(temp);
		AddRoundKey(temp, RK + 16 * (i + 1));
	}
	SubBytes(temp);
	ShiftRows(temp);
	AddRoundKey(temp, RK + 16 * (i + 1));
	for (i = 0; i < 16; i++) CT[i] = temp[i];
}
void u4byte_out(u8 *x, u32 y){
	// 입력받은 4byte단위의 블럭 1개를 1byte단위의 블럭 4개로 저장
	x[0] = (y >> 24) & 0xff;
	x[1] = (y >> 16) & 0xff;
	x[2] = (y >> 8) & 0xff;
	x[3] = y & 0xff;
}


u32 Rcons[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000};
#define RotWord(x) ((x << 8) | (x >> 24)) // 4byte x를 x= [x4 x3 x2 x1] 이라 하면 [x3 x2 x1 x4] 형태로 바꾼다.
#define SubWord(x) ((u32)Sbox[(u8)(x >> 24)] << 24) ^ ((u32)Sbox[(u8)(x >> 16)] << 16) ^ ((u32)Sbox[(u8)(x >> 8)] << 8) ^ ((u32)Sbox[(u8)(x >> 0)] << 0)
// 4byte x를 1byte 단위로 sbox연산을 한다. 이때 sBox 는 입력으로 1byte값을 받으므로 4byte x를  1byte단위로 나누어서 준뒤 그결과값을 다시 4byte로 리턴한다.

void RoundKeyGeneration128(u8 MK[], u8 RK[]){
	u32 W[44];
	int i;
	u32 T;
	W[0] = u4byte_in(MK); 		//W[0] = MK[0] || MK[1] || MK[2] || MK[3]
	W[1] = u4byte_in(MK + 4);	//W[1] = MK[4] || MK[5] || MK[6] || MK[7]
	W[2] = u4byte_in(MK + 8);	//W[2] = MK[8] || MK[9] || MK[10] || MK[11]
	W[3] = u4byte_in(MK + 12);	//W[3] = MK[12] || MK[13] || MK[14] || MK[15]
	for (i = 0; i < 10; i++){
		//T=G_func(W[4*i+3]);
		T = W[4 * i + 3]; // T 에 4N+3 번쨰의 W값을 저장 (N은 0~9round)
		T = RotWord(T);		// 바이트 단위 쉬프트 연산
		T = SubWord(T);		// 바이트 단위 SubBytes 연산
		T ^= Rcons[i]; 		// 상수 Rcons과 xor 연산
		W[4 * i + 4] = W[4 * i] ^ T;	// (해당 라운드키의 첫 4byte)= (이전라운드키의 첫4byte 값) xor (이전라운드키의 마지막 4byte가 Gfunc를 거친 값)
		W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];	//(해당 라운드키의 두번쨰 4byte) = (해당 라운드 키의 첫 4byte) xor (이전 라운드키의 두번쨰 4byte)
		W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];	//(해당 라운드키의 세번쨰 4byte) = (해당 라운드 키의 두번쨰 4byte) xor (이전 라운드키의 세번쨰 4byte)
		W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];	//(해당 라운드키의 네번쨰 4byte) = (해당 라운드 키의 세번쨰 4byte) xor (이전 라운드키의 네번쨰 4byte)
	}
	AES_KeyWorldToByte(W, RK); //생성한 4byte단위의 라운드 키들(W)을 1byte단위 라운드키들(RK)로 저장
}
void AES_KeySchedule(u8 MK[], u8 RK[], int keysize){
	if (keysize == 128){
		RoundKeyGeneration128(MK, RK);
	}
	//if(keysize == 192)RoundKeyGeneration192(MK,RK);
	//if(keysize == 256)RoundKeyGeneration1256(MK,RK);
}


u32 u4byte_in(u8 *x){
	// 입력받은 1byte단위의 블럭 4개를 4byte단위의 블럭 1나로 반환
	return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}
void AES_KeyWorldToByte(u32 W[], u8 RK[]){
	int i;								
	for (i = 0; i < 44; i++){
		u4byte_out(RK + 4 * i, W[i]); // RK[4i]||RK[4i+1]||RK[4i+2]||RK[4i+3]
	}
}


uint8_t get_key(uint8_t* k)
{
	// Load key here
	return 0x00;
}

#if SS_VER == SS_VER_2_0
uint8_t get_pt(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t* pt)
#else
uint8_t get_pt(uint8_t* pt)
#endif

{
    u8 keysize = 128;
    u8 MK[16] = {0x1f,0x2f,0x3f,0xf4,0xd5,0x7d,0x88,0x11,0x16,0xc4 ,0xdd,0x12,0x65,0xe4,0x11,0x1a};
    u8 RK[240] = {0x00, };
    //u32 W[60] = {0x00 ,};
    u8 CT[16] = {0,};
    /*
    MK[0] = pt[0] + 0x01;
    MK[1] = pt[1] + 0x01;
    MK[2] = pt[2] + 0x02;
    MK[3] = pt[3] + 0x03;
    MK[4] = pt[4] + 0x04;
    MK[5] = pt[5] + 0x05;
    MK[6] = pt[6] + 0x06;
    MK[7] = pt[7] + 0x07;
    MK[8] = pt[8] + 0x08;
    MK[9] = pt[9] + 0x09;
    MK[10] = pt[10] + 0x0a;
    MK[11] = pt[11] + 0x0b;
    MK[12] = pt[12] + 0x0c;
    MK[13] = pt[13] + 0x0d;
    MK[14] = pt[14] + 0x0e;
    MK[15] = pt[15] + 0x0f;
    */
	/**********************************
	* Start user-specific code here. */
	trigger_high();
	RoundKeyGeneration128(MK, RK);
    AES_ENC(pt,RK,CT,keysize);
	//16 hex bytes held in 'pt' were sent
	//from the computer. Store your response
	//back into 'pt', which will send 16 bytes
	//back to computer. Can ignore of course if
	//not needed
	
	trigger_low();
	/* End user-specific code here. *
	********************************/
    pt[0] = CT[0];
    pt[1] = CT[1];
    pt[2] = CT[2];
    pt[3] = CT[3];
    pt[4] = CT[4];
    pt[5] = CT[5];
    pt[6] = CT[6];
    pt[7] = CT[7];
    pt[8] = CT[8];
    pt[9] = CT[9];
    pt[10] = CT[10];
    pt[11] = CT[11];
    pt[12] = CT[12];
    pt[13] = CT[13];
    pt[14] = CT[14];
    pt[15] = CT[15];
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
	simpleserial_addcmd('p', 16, get_pt);
#if SS_VER != SS_VER_2_0
	simpleserial_addcmd('k', 16, get_key);
	simpleserial_addcmd('x', 0, reset);
#endif
	while(1)
		simpleserial_get();
}
