#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64; 

#define LINE_SIZE (500000 * 16)
#define N 500000
#define K 500000

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static int my_candy[N];
static u64 my_prefix[N];

static int bag_num;
static int classmate_num;

static int min_note[N];
static int max_note[N];

static int remainder[K+1];

static u64 party(int l_bag, int r_bag)
{
	int k_bag;
	u64 found = 0;
	int min, max;
	int i, j, j_min, j_max, i_min, i_max;

	int tmp;

	/* base case */
	if (l_bag == r_bag)
		return 0;

	/* recursive case */
	k_bag = (l_bag + r_bag) / 2;

	found = party(l_bag, k_bag);
	found += party(k_bag + 1, r_bag);

	min = max = my_candy[k_bag];
	max_note[k_bag] = max;
	min_note[k_bag] = min;

	for (int i = k_bag - 1; i >= l_bag; i--) {
		if (my_candy[i] > max)
			max = my_candy[i];
		else if (my_candy[i] < min)
			min = my_candy[i];

		max_note[i] = max;
		min_note[i] = min;
	}

	min = max = my_candy[k_bag + 1];
	max_note[k_bag + 1] = max;
	min_note[k_bag + 1] = min;

	for (i = k_bag + 2; i <= r_bag; i++) {
		if (my_candy[i] > max)
			max = my_candy[i];
		else if (my_candy[i] < min)
			min = my_candy[i];

		max_note[i] = max;
		min_note[i] = min;
	}

#ifdef DEBUG
	printf("%s: bag (%d, %d)-(%d, %d)\n", __func__, l_bag, k_bag, k_bag + 1, r_bag);
#endif

	/* both min/max on left */
	//printf("  both on left:\n");
#if 0
	for (i = k_bag; i >= l_bag; i--) {
		for (j = k_bag+1; j <= r_bag; j++) {
			if (min_note[i] < min_note[j] && max_note[i] > max_note[j]) {
				if ((my_prefix[j]-my_prefix[i-1]-min_note[i]-max_note[i])%classmate_num == 0)
					found++;
			}
		}
	}
#else
	j = k_bag;
	for (i = k_bag; i >= l_bag; i--) {
		while (j+1 <= r_bag && min_note[i] < min_note[j+1] && max_note[i] > max_note[j+1]) {
			remainder[my_prefix[j+1]%classmate_num]++;
			j++;
		}

		tmp = remainder[(my_prefix[i-1]+min_note[i]+max_note[i])%classmate_num];
		found += tmp;
		//printf("    found %d pairs, i %d\n", tmp, i);
	}

	//memset(remainder, 0, sizeof(remainder));
	j = k_bag;
	for (i = k_bag; i >= l_bag; i--) {
		while (j+1 <= r_bag && min_note[i] < min_note[j+1] && max_note[i] > max_note[j+1]) {
			remainder[my_prefix[j+1]%classmate_num]--;
			j++;
		}
	}
#endif

	/* both min/max on right */
	//printf("  both on right:\n");
#if 0
	for (i = k_bag; i >= l_bag; i--) {
		for (j = k_bag+1; j <= r_bag; j++) {
			if (min_note[i] > min_note[j] && max_note[i] < max_note[j]) {
				if ((my_prefix[j]-my_prefix[i-1]-min_note[j]-max_note[j])%classmate_num == 0)
					found++;
			}
		}
	}
#else
	i = k_bag+1;
	for (j = k_bag+1; j <= r_bag; j++) {
		while (i-1 >= l_bag && min_note[i-1] > min_note[j] && max_note[i-1] < max_note[j]) {
			remainder[(my_prefix[i-2])%classmate_num]++;
			i--;
		}

		tmp = remainder[(my_prefix[j]-min_note[j]-max_note[j])%classmate_num];
		found += tmp;
		//printf("    found %d pairs, j %d\n", tmp, j);
	}

	//memset(remainder, 0, sizeof(remainder));
	i = k_bag+1;
	for (j = k_bag+1; j <= r_bag; j++) {
		while (i-1 >= l_bag && min_note[i-1] > min_note[j] && max_note[i-1] < max_note[j]) {
			remainder[(my_prefix[i-2])%classmate_num]--;
			i--;
		}
	}
#endif

	/* min on left, max on right */
	//printf("  min/max:\n");
#if 0
	j = k_bag;
	for (i = k_bag; i >= l_bag; i--) {
		for (j = k_bag+1; j <= r_bag; j++) {
			if (min_note[i] < min_note[j] && max_note[i] < max_note[j]) {
				if ((my_prefix[j]-my_prefix[i-1]-min_note[i]-max_note[j])%classmate_num == 0)
					found++;
			}
		}
	}
#else
	j_min = k_bag;
	j_max = k_bag+1;
	for (i = k_bag; i >= l_bag; i--) {
		/* remove max on left */
		while (j_max != r_bag+1 && max_note[i] > max_note[j_max]) {
			if (j_max <= j_min)
				remainder[(my_prefix[j_max]-max_note[j_max])%classmate_num]--;
			j_max++;
		}

		/* all min on left, assume max on right */
		while (j_min <= r_bag && min_note[i] < min_note[j_min+1]) {
			j_min++;
			if (j_max <= j_min)
				remainder[(my_prefix[j_min]-max_note[j_min])%classmate_num]++;
		}

		tmp = remainder[(my_prefix[i-1]+min_note[i])%classmate_num];
		found += tmp;
		//printf("    found %d pairs, i %d\n", tmp, i);
	}

	//memset(remainder, 0, sizeof(remainder));
	j_min = k_bag;
	j_max = k_bag+1;
	for (i = k_bag; i >= l_bag; i--) {
		/* remove max on left */
		while (j_max != r_bag+1 && max_note[i] > max_note[j_max]) {
			if (j_max <= j_min)
				remainder[(my_prefix[j_max]-max_note[j_max])%classmate_num]++;
			j_max++;
		}

		/* all min on left, assume max on right */
		while (j_min < r_bag && min_note[i] < min_note[j_min+1]) {
			j_min++;
			if (j_max <= j_min)
				remainder[(my_prefix[j_min]-max_note[j_min])%classmate_num]--;
		}
	}
#endif

	/* max on left, min on right */
#ifdef DEBUG
	printf("  max/min:\n");
#endif
#if 0
	j = k_bag;
	for (i = k_bag; i >= l_bag; i--) {
		for (j = k_bag+1; j <= r_bag; j++) {
			if (min_note[i] > min_note[j] && max_note[i] > max_note[j]) {
				if ((my_prefix[j]-my_prefix[i-1]-max_note[i]-min_note[j])%classmate_num == 0)
					found++;
			}
		}
	}
#else
#if 1
	j_max = k_bag;
	j_min = k_bag+1;
	for (i = k_bag; i >= l_bag; i--) {
		/* remove min on left */
		while (j_min != r_bag+1 && min_note[i] < min_note[j_min]) {
			if (j_min <= j_max) {
				remainder[(my_prefix[j_min]-min_note[j_min])%classmate_num]--;
#ifdef DEBUG
				printf("    remainder %llu --, j_min %d\n", (my_prefix[j_min]-min_note[j_min])%classmate_num, j_min);
#endif
			}
			j_min++;
		}

		/* all max on left, assume min on right */
		while (j_max < r_bag && max_note[i] > max_note[j_max+1]) {
			j_max++;
			if (j_min <= j_max) {
				remainder[(my_prefix[j_max]-min_note[j_max])%classmate_num]++;
#ifdef DEBUG
				printf("    remainder %llu ++, j_max %d\n", (my_prefix[j_max]-min_note[j_max])%classmate_num, j_max);
#endif
			}
		}

		tmp = remainder[(my_prefix[i-1]+max_note[i])%classmate_num];
#ifdef DEBUG
		printf("    check remainder %llu\n", (my_prefix[i-1]+max_note[i])%classmate_num);
#endif
		found += tmp;
#ifdef DEBUG
		printf("    found %d pairs, i %d\n", tmp, i);
#endif
	}

	//memset(remainder, 0, sizeof(remainder));

	j_max = k_bag;
	j_min = k_bag+1;
	for (i = k_bag; i >= l_bag; i--) {
		/* remove min on left */
		while (j_min != r_bag+1 && min_note[i] < min_note[j_min]) {
			if (j_min <= j_max) {
				remainder[(my_prefix[j_min]-min_note[j_min])%classmate_num]++;
#ifdef DEBUG
				printf("    remainder %llu --, j_min %d\n", (my_prefix[j_min]-min_note[j_min])%classmate_num, j_min);
#endif
			}
			j_min++;
		}

		/* all max on left, assume min on right */
		while (j_max < r_bag && max_note[i] > max_note[j_max+1]) {
			j_max++;
			if (j_min <= j_max) {
				remainder[(my_prefix[j_max]-min_note[j_max])%classmate_num]--;
#ifdef DEBUG
				printf("    remainder %llu ++, j_max %d\n", (my_prefix[j_max]-min_note[j_max])%classmate_num, j_max);
#endif
			}
		}
	}


#else
	i_max = k_bag+1;
	i_min = k_bag;
	for (j = k_bag+1; j <= r_bag; j++) {
		while (i_min != l_bag-1 && min_note[i_min] < min_note[j]) {
			if (i_min >= i_max)
				remainder[(my_prefix[i_min-1] + max_note[i_min])%classmate_num]--;
			i_min--;
		}

		while (i_max >= l_bag && max_note[i_max-1] > max_note[j]) {
			i_max--;
			if (i_min >= i_max) {
				remainder[(my_prefix[i_max-1] + max_note[i_max])%classmate_num]++;
			}
		}

		found += remainder[(my_prefix[j]-min_note[j])%classmate_num];
	}

	memset(remainder, 0, sizeof(remainder));
#endif
#endif

	return found;
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *line, *token;

	int i;
	u64 found;

	start = clock();

	line = (char *)malloc(LINE_SIZE * sizeof(char));
	if (line == NULL) {
		printf("Fail to allocate memory\n");
		goto exit;
	}

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d %d", &bag_num, &classmate_num);

	/* parse second line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get second line\n");
		goto exit;
	}

#ifdef DEBUG
	printf("bag:\n");
#endif
	/* O(n) */
	for (i = 0; i < bag_num; i++) {
		/* get candy number */
		token = strtok(i == 0 ? line : NULL, " ");
		if (token == NULL) {
			/* should not happen */
			printf("Fail to get token for event type\n");
			continue;
		}
		my_candy[i] = atoi(token);
		my_prefix[i] = my_candy[i];
		if (i != 0)
			my_prefix[i] += my_prefix[i - 1];

#ifdef DEBUG
		printf("%d: %d\n", i, my_candy[i]);
#endif
	}

	memset(remainder, 0, sizeof(remainder));

	found = party(0, bag_num - 1);

	printf("%lld\n", found);

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
