#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GROUP_1

int main(int argc, char **argv)
{
#ifdef GROUP_1
	int N_max = 8;
	int C_max = 11;
#else
	int N_max = 99999;
	int C_max = 1000001;
#endif

	int n, c, f;

	FILE *fd;

	srand(time(NULL));

	fd = fopen("input-max.txt", "w");
	if (fd == NULL) {
		printf("Fail to open file\n");
		return 0;
	}

	n = rand();
	n %= N_max;
	n += 2;
#ifdef GROUP_1
	n = 4;
#endif

	fprintf(fd, "%d\n", n);

	for (int i = 0; i < n; i++) {
		c = rand();
		c %= C_max;

#ifdef GROUP_1
//		if (i % 2 == 0)
//			c = 0;
#endif

		if (i == 0)
			fprintf(fd, "%d", c);
		else
			fprintf(fd, " %d", c);
	}

	fprintf(fd, "\n");

	f = rand();
	f %= 2;

#ifdef GROUP_1
	f = 0;
#endif


	fprintf(fd, "%d\n", f);

	fclose(fd);
	return 0;
}
