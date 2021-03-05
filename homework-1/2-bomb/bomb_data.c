#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	int MN_max = 100000;
	int DP_max = 1000000000UL;
	int K_max = 10000;

	int m, n;
	int t;
	int c, d;
	int l, r, p, k;
	int tmp;

	FILE *fd;

	srand(time(NULL));

	fd = fopen("input-max.txt", "w");
	if (fd == NULL) {
		printf("Fail to open file\n");
		return 0;
	}

	n = rand();
	n %= MN_max;
	n += 1;

	m = rand();
	m %= MN_max;
	m += 1;

	fprintf(fd, "%d %d\n", n, m);

	for (int i = 0; i < m; i++) {
		t = rand();
		t %= 100;

		if (t < 50) {
			/* player event */
			c = rand();
			c %= n;
			c += 1;

			d = rand();
			d %= DP_max;
			d += 1;

			fprintf(fd, "P %d %d\n", c, d);
		} else {
			/* bomb event */
			l = rand();
			l %= n;
			l += 1;

			while (1) {
				r = rand();
				r %= n;
				r += 1;

				if (l != r)
					break;
			}

			if (l > r) {
				tmp = l;
				l = r;
				r = tmp;
			}

			p = rand();
			p %= DP_max;
			p += 1;

			k = rand();
			k %= K_max;
			k += 1;

			fprintf(fd, "A %d %d %d %d\n", l, r, p, k);
		}
	}

	fclose(fd);
	return 0;
}
