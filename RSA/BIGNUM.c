#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"

void print_mpz(MPZ* a)
{
    SINT32 i;
    for (i = a->len - 1; a >= 0; i--)
    {
        printf("%08x", a->dat[i]);
    }
    printf("\n\n");
}

void Gen_BigNum_File(SINT8* filename, SINT32 bytes, SINT32 num) {
    FILE* fp;
    SINT32 err, i, j;

    if ((err = fopen_s(&fp, filename, "w"))) {
        printf("error at opening file\n");
    }

    for (i = 0; i < num; i++) {
        for (j = 0; j < bytes; j++)
        {
            fprintf(fp, "%02x", rand() & 0xff);
        }
        fprintf(fp, "\n\n");
    }
    fclose(fp);
}
void Gen_BigNum_odd_File(SINT8* filename, SINT32 bytes, SINT32 num) {
    FILE* fp;
    SINT32 err, i, j;

    if ((err = fopen_s(&fp, filename, "w"))) {
        printf("error at opening file\n");
    }

    for (i = 0; i < num; i++) {
        for (j = 0; j < bytes-1; j++)
        {
            
            fprintf(fp, "%02x", rand() & 0xff);
            
        }
        fprintf(fp, "%02x", (rand() & 0xff) | 1);
        fprintf(fp, "\n\n");
    }
    fclose(fp);
}

void Read_MPZ_File(SINT8* filename, MPZ* a, SINT32 num) {
    FILE* fp;
    SINT32 err, i, j, k, pos, bytes;
    SINT8 buf[2048], x;

    if ((err = fopen_s(&fp, filename, "r"))) {
        printf("error at opening file\n");
        exit(0);
    }

    for (i = 0; i < num; i++) {
        a[i].sign = 0;
        fgets(buf, 2048, fp);
        bytes = strlen(buf) / 2;
        a[i].len = (bytes + 3) / 4;
        pos = bytes * 2 - 1;

        for (j = 0; j < a[i].len; j++)
        {
            a[i].dat[j] = 0;
            for (k = 0; k < 8; k++)
            {
                x = buf[pos--];
                if (x >= 'A' && x <= 'F')x = x - 'A' + 10;
                else if (x >= 'a' && x <= 'f')x = x - 'a' + 10;
                else if (x >= '0' && x <= '9')x = x - '0';
                a[i].dat[j] = (x << 28) ^ (a[i].dat[j] >> 4);
                if (pos == -1)break;
            }
        }
        for (; k < 7; k++)
            a[i].dat[a[i].len - 1] >>= 4;

        for (k = a[i].len - 1; k >= 0; k--)
        {
            if (a[i].dat[k] != 0)
                break;
        }
        a[i].len = k + 1;
        fgets(buf, 2048, fp);
    }
    fclose(fp);
}

void Print_MPZ(MPZ* a) { //sign = 0 : non-negative
    SINT32 i;

    for (i = a->len - 1; i >= 0; i--) {
        printf("%08x", a->dat[i]);
    }
    printf("\n\n");
}

void Copy_BigNum_File(SINT8* filename1, SINT8* filename2, SINT32 bytes, SINT32 num) {
    FILE* fp1, * fp2;
    MPZ a;
    SINT32 err, i, j, k, pos;
    SINT8 buf[2048], x;

    if ((err = fopen_s(&fp1, filename1, "r"))) {
        printf("error at opening file\n");
    }

    if ((err = fopen_s(&fp2, filename2, "w"))) {
        printf("error at opening file\n");
    }

    for (i = 0; i < num; i++) {
        a.sign = 0;
        fgets(buf, 2048, fp1);
        bytes = strlen(buf) / 2;
        a.len = (bytes + 3) / 4;
        pos = bytes * 2 - 1;

        for (j = 0; j < a.len; j++)
        {
            a.dat[j] = 0;
            for (k = 0; k < 8; k++)
            {
                x = buf[pos--];
                if (x >= 'A' && x <= 'F')x = x - 'A' + 10;
                else if (x >= 'a' && x <= 'f')x = x - 'a' + 10;
                else if (x >= '0' && x <= '9')x = x - '0';
                a.dat[j] = (x << 28) ^ (a.dat[j] >> 4);
                if (pos == -1)break;
            }
        }
        for (; k < 8; k++)
            a.dat[j] >>= 4;

        for (k = a.len - 1; k >= 0; k--)
        {
            if (a.dat[k] != 0)
                break;
        }
        a.len = k + 1;
        fgets(buf, 2048, fp1);

        for (k = a.len - 1; k >= 0; k--) {
            fprintf(fp2, "%08x", a.dat[k]);
        }
        fprintf(fp2, "\n\n");
    }

    fclose(fp1);
    fclose(fp2);

}

