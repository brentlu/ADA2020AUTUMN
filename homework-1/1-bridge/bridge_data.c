#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv)
{
	int N_max = 200000;
	int XY_max = 1000000000UL;

	int n, x, y;
	int count = 0;

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

	fprintf(fd, "%d\n", n);

	while (count < n) {
		x = rand();
		y = rand();

		if (x == 0 && y == 0)
			continue;

		x %= XY_max;
		y %= XY_max;

		fprintf(fd, "%d %d\n", x, y);
		count++;
	}

	fclose(fd);
	return 0;
}
