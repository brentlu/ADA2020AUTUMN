#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define LINE_SIZE (100000 * 8)
#define N 100000

static char line[LINE_SIZE];

static int seq_num, flag;
static int my_cseq[N+1];

static int my_result[2][N+1];
static u64 addition[2];

static void backtrace(int first, int min_r)
{
	int pos = first + min_r;
	int first_book = 1;

	while (1) {
		if (my_result[min_r][pos] == 0)
			break;

		if (first_book != 0) {
			printf("%d", pos+1);
			first_book = 0;
		} else
			printf(" %d", pos+1);

		my_result[min_r][pos]--;

		if (pos == first)
			pos++;
		else {
			if (my_result[min_r][pos-1] != 0)
				pos--;
			else
				pos++;
		}
	}

	printf("\n");
}

static void boook(void)
{
	int first = seq_num, last = 0, prev_cseq;
	int first_book = 1;

	int min_r;

	for (int i = 0; i < seq_num; i++) {
		if ((first == seq_num) && (my_cseq[i] != 0)) {
			first = i;
			break;
		}
	}

	for (int i = seq_num - 1; i >= 0 ; i--) {
		if ((last == 0) && (my_cseq[i] != 0)) {
			last = i;
			break;
		}
	}

	if (first == last)
		last++;

	for (int r = 0; r < 2; r++) {
		addition[r] = 0;

		for (int i = 0; i < seq_num; i++) {
			my_result[r][i] = 0;
		}

		prev_cseq = my_cseq[first];

		for (int i = first+1; i <= last; i++) {
			if (i != last) {
				if (prev_cseq != 0) {
					if (i == first+1) {
						if (r == 1) {
							/* start from 2nd */
							my_result[r][i] += (prev_cseq+1);
							my_result[r][i-1] += prev_cseq;

							prev_cseq = my_cseq[i] - (prev_cseq+1);
						} else {
							my_result[r][i] += prev_cseq;
							my_result[r][i-1] += prev_cseq;

							prev_cseq = my_cseq[i] - prev_cseq;
						}
					} else {
						my_result[r][i] += (prev_cseq+1);
						my_result[r][i-1] += prev_cseq;

						prev_cseq = my_cseq[i] - (prev_cseq+1);
					}
				} else {
					my_result[r][i] += 1;

					prev_cseq = my_cseq[i] - 1;
				}

				if (prev_cseq < 0) {
					addition[r] -= prev_cseq;
					prev_cseq = 0;
				}
			} else {
				if (my_cseq[i] > prev_cseq) {
					my_result[r][i] += my_cseq[i];
					my_result[r][i-1] += my_cseq[i]-1;
					addition[r] += (my_cseq[i] - prev_cseq - 1);
				} else if (my_cseq[i] == prev_cseq) {
					my_result[r][i] += prev_cseq;
					my_result[r][i-1] += prev_cseq;
				} else {
					if (i-1 != first) {
						my_result[r][i] += prev_cseq;
						my_result[r][i-1] += prev_cseq;
						addition[r] += (prev_cseq - my_cseq[i]);
					} else {
						my_result[r][i] += (prev_cseq-1);
						my_result[r][i-1] += prev_cseq;
						addition[r] += (prev_cseq - my_cseq[i] - 1);
					}
				}
			}
		}

#ifdef DEBUG
		first_book = 1;
		for (int i = 0; i < seq_num; i++) {
			if (first_book == 1) {
				printf("%d", my_result[r][i]);
				first_book = 0;
			} else
				printf(" %d", my_result[r][i]);
		}
		printf("\n");
#endif
	}

	if (addition[0] > addition[1])
		min_r = 1;
	else
		min_r = 0;

	printf("%llu\n", addition[min_r]);

	if (flag != 0) {
		backtrace(first, min_r);
	}

}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *token;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d", &seq_num);

	/* parse second line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get second line\n");
		goto exit;
	}

#ifdef DEBUG
	printf("cseq:\n");
#endif
	/* O(n) */
	for (int i = 0; i < seq_num; i++) {
		/* get suitability number */
		token = strtok(i == 0 ? line : NULL, " ");
		if (token == NULL) {
			/* should not happen */
			printf("Fail to get token for aseq\n");
			continue;
		}
		my_cseq[i] = atoi(token);
#ifdef DEBUG
		printf("  %d: %d\n", i, my_cseq[i]);
#endif
	}

	/* parse third line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d", &flag);

	boook();

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