SINT32 Compare_MPZ(MPZ* a, MPZ* b) { // a>=b return 1, else return 0.   
    SINT32 i;
    if ((a->sign == 0) && (b->sign == 1)) return 1;
    else if ((a->sign == 1) && (b->sign == 0)) return 0;
    else if ((a->sign == 0) && (b->sign == 0))
    { // a == b 
        if (a->len > b->len) return 1;
        else if (a->len < b->len)return 0;
        else {
            for (i = a->len - 1; i >= 0; i--) {
                if (a->dat[i] > b->dat[i])return 1;
                else if (a->dat[i] < b->dat[i])return 0;
            }
            return 1;
        }
    }
    else
    {
        if (a->len > b->len) return 0;
        else if (a->len < b->len)return 1;
        else {
            for (i = a->len - 1; i >= 0; i--) {
                if (a->dat[i] > b->dat[i])return 0;
                else if (a->dat[i] < b->dat[i])return 1;
            }
            return 1;
        }
    }
}
void Write_MPZ_File(SINT8* filename, MPZ* a, SINT32 num) {
    FILE* fp;
    SINT32 err, i, j, k, pos, bytes;
    SINT8 buf[2048], x;

    if ((err = fopen_s(&fp, filename, "w"))) {
        printf("error at opening file\n");
    }
    for (i = 0; i < num; i++) {
        for (k = a[i].len - 1; k >= 0; k--)
            fprintf(fp, "%08x", a[i].dat[k]);
        fprintf(fp, "\n\n");
    }
    fclose(fp);
}

void Swap_MPZ(MPZ* a, MPZ* b) {
    MPZ tmp;
    SINT32 i;
    tmp.sign = a->sign;
    tmp.len = a->len;
    for (i = 0; i < a->len; i++)
    {
        tmp.dat[i] = a->dat[i];
    }

    a->sign = b->sign;
    a->len = b->len;
    for (i = 0; i < b->len; i++)
    {
        a->dat[i] = b->dat[i];
    }
    b->sign = tmp.sign;
    b->len = tmp.len;

    for (i = 0; i < tmp.len; i++)
    {
        b->dat[i] = tmp.dat[i];
    }
}

void Sort_MPZ_File(SINT8* filename1, SINT8* filename2, SINT32 num) {
    MPZ* a;
    SINT32 i, j;

    a = (MPZ*)malloc(sizeof(MPZ) * num);
    Read_MPZ_File(filename1, a, num);

    for (i = num - 1; i >= 0; i--)
        for (j = 0; j < i; j++) {
            if (Compare_MPZ(&a[j], &a[j + 1]))Swap_MPZ(&a[j], &a[j + 1]);
        }


    Write_MPZ_File(filename2, a, num);
    free(a);

}

void MPZ_UADD(MPZ* c, MPZ* a, MPZ* b) //a>=0, b>=0
{
    SINT32 carry, i;
    UINT32 temp;

    if (a->len >= b->len) {
        carry = 0;
        for (i = 0; i < b->len; i++)
        {
            if ((temp = a->dat[i] + carry) < carry) //carry = 1 , temp = 0; 캐리가 발생했다면 실행을 한다,
                temp = b->dat[i];     //temp = a[i]+b[i]+carry 이경우가 발생하는 경우는 a가 0xffffffff 이경 우와 c값이 1일 경우 밖에 없다. 
            else if ((temp += b->dat[i]) < b->dat[i]) carry = 1; // 과정 1에서 캐리값이 발생하지 않은경우 b가 더 크다면 캐리가 발생하게 된는 것이다.
            else carry = 0;
            c->dat[i] = temp;
        }
        for (i = b->len; i < a->len; i++) // 두개의 빅 넘버의 길이가 다를 경우 처음 경우만 고려 해주고 나머지를 계산해주면 된다. 
        {
            if ((temp = a->dat[i] + carry) >= carry) //carry = 1 , temp = 0;
                carry = 0;    //temp = a[i]+b[i]+carry
            c->dat[i] = temp;
        }
        if (carry == 1) // 마지막에 캐리값이 1일 경우에 그냥 1하나 더처리해준다. 
        {
            c->dat[a->len] = 1;
            c->len = a->len + 1;
        }
        else c->len = a->len;
    }
    else
    {
        carry = 0;
        for (i = 0; i < a->len; i++)
        {
            if ((temp = b->dat[i] + carry) < carry) //carry = 1 , temp = 0;
                temp = a->dat[i];     //temp = a[i]+b[i]+carry
            else if ((temp += a->dat[i]) < a->dat[i]) carry = 1;
            else carry = 0;
            c->dat[i] = temp;
        }
        for (i = a->len; i < b->len; i++)
        {
            if ((temp = b->dat[i] + carry) >= carry) //carry = 1 , temp = 0;
                carry = 0;    //temp = a[i]+b[i]+carry
            c->dat[i] = temp;
        }
        if (carry == 1)
        {
            c->dat[b->len] = 1;
            c->len = b->len + 1;
        }
        else c->len = b->len;
    }
}

