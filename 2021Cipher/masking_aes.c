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
unsigned char mask[10];
unsigned char MSbox[256];
unsigned char Sbox[256] = {
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

unsigned Rcon[10] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

void Mask_RK_Update(unsigned char* out, unsigned char* in, int i)
{
   unsigned char tmp[4];

   tmp[0] = in[13] ^ mask[7];
   tmp[1] = in[14] ^ mask[8];
   tmp[2] = in[15] ^ mask[9];
   tmp[3] = in[12] ^ mask[6];

   tmp[0] = MSbox[tmp[0]] ^ Rcon[i];
   tmp[1] = MSbox[tmp[1]];
   tmp[2] = MSbox[tmp[2]];
   tmp[3] = MSbox[tmp[3]];

   // 첫번째 블록 
   out[0] = in[0] ^ tmp[0] ^ mask[1];
   out[1] = in[1] ^ tmp[1] ^ mask[1];
   out[2] = in[2] ^ tmp[2] ^ mask[1];
   out[3] = in[3] ^ tmp[3] ^ mask[1];
   // 두번째 블록
   out[4] = in[4] ^ mask[0] ^ out[0] ^ mask[6];
   out[5] = in[5] ^ mask[0] ^ out[1] ^ mask[7];
   out[6] = in[6] ^ mask[0] ^ out[2] ^ mask[8];
   out[7] = in[7] ^ mask[0] ^ out[3] ^ mask[9];
   // 세번째 블록
   out[8] = in[8] ^ mask[0] ^ out[4] ^ mask[6];
   out[9] = in[9] ^ mask[0] ^ out[5] ^ mask[7];
   out[10] = in[10] ^ mask[0] ^ out[6] ^ mask[8];
   out[11] = in[11] ^ mask[0] ^ out[7] ^ mask[9];
   // 네번째 블록
   out[12] = in[12] ^ mask[6] ^ out[8] ^ mask[0];
   out[13] = in[13] ^ mask[7] ^ out[9] ^ mask[0];
   out[14] = in[14] ^ mask[8] ^ out[10] ^ mask[0];
   out[15] = in[15] ^ mask[9] ^ out[11] ^ mask[0];
}
void RK_Update(unsigned char* out, unsigned char* in, int i)
{
   unsigned char tmp[4];

   unsigned Rcon[10] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };


   tmp[0] = in[13]; tmp[1] = in[14]; tmp[2] = in[15]; tmp[3] = in[12];

   tmp[0] = Sbox[tmp[0]] ^ Rcon[i];
   tmp[1] = Sbox[tmp[1]];
   tmp[2] = Sbox[tmp[2]];
   tmp[3] = Sbox[tmp[3]];

   out[0] = in[0] ^ tmp[0]; out[1] = in[1] ^ tmp[1]; out[2] = in[2] ^ tmp[2]; out[3] = in[3] ^ tmp[3];
   out[4] = in[4] ^ out[0]; out[5] = in[5] ^ out[1]; out[6] = in[6] ^ out[2]; out[7] = in[7] ^ out[3];
   out[8] = in[8] ^ out[4]; out[9] = in[9] ^ out[5]; out[10] = in[10] ^ out[6]; out[11] = in[11] ^ out[7];
   out[12] = in[12] ^ out[8]; out[13] = in[13] ^ out[9]; out[14] = in[14] ^ out[10]; out[15] = in[15] ^ out[11];
}

