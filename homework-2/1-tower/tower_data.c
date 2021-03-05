#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	int N_max = 100000;
	int K_max = 400;
	int A_max = 1000000000; /* 10^9 */
	int B_max = 100000;
	int S_max = 2000000000;

	int n, k, a, b, s;
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

	k = rand();
	k %= K_max;
	k += 1;

	a = rand();
	a %= A_max;
	a += 1;

	b = rand();
	b %= B_max;
	b += 1;

	fprintf(fd, "%d %d %d %d\n", n, k, a, b);

	for (int i = 0; i < n; i++) {
		s = rand();
		s %= S_max;
		s -= (S_max/2);

		if (i == 0)
			fprintf(fd, "%d", s);
		else
			fprintf(fd, " %d", s);
	}

	fprintf(fd, "\n");

	fclose(fd);
	return 0;
}