void MPZ_USUB(MPZ* c, MPZ* a, MPZ* b) {
    SINT32 borrow, i;
    UINT32 temp;

    if (Compare_MPZ(a, b) == 1) {
        c->sign = 0;
        borrow = 0;
        for (i = 0; i < b->len; i++) { //ci=ai-bi-borrow=(ai-borrow)-bi
            if (a->dat[i] >= borrow) 
            {
                temp = a->dat[i] - borrow;
                if ((c->dat[i] = temp - b->dat[i]) > temp)borrow = 1; // temp보다 크면 borrow =1 발생 뺄려는 값보다 결과 값이 크기 때문에 borrow가 발생한다. 
                else
                    borrow=0;
            }
            else // else 문에 들어오는경우는 dat[i]=0 borrow=1 인경우 밖에 없다. 따라서 이미 borrow=1이다. 0-1=0xFFFFFFFF
                c->dat[i] = 0xFFFFFFFF - b->dat[i];
        }

        for (i = b->len; i < a->len; i++) { //ci=ai-borrow
            if (a->dat[i] >= borrow) {
                c->dat[i] = a->dat[i] - borrow;
                borrow = 0;
            }
            else
                c->dat[i] = 0xFFFFFFFF;
        }
        c->len = a->len;
        for (i = a->len - 1; i >= 0; i--) {
            if (c->dat[i] == 0) {
                c->len--;
            }
            else
                break;
        }
    }
    else { //c=a-b, b>a ==> c=a-b=-(b-a)
        c->sign = 1;
        borrow = 0;
        for (i = 0; i < a->len; i++) { //ci=bi-ai-borrow=(bi-borrow)-ai
            if (b->dat[i] >= borrow) {
                temp = b->dat[i] - borrow;
                if ((c->dat[i] = temp - a->dat[i]) > temp)borrow = 1; // temp보다 크면 borrow =1 발생 뺄려는 값보다 결과 값이 크기 때문에 borrow가 발생한다. 
                else
                    borrow = 0; 
            }
            else // else 문에 들어오는경우는 dat[i]=0 borrow=1 인경우 밖에 없다. 따라서 이미 borrow=1이다. 0-1=0xFFFFFFFF
                c->dat[i] = 0xFFFFFFFF - a->dat[i];
        }

        for (i = a->len; i < b->len; i++) { //ci=ai-borrow
            if (b->dat[i] >= borrow) {
                c->dat[i] = b->dat[i] - borrow;
                borrow = 0;
            }
            else
                c->dat[i] = 0xFFFFFFFF;
        }
        c->len = b->len;
        for (i = b->len - 1; i >= 0; i--) {
            if (c->dat[i] == 0) {
                c->len--;
            }
            else
                break;
        }
    }
}

void MPZ_ADD(MPZ* c, MPZ* a, MPZ* b) {
    if (a->sign == 0 && b->sign == 0) {
        MPZ_UADD(c, a, b);
        c->sign = 0;
    }
    else if (a->sign == 1 && b->sign == 1) { //c= -((-a)+(-b))
        MPZ_UADD(c, a, b);
        c->sign = 1;
    }
    else if (a->sign == 0 && b->sign == 1) { //a>=0, b>=0 ==> c
        MPZ_USUB(c, a, b);
    }
    else { // c=a+b = b-(-a)
        MPZ_USUB(c, b, a);
    }
}

