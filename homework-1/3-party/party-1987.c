#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef signed long long s64;
typedef unsigned long long u64; 

#define LINE_SIZE (500000 * 16)
#define N 500000

static char line[LINE_SIZE];

static int my_candy[N];
static u64 my_prefix[N];

int main(int argc, char **argv)
{
	char *token;

	int bag_num, classmate_num;

	int min, max;
	u64 sum, found = 0;

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

	for (int i = 0; i < bag_num; i++) {
		/* get candy number */
		token = strtok(i == 0 ? line : NULL, " ");
		if (token == NULL) {
			/* should not happen */
			printf("Fail to get token for event type\n");
			continue;
		}

		my_candy[i] = my_prefix[i] = atoi(token);
		if (i == 0)
			continue;

		my_prefix[i] += my_prefix[i-1];

		//printf("i %d: candy %d, prefix %llu\n", i, my_candy[i], my_prefix[i]);

		min = max = my_candy[i];

		for (int j = i - 1; j >= 0; j--) {
			if (my_candy[j] < min)
				min = my_candy[j];
			else if (my_candy[j] > max)
				max = my_candy[j];

			sum = my_prefix[i];
			if (j > 0)
				sum -= my_prefix[j-1];

			sum -= (min + max);

			//printf("(%d, %d) sum %llu\n", j, i, sum);

			if (sum % classmate_num == 0) {
				//printf("found (%d, %d)\n", j, i);
				found++;
			}
		}
	}

	printf("%lld\n", found);

exit:
	return 0;
}