void Mask_RK_Update2(unsigned char* out, unsigned char* in)
{
   unsigned char tmp[4];

   tmp[0] = in[13] ^ mask[7];
   tmp[1] = in[14] ^ mask[8];
   tmp[2] = in[15] ^ mask[9];
   tmp[3] = in[12] ^ mask[6];

   tmp[0] = MSbox[tmp[0]] ^ Rcon[9];
   tmp[1] = MSbox[tmp[1]];
   tmp[2] = MSbox[tmp[2]];
   tmp[3] = MSbox[tmp[3]];

   // 첫번째 블록
   out[0] = in[0] ^ tmp[0];
   out[1] = in[1] ^ tmp[1];
   out[2] = in[2] ^ tmp[2];
   out[3] = in[3] ^ tmp[3];
   // 두번째 블록
   out[4] = in[4] ^ out[0];
   out[5] = in[5] ^ out[1];
   out[6] = in[6] ^ out[2];
   out[7] = in[7] ^ out[3];
   // 세번째 블록
   out[8] = in[8] ^ mask[6] ^ out[4];
   out[9] = in[9] ^ mask[7] ^ out[5];
   out[10] = in[10] ^ mask[8] ^ out[6];
   out[11] = in[11] ^ mask[9] ^ out[7];
   // 네번째 블록
   out[12] = in[12] ^ mask[6] ^ out[8];
   out[13] = in[13] ^ mask[7] ^ out[9];
   out[14] = in[14] ^ mask[8] ^ out[10];
   out[15] = in[15] ^ mask[9] ^ out[11];

   // 마스크 옳바르게 바꾸기 
   out[0] = out[0] ^ mask[6] ^ mask[0];
   out[1] = out[1] ^ mask[7] ^ mask[0];
   out[2] = out[2] ^ mask[8] ^ mask[0];
   out[3] = out[3] ^ mask[9] ^ mask[0];

   out[8] = out[8] ^ mask[0];
   out[9] = out[9] ^ mask[0];
   out[10] = out[10] ^ mask[0];
   out[11] = out[11] ^ mask[0];
}


void Mask_KeyExpansion(unsigned char* MK, unsigned char* RK)
{
   int i;

   for (i = 0; i < 16; i += 4)
   {
      RK[i] = MK[i] ^ mask[6] ^ mask[0];
      RK[i + 1] = MK[i + 1] ^ mask[7] ^ mask[0];
      RK[i + 2] = MK[i + 2] ^ mask[8] ^ mask[0];
      RK[i + 3] = MK[i + 3] ^ mask[9] ^ mask[0];
   }

   for (i = 0; i < 9; i++)
   {
      Mask_RK_Update(RK + 16, RK, i);
      RK += 16;
   }

   Mask_RK_Update2(RK + 16, RK);
}
uint8_t get_key(uint8_t* k)
{
	// Load key here
	return 0x00;
}
void KeyExpansion(unsigned char* MK, unsigned char* RK)
{
   int i;

   for (i = 0; i < 16; i++)
      RK[i] = MK[i];

   for (i = 0; i < 10; i++) {

      RK_Update(RK + 16, RK, i);
      RK += 16;
   }
}


void KeyAddition(unsigned char* input, unsigned char* RK)
{
   int i;

   for (i = 0; i < 16; i++)
      input[i] ^= RK[i];
}


void Mask_Subbyte(unsigned char* input)
{
   int i;

   for (i = 0; i < 16; i++)
      input[i] = MSbox[input[i]];
}

void Subbyte(unsigned char* input)
{
   int i;

   for (i = 0; i < 16; i++)
      input[i] = Sbox[input[i]];
}


void ShiftRow(unsigned char* input)
{
   unsigned char tmp;

   tmp = input[5]; input[5] = input[9]; input[9] = input[13]; input[13] = input[1]; input[1] = tmp;

   tmp = input[10]; input[10] = input[2]; input[2] = tmp;
   tmp = input[14]; input[14] = input[6]; input[6] = tmp;

   tmp = input[11]; input[11] = input[7]; input[7] = input[3]; input[3] = input[15]; input[15] = tmp;
}


unsigned char MUL2(unsigned char a)
{
   if (a & 0x80)
      return (a << 1) ^ 0x1b;
   else
      return a << 1;
}

unsigned char MUL3(unsigned char a)
{
   return MUL2(a) ^ a;
}


void Submix(unsigned char* in)
{
   unsigned char tmp[4];

   tmp[0] = MUL2(in[0]) ^ MUL3(in[1]) ^ in[2] ^ in[3];
   tmp[1] = in[0] ^ MUL2(in[1]) ^ MUL3(in[2]) ^ in[3];
   tmp[2] = in[0] ^ in[1] ^ MUL2(in[2]) ^ MUL3(in[3]);
   tmp[3] = MUL3(in[0]) ^ in[1] ^ in[2] ^ MUL2(in[3]);

   in[0] = tmp[0]; in[1] = tmp[1]; in[2] = tmp[2]; in[3] = tmp[3];
}


