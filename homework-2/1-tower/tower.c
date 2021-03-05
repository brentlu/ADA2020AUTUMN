#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG
//#define FULL_DP_MATRIX
//#define PRINT_BACKTRACE
//#define DUMP_DP_MATRIX

typedef signed long long s64;
typedef unsigned long long u64;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LINE_SIZE (1000000 * 16)
#define N 100000
#define K 400

static char line[LINE_SIZE];

static int my_cell[N];
static int cell_num, budget;
static s64 bonus_a, bonus_b; /* avoid overflow */

#ifdef FULL_DP_MATRIX
static s64 my_defense[K][N];
#else
static s64 my_defense[2][N];
#endif


#ifdef FULL_DP_MATRIX
#ifdef DUMP_DP_MATRIX
void dump(void) {
	for (int j = 0; j < cell_num; j++) {
		printf("%d,", j);
	}
	printf("\n");

	for (int i = 0; i < budget; i++) {
		for (int j = 0; j < cell_num; j++) {
			if (j < i)
				printf("0,");
			else
				printf("%lld,", my_defense[i][j]);
		}
		printf("\n");
	}
}
#endif

#ifdef PRINT_BACKTRACE
void backtrace(int max_i, int max_j)
{
	int i = max_i, j = max_j;

	printf("  %d-th tower selects %d-th cell, %d\n", i, j, my_cell[j]);

	while (i > 0) {
		if (i == j) {
			i -= 1;
			j -= 1;
		} else {
			int max_valid = 0, prev_j;
			s64 max_defence, tmp;

			for (int k = i-1; k < j; k++) {
				tmp = my_defense[i-1][k] + my_cell[j] + (bonus_a*MAX((bonus_b-(j-k)), 0));

				if (max_valid == 0) {
					max_defence = tmp;
					prev_j = k;
					max_valid = 1;
				} else if (tmp > max_defence) {
					max_defence = tmp;
					prev_j = k;
				}
			}

			i -= 1;
			j = prev_j;
		}

		printf("  %d-th tower selects %d-th cell, %d\n", i, j, my_cell[j]);
	}

}
#endif

s64 tower(void)
{
	s64 max_defence = 0;
#ifdef PRINT_BACKTRACE
	int max_i, max_j;
#endif

	for (int j = 0; j < cell_num; j++) {
		my_defense[0][j] = my_cell[j];

		if (my_defense[0][j] > max_defence) {
			max_defence = my_defense[0][j];

#ifdef PRINT_BACKTRACE
			max_i = 0;
			max_j = j;
#endif
		}
	}

	for (int i = 1; i < budget; i++) {
		s64 tmp;
		int max1, max2;

		max1 = max2 = i-1;
		for (int j = i; j < cell_num; j++) {
			/* check (i-1,j-1) */

			/*
			 * max1: index of max defence
			 * max2: index of max defence without bonus
			 */
			if (my_defense[(i-1)%2][j-1] > my_defense[(i-1)%2][max2])
				max2 = j-1;

			tmp = my_defense[(i-1)%2][j-1] + bonus_a*MAX(bonus_b-1, 0);
			if (tmp > my_defense[(i-1)%2][max1] + bonus_a*MAX(bonus_b-(j-max1), 0))
				max1 = j-1;

			tmp = my_defense[(i-1)%2][max2];
			if (tmp > my_defense[(i-1)%2][max1] + bonus_a*MAX(bonus_b-(j-max1), 0))
				max1 = max2;

			my_defense[i%2][j] = my_defense[(i-1)%2][max1] + my_cell[j] + bonus_a*MAX(bonus_b-(j-max1), 0);

			if (my_defense[i%2][j] > max_defence) {
				max_defence = my_defense[i%2][j];

#ifdef PRINT_BACKTRACE
				max_i = i;
				max_j = j;
#endif
			}
		}
	}

#ifdef PRINT_BACKTRACE
	backtrace(max_i, max_j);
#endif
#ifdef DUMP_DP_MATRIX
	dump();
#endif

	return max_defence;
}
#else
s64 tower(void)
{
	s64 max_defence = 0;

	for (int j = 0; j < cell_num; j++) {
		my_defense[0][j] = my_cell[j];

		if (my_defense[0][j] > max_defence)
			max_defence = my_defense[0][j];
	}

	for (int i = 1; i < budget; i++) {
		s64 tmp;
		int max1, max2;

		max1 = max2 = i-1;
		for (int j = i; j < cell_num; j++) {
			/* check (i-1,j-1) */

			/*
			 * max1: index of max defence
			 * max2: index of max defence without bonus
			 */
			if (my_defense[(i-1)%2][j-1] > my_defense[(i-1)%2][max2])
				max2 = j-1;

			tmp = my_defense[(i-1)%2][j-1] + bonus_a*MAX(bonus_b-1, 0);
			if (tmp > my_defense[(i-1)%2][max1] + bonus_a*MAX(bonus_b-(j-max1), 0))
				max1 = j-1;

			tmp = my_defense[(i-1)%2][max2];
			if (tmp > my_defense[(i-1)%2][max1] + bonus_a*MAX(bonus_b-(j-max1), 0))
				max1 = max2;

			my_defense[i%2][j] = my_defense[(i-1)%2][max1] + my_cell[j] + bonus_a*MAX(bonus_b-(j-max1), 0);

			if (my_defense[i%2][j] > max_defence)
				max_defence = my_defense[i%2][j];
		}
	}

	return max_defence;
}
#endif

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *token;

	s64 max_defence;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d %d %lld %lld", &cell_num, &budget, &bonus_a, &bonus_b);

	/* parse second line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get second line\n");
		goto exit;
	}

#ifdef DEBUG
	printf("cell:\n");
#endif
	/* O(n) */
	for (int i = 0; i < cell_num; i++) {
		/* get suitability number */
		token = strtok(i == 0 ? line : NULL, " ");
		if (token == NULL) {
			/* should not happen */
			printf("Fail to get token for event type\n");
			continue;
		}
		my_cell[i] = atoi(token);

#ifdef DEBUG
		printf("  %d: %d\n", i, my_cell[i]);
#endif
	}

	max_defence = tower();

	printf("%lld\n", max_defence);

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