void MPZ_UADD_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num) {
    MPZ* a, * b, * c;
    SINT32 i, j;
    a = (MPZ*)malloc(sizeof(MPZ) * num);
    b = (MPZ*)malloc(sizeof(MPZ) * num);
    c = (MPZ*)malloc(sizeof(MPZ) * num);

    Read_MPZ_File(rbuf1, a, num);
    Read_MPZ_File(rbuf2, b, num);

    for (i = 0; i < num; i++) {
        MPZ_UADD(&c[i], &a[i], &b[i]);
    }
    Write_MPZ_File(wbuf, c, num);
    free(a);
    free(b);
    free(c);
}

void MPZ_SUB(MPZ* c, MPZ* a, MPZ* b) { //ci=ai-bi-borrow = (ai-borrow)-
    if (a->sign == 0 && b->sign == 0) { // c=a-b=((-b)-(-a)
        MPZ_USUB(c, a, b);
    }
    else if (a->sign == 1 && b->sign == 1) { //c=a-b=((-b)+(a))
        MPZ_USUB(c, b, a);
    }
    else if (a->sign == 0 && b->sign == 1) { //a>=0, b>=0 ==> c
        MPZ_USUB(c, a, b);
        c->sign = 0;
    }
    else { // c=a-b=-((b)+(-a))
        MPZ_USUB(c, b, a);
        c->sign = 1;
    }
}

void MPZ_USUB_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num) {
    MPZ* a, * b, * c;
    SINT32 i, j;
    a = (MPZ*)malloc(sizeof(MPZ) * num);
    b = (MPZ*)malloc(sizeof(MPZ) * num);
    c = (MPZ*)malloc(sizeof(MPZ) * num);

    Read_MPZ_File(rbuf1, a, num);
    Read_MPZ_File(rbuf2, b, num);

    for (i = 0; i < num; i++) {
        MPZ_USUB(&c[i], &a[i], &b[i]);
    }
    Write_MPZ_File(wbuf, c, num);
    free(a);
    free(b);
    free(c);
}
/*
void MPZ_UMUL(MPZ* c, MPZ* a, MPZ* b) {
    SINT32 i ,j ;
    UINT32 U;

    SINT64 UV;
    for (i = 0; i < a->len; i++) {
        c->dat[i] = 0;
    }
    for (i = 0; i < a->len; i++) {
        U = 0;
        for (j = 0; j < a->len; j++) {
            UV = c->dat[i + j] + a->dat[i] * b->dat[j] + U;
            c->dat[i + j] = UV & 0xffffffff;
        }
        c->dat[i + a->len] = UV >> 32;
    }
}
*/
void MPZ_UMUL(MPZ* c, MPZ* a, MPZ* b) {
    SINT32 i, j;
    UINT64 UV;
    UINT32 U;
    for (i = 0; i < b->len; i++)
        c->dat[i] = 0;


    for (i = 0; i < a->len; i++) {
        U = 0;
        for (j = 0; j < b->len; j++) {
            UV = c->dat[i + j] + (UINT64)a->dat[i] * b->dat[j] + U;
            U = UV >> 32;
            c->dat[i + j] = UV & 0xffffffff;
        }
        c->dat[i + b->len] = U;
    }
    c->len = a->len + b->len;
    for (i = c->len - 1; i >= 0; i--) {
        if (c->dat[i] == 0)
            c->len--;
        else
            break;
    }
}
void MPZ_UMUL_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num) {
    MPZ* a, * b, * c;
    SINT32 i, j;
    a = (MPZ*)malloc(sizeof(MPZ) * num);
    b = (MPZ*)malloc(sizeof(MPZ) * num);
    c = (MPZ*)malloc(sizeof(MPZ) * num);

    Read_MPZ_File(rbuf1, a, num);
    Read_MPZ_File(rbuf2, b, num);

    for (i = 0; i < num; i++) {
        MPZ_UMUL(&c[i], &a[i], &b[i]);
    }
    Write_MPZ_File(wbuf, c, num);
    free(a);
    free(b);
    free(c);
}
void MPZ_MUL(MPZ* c, MPZ* a, MPZ* b) {
    if (a->sign == 0 && b->sign == 0) { 
        MPZ_UMUL(c, a, b);
        c->sign = 0;
    }
    else if (a->sign == 1 && b->sign == 1) { 
        MPZ_UMUL(c, a, b);
        c->sign = 0;
    }
    else if (a->sign == 0 && b->sign == 1) { 
        MPZ_UMUL(c, a, b);
        c->sign = 1;
        if (c->len == 0) {
            c->sign = 0;
        }
    }
    else { 
        MPZ_UMUL(c, a, b);
        c->sign = 1;
        if (c->len == 0) {
            c->sign = 0;
        }
    }
}
void COPY_MPZ(MPZ* r, MPZ* a) {
    SINT32 i;
    for (i = 0; i < a->len; i++) {
        r->dat[i] = a->dat[i];
    }
    r->len = a->len;
    r->sign = a->sign;
}
void MPZ_WORD_SHIFT(MPZ* r, MPZ* a, SINT32 shift) //if shift>=0, left shift. Else, right shift
{
    SINT32 i;

    if (shift == 0)
    {
        COPY_MPZ(r, a);
    }
    else if (shift > 0)
    {
        for (i = a->len - 1; i >= 0; i--)
            r->dat[i + shift] = a->dat[i];
        for (i = shift - 1; i >= 0; i--)
            r->dat[i] = 0;
        r->sign = a->sign;
        r->len = a->len + shift;
    }
    else
    {
        for (i = -shift; i < a->len; i++)
            r->dat[i + shift] = a->dat[i];
        r->sign = a->sign;
        r->len = a->len + shift;
        if (r->len <= 0)
        {
            r->len = 0;
            r->sign = 0;
        }
    }
}
void MPZ_BIT_SHIFT(MPZ* r, MPZ* a, SINT32 shift) //if shift>=0, left shift. Else, right shift, -32<shift<32
{
    SINT32 i;

    if (shift == 0)
    {
        COPY_MPZ(r, a);
    }
    else if (shift > 0)
    {
        r->sign = a->sign;
        r->len = a->len + 1;
        r->dat[a->len] = a->dat[a->len - 1] >> (32 - shift);
        for (i = a->len - 1; i >= 1; i--)
        {
            r->dat[i] = (a->dat[i] << shift) | (a->dat[i - 1] >> (32 - shift));
        }
        r->dat[0] = a->dat[0] << shift;
        if (r->dat[a->len] == 0)
            r->len = a->len;
    }
    else
    {
        r->sign = a->sign;
        r->len = a->len;
        for (i = 0; i < a->len - 1; i++)
        {
            r->dat[i] = (a->dat[i] >> (-shift)) | (a->dat[i + 1] << (32 + shift));
        }
        r->dat[a->len - 1] = a->dat[a->len - 1] >> (-shift);
        if (r->dat[a->len - 1] == 0)
            r->len = a->len - 1;
        if (r->len == 0)
            r->sign = 0;
    }
}

