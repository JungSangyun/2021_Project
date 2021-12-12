#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "typedef.h"

void LtoR_Exp(MPZ* r, MPZ* input, MPZ* exp, MPZ* mod)
{
	MPZ x, y;
	SINT32 i, j;
	COPY_MPZ(&x, input);

	for (i = 31; i >= 0; i--)
	{
		if ((exp->dat[exp->len - 1] & (1 << i)))
			break;
	}

	for (j = i - 1; j >= 0; j--)
	{
		MPZ_MODMUL(r, &x, &x, mod);
		COPY_MPZ(&x, r);
		if (exp->dat[exp->len - 1] & (1 << j))
		{
			MPZ_MODMUL(r, &x, input, mod);
			COPY_MPZ(&x, r);
		}
	}// 최상위 워드 계산하는 코드 

	for (i = exp->len - 2; i >= 0; i--)
	{
		for (j = 31; j >= 0; j--)
		{
			MPZ_MODMUL(r, &x, &x, mod);
			COPY_MPZ(&x, r);
			if (exp->dat[i] & (1 << j))
			{
				MPZ_MODMUL(r, &x, input, mod);
				COPY_MPZ(&x, r);
			}
		}
	}// 나머지 워드 계산하는 코드 
}

void RtoL_Exp(MPZ* r, MPZ* input, MPZ* exp, MPZ* mod)
{
	MPZ x, y, z;
	SINT32 i, j;
	COPY_MPZ(&x, input);

	y.sign = 0;
	y.len = 1;
	y.dat[0] = 1;

	for (i = 0; i <= exp->len - 2; i++)
	{
		for (j = 0; j < 32; j++)
		{
			if (exp->dat[i] & (1 << j))
			{
				MPZ_MODMUL(r, &y, &x, mod);
				COPY_MPZ(&y, r);
			}

			MPZ_MODMUL(&z, &x, &x, mod);
			COPY_MPZ(&x, &z);
		}
	}

	for (i = 31; i >= 0; i--)
	{
		if ((exp->dat[exp->len - 1] & (1 << i)))
			break;
	}
	// 마지막 워드만 계산하기
	for (j = 0; j <= i; j++)
	{
		if (exp->dat[exp->len - 1] & (1 << j))
		{
			MPZ_MODMUL(r, &y, &x, mod);
			COPY_MPZ(&y, r);
		}

		MPZ_MODMUL(&z, &x, &x, mod);
		COPY_MPZ(&x, &z);
	}
}

void LtoR4ary_Exp(MPZ* r, MPZ* input, MPZ* exp, MPZ* mod)
{
	MPZ x, y, S[16];
	SINT32 i, j, wlen, explen;
	for (i = 31; i >= 0; i--)
	{
		if ((exp->dat[exp->len - 1] & (1 << i)))
			break;
	} //32bit 중에 최상위 비트가 몇번째 비트인지 
	explen = (exp->len - 1) * 32 + i; // explen 지수의 총 비트수 31에서 나오면 32를 더해줘야한다. 
	explen++;
	// m 에 대한 사전계산이 필요하다. 
	S[0].dat[0] = 1;
	S[0].len = 1;
	S[0].sign = 0; // S[0]에 대입하기

	for (i = 1; i < 16; i++) {
		MPZ_MODMUL(&S[i], &S[i - 1], input, mod); // 각각의 사비트의 계산이 필요하다. 
	} // 4
	wlen = (explen + 3) / 4;
	// 몇조각인지 정확하게 알기 위해서 (explen + 3) / 4 
	COPY_MPZ(&x, &S[(exp->dat[exp->len - 1] >> (4 * ((wlen + 7) % 8))) & 15]);

	// 첫부분은 복사를 먼저하기 때문에 아래의 for문에서 -2부터 시작을 해도 된다. (1) ^ 2^k연산은 불필요하기 때문에 바로 초기값을 넣어준다.
	for (i = wlen - 2; i > -1; i--) {
		MPZ_MODMUL(&y, &x, &x, mod);// X^2
		MPZ_MODMUL(&x, &y, &y, mod);// X^4
		MPZ_MODMUL(&y, &x, &x, mod);// X^8
		MPZ_MODMUL(&x, &y, &y, mod);// x^16
		MPZ_MODMUL(&x, &x, &S[(exp->dat[i / 8] >> ((i % 8) * 4)) & 15] , mod);
		COPY_MPZ(&x, &y);
	}
	COPY_MPZ(r, &x);
}

