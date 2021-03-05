#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	int NM_max = 500; /* 1 ~ 500 */
	int K_max = 11; /* 0 ~ 10 */
	int C_max = 1000000000UL;
	int X_max = 2000000000UL;

	int n, m, k, c;
	int n_count = 0;
	int m_count = 0;

	int x;

	FILE *fd;

	srand(time(NULL));

	fd = fopen("input-max.txt", "w");
	if (fd == NULL) {
		printf("Fail to open file\n");
		return 0;
	}

	n = rand();
	n %= NM_max;
	n += 1;

	m = rand();
	m %= NM_max;
	m += 1;

	k = rand();
	k %= K_max;

	c = rand();
	c %= C_max;
	c += 1;

	fprintf(fd, "%d %d %d %d\n", n, m, k, c);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			x = rand();
			x %= X_max;
			x -= 1000000000UL;

			if (j == 0)
				fprintf(fd, "%d", x);
			else
				fprintf(fd, " %d", x);

		}
		fprintf(fd, "\n");
	}

	fclose(fd);
	return 0;
}