void MUL_WORD_MPZ(MPZ* r, UINT32 t, MPZ* a) {
    SINT32 i;
    UINT64 UV;
    UINT32 U = 0;
    UINT32 V;
    r->sign = a->sign;
    r->len = a->len;
    for (i = 0; i < a->len; i++) {

        UV = (UINT64)a->dat[i] * t + U;
        U = UV >> 32; 
        r->dat[i] = UV & 0xffffffff;

    }
    if (U != 0) {
        r->dat[a->len] = U;
        r->len = a->len + 1;
    }
}
void MPZ_UDIV(MPZ* q, MPZ* r, MPZ* a, MPZ* b) { // a>=0 , b>=0
    UINT32 x0, x1, x2;
    UINT64 x_ ,y_ , z_;
    SINT32 i;
    SINT32 normbits;
    MPZ x ,y ,temp, Qtemp;
    MPZ Qmulytemp;
    MPZ temp2;
    if (Compare_MPZ(a, b) == 0) {
        q->len = 0; 
        q->sign = 0;
        COPY_MPZ(r, a);
    }
    else {
        // b->dat[b->len -1]
        //beginning of normalization ;;
        for (i = 31; i >= 0; i--) {
            if ((b->dat[b->len - 1] & (1 << i))) {
                break;
            }
        } //  normalization을 위해서 다음의 b의 최상의 비트의 위치를 찾는과정 (과정 0)

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        normbits = 31 - i;
        MPZ_BIT_SHIFT(&x, a ,normbits); // normalization
        MPZ_BIT_SHIFT(&y, b ,normbits);
        
        for (i = 0; i <= x.len - y.len; i++) {
            q->dat[i] = 0;
        }
        q->len = x.len - y.len + 1; 
        // 몫의 자리수에 0을 채워야한다. step1
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPZ_WORD_SHIFT(&temp, &y, x.len - y.len); 

        if (Compare_MPZ(&x, &temp) == 1) {
            q->dat[x.len - y.len]++;
            MPZ_USUB(&x, &x, &temp);
        }
        // step 2. 몫의 가장 높은 자리수를 찾기 위해서 조건이 맞을 경우 temp(y << k)를 계속해서 x에서 빼줘야한다. (과정 2)

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

     
        for (i = x.len - 1; i >= y.len; i--) {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // step 3.1
            if (x.dat[i] == y.dat[y.len - 1]) {
                q->dat[i - y.len] = 0xffffffff; 
            }
            else {
                q->dat[i - y.len] = (((UINT64)x.dat[i] << 32) | x.dat[i - 1]) / y.dat[y.len - 1];
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //step 3.2  // q와 y의 두개의 워드를 곱하는 과정을 구현해야하고 ,그 둘의 크기를 비교하는 값을 구현해야함. 
            for (;;) {
                x_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 2];
                x0 = x_ & 0xffffffff;
                y_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 1];
                y_ += (x_ >> 32);
                x1 = y_ & 0xffffffff;
                x2 = y_ >> 32;

                if (x2 < x.dat[i])
                    break;
                else if (x2 == x.dat[i]) {
                    if (x1 < x.dat[i - 1]) {
                        break;
                    }
                    if (x1 == x.dat[i - 1]) {
                        if (x0 <= x.dat[i - 2])
                            break;
                    }
                }
                q->dat[i - y.len]--;
            }
            /*
            x_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 2];
            x0 = x_ & 0xffffffff;
            y_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 1];
            y_ += x_ >> 32;
            x1 = y_ & 0xffffffff;
            x2 = y_ >> 32;

            if (x2 > x.dat[i])
                q->dat[i - y.len]--;
            else if (x2 == x.dat[i]) {
                if (x1 > x.dat[i - 1]) {
                    q->dat[i - y.len]--;
                }
                else if (x1 == x.dat[i - 1]) {
                    if (x0 > x.dat[i - 2])
                        q->dat[i - y.len]--;
                }
            }
            */
            /*
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // step 3.3 
            // q->data[i - y.len]를 어떤 MPZ에 담고,그다음에 곱을 진행하는 것이 편함.
            // q->data[i - y.len] 를 변수 MPZ변수에 담기
            Qtemp.len = 1;
            Qtemp.sign = 0;
            Qtemp.dat[0] = q->dat[i - y.len];
            MPZ_UMUL(&Qmulytemp , &temp , &Qtemp); // 무조건 양수임
            MPZ_SUB(&x, &x, &Qmulytemp); //  음수가 나올 수도 있기 때문에 SUB 사용
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // step 3.4 
            if (x.sign == 1) {
                MPZ_ADD(&x, &x, &temp); // 음수랑 양수랑 더하는 것이기 때문에 ADD 사용
                q->dat[i - y.len]--;
            }
            */

            MPZ_WORD_SHIFT(&temp , &y , i - y.len); // y*b^(i-t-1)
            MUL_WORD_MPZ(&temp2, q->dat[i - y.len] , &temp);
            MPZ_USUB(&x, &x, &temp2);
            if (x.sign == 1) {
                MPZ_ADD(&x, &x, &temp);
                q->dat[i - y.len]--;
            }
            
        }
        MPZ_BIT_SHIFT(r, &x, -normbits);
        q->sign = 0;
        for (i = q->len - 1; i >= 0; i--) {
            if (q->dat[i] == 0)
                q->len--;
            else
                break;
        }
    }
    
}
void MPZ_MODADD(MPZ *r , MPZ *a ,  MPZ *b , MPZ *n) {
    MPZ x;
    MPZ_ADD(&x, a, b);
    if (Compare_MPZ(&x, n) == 1) {
        MPZ_SUB(r, r, n);
    }
}
void MPZ_MODSUB(MPZ* r, MPZ* a, MPZ* b, MPZ* n) {
    MPZ_SUB(r, a, b);
    if (r->sign == 1) {
        MPZ_ADD(r, r, n);
    }
}
void MPZ_MODMUL(MPZ* r, MPZ* a, MPZ* b, MPZ* n) {
    MPZ x, y;
    MPZ_MUL(&x, a, b);
    MPZ_UDIV(&y, r, &x, n);
}
UINT32 minus_modinv(SINT64 m, SINT64 mod) // return -m^(-1) mod (mod) mod와 m의 크기가 상관이 없다.
{
    SINT64 r1, r2, r, q;
    SINT64 t1 = 0, t2 = 1, t;

    if (mod == 1)
        return 0;
    r1 = mod;
    r2 = m;

    while (r1 > 1)
    {
        q = r1 / r2;
        r = r1 % r2;
        t = t1 - q * t2;
        r1 = r2;
        r2 = r;
        t1 = t2;
        t2 = t;
    }
    t1 = (-t1) % mod; //범위내에 저장
    if (t1 < 0) t1 += mod; // 음수가 되는것을 방지하기 위해서 더해준다. 
    return (UINT32)t1;
    
}


