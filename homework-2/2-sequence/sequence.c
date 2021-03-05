#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN2(a, b) (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c) (((a) > (b)) ? ((((b) > (c)) ? (c) : (b))) : ((((a) > (c)) ? (c) : (a))))

#define INVALID (0xFFFFFFFFFFFF)

#define LINE_SIZE (200 * 16)
#define N 200

static char line[LINE_SIZE];

static int seq_num, dollar, diff;

static int my_aseq[N];
static int my_cseq[N];
static int my_sseq[N];

/* for dp */
static u64 my_cost[3*N+1][N][N];
static int max_score;

static int my_max_length;
static int my_max_seq[N];

static int my_max_operation;

void backtrace(int s, int i, int j, u64 cost)
{
	if (i == j) {
		/* middle */
		my_max_length++;
		my_max_seq[i] = 1;
#ifdef DEBUG
		printf("  select %d\n", i);
#endif

		printf("%d\n", my_max_operation);
		return;
	}

	if (MAX(my_aseq[i] - my_aseq[j], my_aseq[j] - my_aseq[i]) <= diff) {
		if (j - i == 1) {
			if (s > my_sseq[i] + my_sseq[j]) {
				printf("ERROR\n");
			} else if (s == my_sseq[i] + my_sseq[j]) {
				my_max_length += 2;
				my_max_seq[i] = my_max_seq[j] = 1;
#ifdef DEBUG
				printf("  select %d %d\n", i, j);
#endif

				printf("%d\n", my_max_operation);
			} else {
				if (my_cost[s][i+1][j] > my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else
					backtrace(s, i+1, j, cost);
			}
		} else {
			if (s > my_sseq[i] + my_sseq[j]) {
				u64 min = MIN3(my_cost[s][i+1][j], my_cost[s][i][j-1], my_cost[s-my_sseq[i]-my_sseq[j]][i+1][j-1]);

				if (min == my_cost[s][i+1][j])
					backtrace(s, i+1, j, cost);
				else if (min == my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else {
					my_max_length += 2;
					my_max_seq[i] = my_max_seq[j] = 1;
#ifdef DEBUG
					printf("  select %d %d\n", i, j);
#endif

					backtrace(s-my_sseq[i]-my_sseq[j], i+1, j-1, cost);
				}
			} else if (s == my_sseq[i] + my_sseq[j]) {
				my_max_length += 2;
				my_max_seq[i] = my_max_seq[j] = 1;
#ifdef DEBUG
				printf("  select %d %d\n", i, j);
#endif

				printf("%d\n", my_max_operation);
			} else {
				if (my_cost[s][i+1][j] > my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else
					backtrace(s, i+1, j, cost);
			}
		}
	} else {
		if (j - i == 1) {
			if (s > my_sseq[i] + my_sseq[j]) {
				printf("ERROR\n");
			} else if (s == my_sseq[i] + my_sseq[j]) {
				my_max_length += 2;
				my_max_seq[i] = my_max_seq[j] = 1;
#ifdef DEBUG
				printf("  select %d %d\n", i, j);
#endif

				my_max_operation++;

				printf("%d\n", my_max_operation);

				if (my_cseq[i] < my_cseq[j])
					printf("%d %d\n", i+1, my_aseq[j]);
				else
					printf("%d %d\n", j+1, my_aseq[i]);
			} else {
				if (my_cost[s][i+1][j] > my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else
					backtrace(s, i+1, j, cost);
			}
		} else {
			if (s > my_sseq[i] + my_sseq[j]) {
				u64 min = MIN3(my_cost[s][i+1][j], my_cost[s][i][j-1], my_cost[s-my_sseq[i]-my_sseq[j]][i+1][j-1]+MIN2(my_cseq[i], my_cseq[j]));

				if (min == my_cost[s][i+1][j])
					backtrace(s, i+1, j, cost);
				else if (min == my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else {
					my_max_length += 2;
					my_max_seq[i] = my_max_seq[j] = 1;
#ifdef DEBUG
					printf("  select %d %d\n", i, j);
#endif

					my_max_operation++;

					backtrace(s-my_sseq[i]-my_sseq[j], i+1, j-1, cost-MIN2(my_cseq[i], my_cseq[j]));

					if (my_cseq[i] < my_cseq[j])
						printf("%d %d\n", i+1, my_aseq[j]);
					else
						printf("%d %d\n", j+1, my_aseq[i]);
				}
			} else if (s == my_sseq[i] + my_sseq[j]) {
				u64 min = MIN3(my_cost[s][i+1][j], my_cost[s][i][j-1], MIN2(my_cseq[i], my_cseq[j]));

				if (min == my_cost[s][i+1][j])
					backtrace(s, i+1, j, cost);
				else if (min == my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else {
					my_max_length += 2;
					my_max_seq[i] = my_max_seq[j] = 1;
#ifdef DEBUG
					printf("  select %d %d\n", i, j);
#endif

					my_max_operation++;

					printf("%d\n", my_max_operation);

					if (my_cseq[i] < my_cseq[j])
						printf("%d %d\n", i+1, my_aseq[j]);
					else
						printf("%d %d\n", j+1, my_aseq[i]);
				}
			} else {
				if (my_cost[s][i+1][j] > my_cost[s][i][j-1])
					backtrace(s, i, j-1, cost);
				else
					backtrace(s, i+1, j, cost);
			}
		}
	}
}

void sequence(void)
{
	int score = 0;
	int first;

	for (int s = 0; s <= max_score; s++) {
		for (int i = 0; i < seq_num; i++) {
			if (my_sseq[i] == s)
				my_cost[s][i][i] = 0;
			else
				my_cost[s][i][i] = INVALID;
		}

		for (int j = 1; j < seq_num; j++) {
			for (int i = j-1; i >= 0; i--) {
				if (MAX(my_aseq[i] - my_aseq[j], my_aseq[j] - my_aseq[i]) <= diff) {
					if (j - i == 1) {
						if (s > my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = INVALID;
						else if (s == my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = 0;
						else
							my_cost[s][i][j] = MIN2(my_cost[s][i+1][j], my_cost[s][i][j-1]);
					} else {
						if (s > my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = MIN3(my_cost[s][i+1][j], my_cost[s][i][j-1], my_cost[s-my_sseq[i]-my_sseq[j]][i+1][j-1]);
						else if (s == my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = 0;
						else
							my_cost[s][i][j] = MIN2(my_cost[s][i+1][j], my_cost[s][i][j-1]);
					}
				} else {
					if (j - i == 1) {
						if (s > my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = INVALID;
						else if (s == my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = MIN2(my_cseq[i], my_cseq[j]);
						else
							my_cost[s][i][j] = MIN2(my_cost[s][i+1][j], my_cost[s][i][j-1]);
					} else {
						if (s > my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = MIN3(my_cost[s][i+1][j], my_cost[s][i][j-1], my_cost[s-my_sseq[i]-my_sseq[j]][i+1][j-1]+MIN2(my_cseq[i], my_cseq[j]));
						else if (s == my_sseq[i] + my_sseq[j])
							my_cost[s][i][j] = MIN3(my_cost[s][i+1][j], my_cost[s][i][j-1], MIN2(my_cseq[i], my_cseq[j]));
						else
							my_cost[s][i][j] = MIN2(my_cost[s][i+1][j], my_cost[s][i][j-1]);
					}
				}
			}
		}

		if (s > score && my_cost[s][0][seq_num-1] <= dollar)
			score = s;

#ifdef DEBUG
		printf("dp: s=%d\n", s);

		for (int i = 0; i < seq_num; i++) {
			for (int j = 0; j < seq_num; j++)
				printf("%5lld ", my_cost[s][i][j] == INVALID ? -1 : my_cost[s][i][j]);

			printf("\n");
		}
		printf("\n");
#endif
	}

	my_max_length = 0;
	for (int i = 0; i < seq_num; i++) {
		my_max_seq[i] = 0;
	}

	my_max_operation = 0;

	printf("%d\n", score);
	backtrace(score, 0, seq_num-1, my_cost[score][0][seq_num-1]);
	printf("%d\n", my_max_length);

	/* print the path */
	first = 0;
	for (int i = 0; i < seq_num; i++) {
		if (my_max_seq[i] == 0)
			continue;

		if (first == 0) {
			printf("%d", i+1);
			first++;
		} else
			printf(" %d", i+1);
	}
	printf("\n");
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

	sscanf(line, "%d %d %d", &seq_num, &dollar, &diff);

	/* parse second line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get second line\n");
		goto exit;
	}

#ifdef DEBUG
	printf("aseq:\n");
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
		my_aseq[i] = atoi(token);
#ifdef DEBUG
		printf("  %d: %d\n", i, my_aseq[i]);
#endif
	}

	/* parse third line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get third line\n");
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
			printf("Fail to get token for cseq\n");
			continue;
		}
		my_cseq[i] = atoi(token);

#ifdef DEBUG
		printf("  %d: %d\n", i, my_cseq[i]);
#endif
	}

	/* parse forth line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get forth line\n");
		goto exit;
	}

#ifdef DEBUG
	printf("sseq:\n");
#endif
	/* O(n) */
	for (int i = 0; i < seq_num; i++) {
		/* get suitability number */
		token = strtok(i == 0 ? line : NULL, " ");
		if (token == NULL) {
			/* should not happen */
			printf("Fail to get token for sseq\n");
			continue;
		}
		my_sseq[i] = atoi(token);
		max_score += my_sseq[i];

#ifdef DEBUG
		printf("  %d: %d\n", i, my_sseq[i]);
#endif
	}

	sequence();

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
