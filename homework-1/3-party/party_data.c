#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DISTINCT_X

int bag[500000];

int main(int argc, char **argv)
{
	int K_max = 500000;
	int N_max = 500000;
	int X_max = 1000000000UL;

	int k, n, x, i;
	int count = 0;
	int found = 0;

	FILE *fd;

	srand(time(NULL));

	fd = fopen("input-max.txt", "w");
	if (fd == NULL) {
		printf("Fail to open file\n");
		return 0;
	}

	while (1) {
		n = rand();
		n %= N_max;
		n += 1;

		if (n >= 2)
			break;
	}

	k = rand();
	k %= K_max;
	k += 1;

	fprintf(fd, "%d %d\n", n, k);

	while (count < n) {
		x = rand();
		x %= X_max;
		x += 1;

#ifdef DISTINCT_X
		for (i = 0; i < count; i++) {
			if (bag[i] == x)
				break;
		}
#endif

		if (i == count) {
			bag[count] = x;
			count++;
		}
	}

	for (int i = 0; i < n; i++) {
		if (i == 0)
			fprintf(fd, "%d", bag[i]);
		else
			fprintf(fd, " %d", bag[i]);

	}

	fprintf(fd, "\n");

	fclose(fd);
	return 0;
}