void Mont_Mul(MPZ *r , MPZ *x , MPZ *y , MPZ *mod , UINT32 minv) //  = r = xyR^-1
{
    SINT32 i;
    UINT32 u;
    MPZ temp ,A;
    A.len = 0;
    A.sign = 0;
    A.dat[0] = 0;

    for (i = 0; i < x->len; i++) {
        
        u = (A.dat[0] + x->dat[i] * y->dat[0])  * minv; // 어짜피 32bit 짜리 변수여서 자동으로 넘어가는 것은 없어진다. 
        MUL_WORD_MPZ(&temp, x->dat[i], y);
        MPZ_UADD(&A, &A, &temp);
        MUL_WORD_MPZ(&temp, u, mod);
        MPZ_UADD(&A, &A, &temp);
        MPZ_WORD_SHIFT(&A, &A, -1);

        
    }
    for (i = x->len; i < mod->len; i++) { // x가 mod보다 작을때는 x의 최상위 비트를 0이라고 생각하고 다시 계산을 한다.

        u = (A.dat[0]) * minv; // 어짜피 32bit 짜리 변수여서 자동으로 넘어가는 것은 없어진다. 
        MUL_WORD_MPZ(&temp, u, mod);
        MPZ_UADD(&A, &A, &temp);
        MPZ_WORD_SHIFT(&A, &A, -1);
    }
    if (Compare_MPZ(&A, mod) == 1) {
        MPZ_USUB(r, &A, mod);
    }
    else
        COPY_MPZ(r, &A);

    for (i = r->len - 1; i >= 0; i--) {
        if (r->dat[i] == 0)
            r->len--;
        else
            break;
    }
}
void MPZ_modinv(MPZ* result, MPZ* a, MPZ* mod) {
    MPZ r1, r2, r, q;
    MPZ t1, t2, t, one ,two , temp;

    COPY_MPZ(&r1, mod);
    COPY_MPZ(&r2, a);
    t1.dat[0] = 0;
    t1.len = 0;
    t1.sign = 0;

    t2.dat[0] = 1;
    t2.len = 1;
    t2.sign = 0;

    two.dat[0] = 2;
    two.len = 1;
    two.sign = 0;


    while (Compare_MPZ(&r1, &two)) { // r1>2
        MPZ_UDIV(&q, &r, &r1, &r2);
        MPZ_MODMUL( &temp ,&q, &t2 , mod);
        MPZ_MODSUB(&t, &t1, &temp , mod);
        COPY_MPZ(&r1, &r2);
        COPY_MPZ(&r2, &r);
        COPY_MPZ(&t1, &t2);
        COPY_MPZ(&t2, &t);

        
    }
    COPY_MPZ(result, &t1);
}
void MPZ_UDIV_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf1, SINT32 * wbuf2, SINT32 num) {
    MPZ* a, * b, * c , *d;
    SINT32 i, j;
    a = (MPZ*)malloc(sizeof(MPZ) * num);
    b = (MPZ*)malloc(sizeof(MPZ) * num);
    c = (MPZ*)malloc(sizeof(MPZ) * num);
    d = (MPZ*)malloc(sizeof(MPZ) * num);

    Read_MPZ_File(rbuf1, a, num);
    Read_MPZ_File(rbuf2, b, num);

    for (i = 0; i < num; i++) {
        MPZ_UDIV(&c[i], &d[i],  &a[i], &b[i]);
        printf("hihi");
    }
    Write_MPZ_File(wbuf1, c, num);
    Write_MPZ_File(wbuf2, d, num);
    free(a);
    free(b);
    free(c);
}
