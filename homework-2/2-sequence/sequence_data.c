#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	int N_max = 20;//200;
	int K_max = 0;
	int D_max = 0;
	int A_max = 1000000000; /* 10^9 */
	int C_max = 600;
	int S_max = 1;

	int n, k, d, a, c, s;
	int tmp;

	FILE *fd;

	srand(time(NULL));

	fd = fopen("input-max.txt", "w");
	if (fd == NULL) {
		printf("Fail to open file\n");
		return 0;
	}

	n = rand();
	n %= N_max;
	n += 1;

#if 0
	k = rand();
	k %= K_max;
	k += 1;

	d = rand();
	d %= D_max;
	d += 1;
#else
	k = d = 0;
#endif

	fprintf(fd, "%d %d %d\n", n, k, d);

	for (int i = 0; i < n; i++) {
		a = rand();
		a %= A_max;
		a += 1;

		if (i == 0)
			fprintf(fd, "%d", a);
		else
			fprintf(fd, " %d", a);
	}

	fprintf(fd, "\n");

	for (int i = 0; i < n; i++) {
		c = rand();
		c %= C_max;
		c += 1;

		if (i == 0)
			fprintf(fd, "%d", c);
		else
			fprintf(fd, " %d", c);
	}

	fprintf(fd, "\n");

	for (int i = 0; i < n; i++) {
#if 0
		s = rand();
		s %= S_max;
		s += 1;
#else
		s = 1;
#endif

		if (i == 0)
			fprintf(fd, "%d", s);
		else
			fprintf(fd, " %d", s);
	}

	fprintf(fd, "\n");

	fclose(fd);
	return 0;
}