void MixColums(unsigned char* input)
{
   Submix(input);
   Submix(input + 4);
   Submix(input + 8);
   Submix(input + 12);
}


void Remask(unsigned char* input)
{
   for (int i = 0; i < 16; i += 4)
   {
      input[i] = input[i] ^ (mask[1] ^ mask[2]);
      input[i + 1] = input[i + 1] ^ (mask[1] ^ mask[3]);
      input[i + 2] = input[i + 2] ^ (mask[1] ^ mask[4]);
      input[i + 3] = input[i + 3] ^ (mask[1] ^ mask[5]);
   }
}


void Mask_ENC(unsigned char* in, unsigned char* RK, unsigned char* out)
{
   int i;
   unsigned char temp[16];

   // 평문 마스킹 //
   for (i = 0; i < 16; i += 4)
   {
      temp[i] = in[i] ^ mask[6];
      temp[i + 1] = in[i + 1] ^ mask[7];
      temp[i + 2] = in[i + 2] ^ mask[8];
      temp[i + 3] = in[i + 3] ^ mask[9];
   }

   for (i = 0; i < 9; i++)
   {
      KeyAddition(temp, RK);
      Mask_Subbyte(temp);
      ShiftRow(temp);
      Remask(temp);
      MixColums(temp);
      RK += 16;
   }

   KeyAddition(temp, RK);
   Mask_Subbyte(temp);
   ShiftRow(temp);
   RK += 16;
   KeyAddition(temp, RK);

   for (i = 0; i < 16; i++)
   {
      out[i] = temp[i];
   }
}

void ENC(unsigned char* in, unsigned char* RK, unsigned char* out)
{
   int i;
   unsigned char temp[16];

   for (i = 0; i < 16; i++)
   {
      temp[i] = in[i];
   }

   for (i = 0; i < 9; i++)
   {
      KeyAddition(temp, RK);
      Subbyte(temp);
      ShiftRow(temp);
      MixColums(temp);
      RK += 16;
   }

   KeyAddition(temp, RK);
   Subbyte(temp);
   ShiftRow(temp);
   RK += 16;
   KeyAddition(temp, RK);

   for (i = 0; i < 16; i++)
   {
      out[i] = temp[i];
   }
}

#if SS_VER == SS_VER_2_0
uint8_t get_pt(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t* pt)
#else
uint8_t get_pt(uint8_t* pt)
#endif

{
	int i;
    unsigned char MK[16] = {0xe1,0x2f,0x3f,0xf4,0xd5,0x7d,0x88,0x11,0x16,0xc4 ,0xdd,0x12,0x65,0xe4,0x11,0x1a};
    unsigned char CT[16] = {0x00, };
    unsigned char RK[176] = {0x00 , };
    
	trigger_high();
    mask[0] = Sbox[pt[0]];
    mask[1] = Sbox[pt[1]];
    mask[2] = Sbox[pt[2]];
    mask[3] = Sbox[pt[3]];
    mask[4] = Sbox[pt[4]];
    mask[5] = Sbox[pt[5]];
    for (i = 0; i < 256; i++)
      MSbox[i ^ mask[0]] = Sbox[i] ^ mask[1];
    mask[6] = MUL2(mask[2]) ^ MUL3(mask[3]) ^ mask[4] ^ mask[5];
    mask[7] = mask[2] ^ MUL2(mask[3]) ^ MUL3(mask[4]) ^ mask[5];
    mask[8] = mask[2] ^ mask[3] ^ MUL2(mask[4]) ^ MUL3(mask[5]);
    mask[9] = MUL3(mask[2]) ^ mask[3] ^ mask[4] ^ MUL2(mask[5]);
    
    
	Mask_KeyExpansion(MK, RK);
    Mask_ENC(pt, RK, CT);
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

	trigger_low();
	/* End user-specific code here. *
	********************************/
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
