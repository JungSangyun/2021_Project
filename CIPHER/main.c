#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "typedef.h"

int main()
{
	LARGE_INTEGER Frequency;
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	SINT64 elapsed;
	SINT32 i;
	double duringtime;
	MPZ* a;
	a = (MPZ*)malloc(sizeof(MPZ) * 1000);

	QueryPerformanceFrequency(&Frequency);
	
	srand(time(NULL));

	QueryPerformanceCounter(&BeginTime);

	//Gen_BigNum_File("rand_A.txt", 128, 1000);
	//Gen_BigNum_File("rand_B.txt", 128, 1000);

	//Read_MPZ_File("rand_A.txt", a, 1000);
	//for (i = 0; i < 1000; i++) {
	//	
	//	Print_MPZ(a[i]);
	//}

	//Copy_BigNum_File("rand_A.txt", "Copy_rand_B.txt" , 128, 1000);

	//Sort_MPZ_File("rand_A.txt", "Sorted_A.txt", 1000);





	/*for (i = 0; i < 1000; i++)
	{
		print_mpz(&a[i]);
	}*/

	MPZ_UMUL_TEST("rand_A.txt", "rand_B.txt", "rand_M.txt", 1000);


	QueryPerformanceCounter(&EndTime);

	elapsed = EndTime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;

	printf("elapsed time: %lf\n", duringtime);

	free(a);

	return 0;

}