void LtoR_Mont_Exp(MPZ* r, MPZ* input, MPZ* exp, MPZ* mod , int tlqkf)
{
	MPZ A, x_,x, y , R2, R2n, temp, one;
	SINT32 i, j;
	UINT32 minv;
	R2.len = mod->len * 2 + 1;
	
	
	memset(R2.dat, 0, sizeof(UINT32) * mod->len * 2);
	R2.dat[2 * mod->len] = 1;//최상위 워드는 1로 채우기
	R2.sign = 0;
	MPZ_UDIV(&temp, &R2n, &R2, mod);
	minv = minus_modinv(mod->dat[0], (UINT64)0x100000000);

	MPZ_UDIV(&A, &temp, input, mod);
	Mont_Mul(&A,input, &R2n, mod, minv);
	COPY_MPZ(&x_, &A);  //A = x_ = input * R

	for (i = 31; i >= 0; i--)
	{
		if ((exp->dat[exp->len - 1] & (1 << i)))
			break;
	}

	for (j = i - 1; j >= 0; j--)
	{
		Mont_Mul(&A, &A, &A, mod , minv); 
		if (exp->dat[exp->len - 1] & (1 << j))
		{
			Mont_Mul(&A , &A,  &x_,mod, minv);
		}
	}

	for (i = exp->len - 2; i >= 0; i--)
	{
		for (j = 31; j >= 0; j--)
		{
			Mont_Mul(&A, &A, &A, mod, minv);
			
			if (exp->dat[i] & (1 << j))
			{
				Mont_Mul(&A, &A, &x_, mod, minv);
			}
		}
	}
	one.dat[0] = 1;
	one.len = 1;
	one.sign = 0;
	Mont_Mul(r, &A, &one, mod, minv);
	
}
void LtoR_Mont_EXP_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* rbuf3, SINT8* wbuf1, SINT32 num) { // 입력 버퍼 3개 출력 버퍼 1개
	MPZ* a, * b, * c, * d;
	SINT32 i, j;
	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);
	d = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);
	Read_MPZ_File(rbuf3, c, num);

	for (i = 0; i < num; i++) {
		LtoR_Mont_Exp(&d[i], &a[i], &b[i], &c[i] , i);
	}

	Write_MPZ_File(wbuf1, c, num);

	free(a);
	free(b);
	free(c);
	free(d);

}
void LtoR_Exp_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* rbuf3, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c, * d;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);
	d = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);
	Read_MPZ_File(rbuf3, c, num);

	for (i = 0; i < num; i++)
	{
		LtoR_Exp(&d[i], &a[i], &b[i], &c[i]);
	}

	Write_MPZ_File(wbuf, d, num);

	free(a);
	free(b);
	free(c);
	free(d);
}

void RtoL_Exp_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* rbuf3, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c, * d;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);
	d = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);
	Read_MPZ_File(rbuf3, c, num);

	for (i = 0; i < num; i++)
	{
		RtoL_Exp(&d[i], &a[i], &b[i], &c[i]);
	}

	Write_MPZ_File(wbuf, d, num);

	free(a);
	free(b);
	free(c);
	free(d);
}
void Lto4ary_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* rbuf3, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c, * d;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);
	d = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_File(rbuf1, a, num);
	Read_MPZ_File(rbuf2, b, num);
	Read_MPZ_File(rbuf3, c, num);

	for (i = 0; i < num; i++)
	{
		LtoR4ary_Exp(&d[i], &a[i], &b[i], &c[i]);
	}

	Write_MPZ_File(wbuf, d, num);

	free(a);
	free(b);
	free(c);
	free(d);
}
