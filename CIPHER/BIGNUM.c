#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"

void print_mpz(MPZ* a)
{
    SINT32 i;
    for (i = a->len - 1; a >= 0; --a)
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

void Read_MPZ_File(SINT8* filename, MPZ* a, SINT32 num) {
    FILE* fp;
    SINT32 err, i, j, k, pos, bytes;
    SINT8 buf[512], x;

    if ((err = fopen_s(&fp, filename, "r"))) {
        printf("error at opening file\n");
    }

    for (i = 0; i < num; i++) {
        a[i].sign = 0;
        fgets(buf, 512, fp);
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
        for (; k < 8; k++)
            a[i].dat[j] >>= 4;

        for (k = a[i].len - 1; k >= 0; k--)
        {
            if (a[i].dat[k] != 0)
                break;
        }
        a[i].len = k + 1;
        fgets(buf, 512, fp);
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
    SINT8 buf[512], x;

    if ((err = fopen_s(&fp1, filename1, "r"))) {
        printf("error at opening file\n");
    }

    if ((err = fopen_s(&fp2, filename2, "w"))) {
        printf("error at opening file\n");
    }

    for (i = 0; i < num; i++) {
        a.sign = 0;
        fgets(buf, 512, fp1);
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
        fgets(buf, 512, fp1);

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
    SINT8 buf[512], x;

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

void MPZ_UADD(MPZ* c, MPZ* a, MPZ* b) { // a>=0, b>=0
    SINT32 carry, i;
    UINT32 temp;

    if (a->len >= b->len) {
        carry = 0;
        for (i = 0; i < b->len; i++) {
            if ((temp = a->dat[i] + carry) < carry) //carry =1 temp=0
                temp = b->dat[i]; //temp=a[i]+b[i]+carry
            else if ((temp += b->dat[i]) < b->dat[i]) carry = 1;
            else carry = 0;
            c->dat[i] = temp;
        }
        for (i = b->len; i < a->len; i++) {
            if ((temp = a->dat[i] + carry) >= carry) //carry =1 temp=0
                carry = 0; //temp=a[i]+b[i]+carry
            c->dat[i] = temp;
        }
        if (carry == 1) {
            c->dat[a->len] = 1;
            c->len = a->len + 1;
        }
        else c->len = a->len;
    }
}

void MPZ_USUB(MPZ* c, MPZ* a, MPZ* b) {
    SINT32 borrow, i;
    UINT32 temp;

    if (Compare_MPZ(a, b) == 1) {
        c->sign = 0;
        borrow = 0;
        for (i = 0; i < b->len; i++) { //ci=ai-bi-borrow=(ai-borrow)-bi
            if (a->dat[i] >= borrow) {
                temp = a->dat[i] - borrow;
                if ((c->dat[i] = temp - b->dat[i]) > temp)borrow = 1; // temp보다 크면 borrow =1 발생 뺄려는 값보다 결과 값이 크기 때문에 borrow가 발생한다. 
            }
            else // else 문에 들어오는경우는 dat[i]=0 borrow=1 인경우 밖에 없다. 따라서 이미 borrow=1이다. 0-1=0xFFFFFFFF
               //borrow = 1;
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
            }
            else // else 문에 들어오는경우는 dat[i]=0 borrow=1 인경우 밖에 없다. 따라서 이미 borrow=1이다. 0-1=0xFFFFFFFF
               //borrow = 1;
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
    for (i = c->len - 1; i >= 0; i++) {
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
