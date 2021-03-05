#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LINE_SIZE (64)
#define N (200000)
#define Q (200000)


struct queue {
	int type;
	u64 num;
};

static char line[LINE_SIZE];

struct queue my_queue[2*Q];
static int my_head, my_tail;

static int type_num, group_num;

static u64 my_count[N];
static int my_height;

void queue_add(int dir, int type, u64 num)
{
	int idx;

	if (my_head == -1) {
		idx = my_head = my_tail = Q;
	} else {
		switch (dir) {
		case 0:
			my_head--;
			idx = my_head;
			break;
		case 1:
			my_tail++;
			idx = my_tail;
			break;
		};
	}

	my_queue[idx].type = type;
	my_queue[idx].num = num;

	my_count[type] += ((u64)num * (u64)my_height);

#ifdef DEBUG
	printf("add type %d %llu\n", type, ((u64)num * (u64)my_height));
#endif
}

void queue_remove(int dir, u64 num)
{
	while (num) {
		switch (dir) {
		case 0:
			if (my_queue[my_head].num <= num) {
				/* remove entire item */
				num -= my_queue[my_head].num;
				my_count[my_queue[my_head].type] -= ((u64)my_queue[my_head].num * (u64)my_height);
				my_head++;
			} else {
				my_queue[my_head].num -= num;
				my_count[my_queue[my_head].type] -= ((u64)num * (u64)my_height);
				num = 0;
			}
			break;
		case 1:
			if (my_queue[my_tail].num <= num) {
				/* remove entire item */
				num -= my_queue[my_tail].num;
				my_count[my_queue[my_tail].type] -= ((u64)my_queue[my_tail].num * (u64)my_height);
				my_tail--;
			} else {
				my_queue[my_tail].num -= num;
				my_count[my_queue[my_tail].type] -= ((u64)num * (u64)my_height);
				num = 0;
			}
			break;
		};
	}

	if (my_head > my_tail)
		my_head = my_tail = -1;
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *token;

	char where[8];
	int dir, num, type;
	u64 persuade;


	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d %d", &type_num, &group_num);

	for (int i = 0; i < type_num; i++)
		my_count[i] = 0;

	my_height = group_num;

	my_head = my_tail = -1;

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		sscanf(line, "%s %d %d %llu", where, &num, &type, &persuade);

		if (!strncmp(where, "front", sizeof(where)))
			dir = 0;
		else
			dir = 1;

		if (persuade != 0)
			queue_remove(dir, persuade);

		queue_add(dir, type-1, num+persuade);

		my_height--;

#ifdef DEBUG
		printf("dir %d, num %d, type %d, persuade %llu\n", dir, num, type, persuade);
#endif

	}

	for (int i = 0; i < type_num; i++) {
		if (i == 0)
			printf("%llu", my_count[i]);
		else
			printf(" %llu", my_count[i]);
	}
	printf("\n");

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
