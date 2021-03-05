#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64; 

#define MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define MAX3(a, b, c) (((a) > (b)) ? ((((a) > (c)) ? (a) : (c))) : ((((b) > (c)) ? (b) : (c))))

#define LINE_SIZE (16 * 512)
#define L 11
#define M 500
#define N 500

s64 board[L][M][N];

int row, column;
int max_row[L];
int max_column[L];

int max_jump = 0;
int stop_farming = 0;

void print_path(int level, int r, int c, int moves) {
	s64 max_prev, prev, up, left;
	int r_prev = r, c_prev = c;

#ifdef DEBUG
	printf("%s: level %d, r %d, c %d\n", __func__, level, r, c);
#endif

	if ((r == 0) && (c == 0)) {
		if (level == 0) /* reach the end */
			printf("%d\n", moves);
		else {
			/* jump from previous level */
			print_path(level-1, max_row[level-1], max_column[level-1], moves+1);
#ifdef DEBUG
			printf("Jump %d %d (to level %d)\n", r, c, level);
#else
			printf("Jump %d %d\n", r, c);
#endif
		}
	} else {
		if (r == 0) /* comes from up */
			c_prev -= 1;
		else if (c == 0) /* comes from left */
			r_prev -= 1;
		else {
			up = board[level][r-1][c];
			left = board[level][r][c-1];

			if (up > left) /* comes from left */
				r_prev -= 1;
			else /* comes from up */
				c_prev -= 1;
		}

		if (level == 0) {
			/* we have no chice */
			print_path(level, r_prev, c_prev, moves+1);
			printf("Move %d %d\n", r, c);
		} else {
			max_prev = board[level-1][max_row[level-1]][max_column[level-1]];

			/* compare with prev in same level */
			if (board[level][r_prev][c_prev] < max_prev) {
				print_path(level-1, max_row[level-1], max_column[level-1], moves+1);
#ifdef DEBUG
				printf("Jump %d %d (to level %d)\n", r, c, level);
#else
				printf("Jump %d %d\n", r, c);
#endif
			} else {
				print_path(level, r_prev, c_prev, moves+1);
				printf("Move %d %d\n", r, c);
			}
		}
	}
}

void print_report(int jump) {
	s64 max_harvest;

	if (stop_farming == 0)
		max_jump = jump;

	max_harvest = board[max_jump][row-1][column-1];

	printf("%lld\n", max_harvest);

	print_path(max_jump, row-1, column-1, 0);
}

s64 farming(int level)
{
	s64 max;
	s64 max_prev, up, left;

	if (level != 0)
		max_prev = farming(level - 1);

	if (stop_farming)
		return 0;

#ifdef DEBUG
	printf("\nlevel %d:\n", level);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			printf("%4lld ", board[level][i][j]);
		}
		printf("\n");
	}
#endif

	if (level != 0)
		board[level][0][0] += max_prev;

	max = board[level][0][0];

	/* first column */
	for (int i = 1; i < row; i++) {
		up = board[level][i-1][0];

		if (level != 0)
			board[level][i][0] += MAX2(up, max_prev);
		else
			board[level][i][0] += up;

		if (board[level][i][0] > max) {
			max = board[level][i][0];
			max_row[level] = i;
			max_column[level] = 0;
		}
	}

	/* first row */
	for (int i = 1; i < column; i++) {
		left = board[level][0][i-1];

		if (level != 0)
			board[level][0][i] += MAX2(left, max_prev);
		else
			board[level][0][i] += left;

		if (board[level][0][i] > max) {
			max = board[level][0][i];
			max_row[level] = 0;
			max_column[level] = i;
		}
	}

	for (int i = 1; i < row; i++) {
		for (int j = 1; j < column; j++) {
			left = board[level][i][j-1];
			up = board[level][i-1][j];

			if (level != 0)
				board[level][i][j] += MAX3(left, up, max_prev);
			else
				board[level][i][j] += MAX2(left, up);

			if (board[level][i][j] > max) {
				max = board[level][i][j];
				max_row[level] = i;
				max_column[level] = j;
			}
		}
	}

	if (level != 0) {
		if (board[level][row-1][column-1] < board[level-1][row-1][column-1]) {
			stop_farming = 1;
			max_jump = level-1;
		}
	}

#ifdef DEBUG
	printf("\nlevel %d: max %lld at (%d %d)\n", level, max, max_row[level], max_column[level]);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			if ((i == max_row[level]) && (j == max_column[level]))
				printf("%3lld* ", board[level][i][j]);
			else
				printf("%4lld ", board[level][i][j]);
		}
		printf("\n");
	}
#endif

	return max;
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char line[LINE_SIZE];

	char *token;
	int jump, penalty;

	int idx = 0;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d %d %d %d", &row, &column, &jump, &penalty);

	//printf("Input: row %d, column %d, jump %d, penalty %d\n", row, column, jump, penalty);

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {

		for (int i = 0; i < column; i++) {
			token = strtok(i == 0 ? line: NULL, " ");
			if (token == NULL) {
				/* should not happen */
				printf("Fail to get token\n");
				continue;
			}

			board[0][idx][i] = atoi(token);
		}

		idx++;
	}

#ifdef DEBUG
	if (row != idx) {
		printf("Data corrpution, n %d, row_idx %d\n", row, idx);
	}
#endif

	for (int i = 1; i <= jump; i++) {
		for (int j = 0; j < row; j++) {
			for (int k = 0; k < column; k++) {
				board[i][j][k] = board[i-1][j][k] - penalty;
			}
		}
	}

	farming(jump);

	print_report(jump);

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
