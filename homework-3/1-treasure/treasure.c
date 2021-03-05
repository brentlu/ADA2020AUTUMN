#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LINE_SIZE (500000 * 16)
#define N (500000)
#define M (500000)

#define INVALID (0xFFFFFFFFFFFFFFFF)

struct edge {
	int to;
	int unit;

	struct edge *next;
};

static char line[LINE_SIZE];

static struct edge my_buf[2*M];

static int city_num, road_num;

static u64 my_city[N]; /* larger type for merge cities */
static struct edge *my_road[N];
static struct edge *my_road_t[N]; /* transpose */

struct dfs {
	int c; /* color 0:white, 1:gray, 2:black */
	int pi;
	int d;     /* discover time */
	int f;     /* finish time */
};

/* dfs structure */
static struct dfs my_dfs[N];
static int my_time;

/* cities in order of finish time */
static int my_order[N];
static int my_count;

static int my_group[N];

static u64 dfs_treasure[N];

void print_graph(struct edge *roads[])
{
	for (int i = 0; i < city_num; i++) {
		struct edge *road = roads[i];

		if (my_city[i] == INVALID)
			continue;

		printf("%d[%llu]:", i+1, my_city[i]);
		while (road) {
			printf(" ->%d[%d]", road->to+1, road->unit);
			road = road->next;
		}
		printf("\n");
	}
}

void merge_city(int from, int to)
{
	struct edge *road = my_road[from];
	struct edge *prev, *next;

	if (from != to) {
		my_city[to] += my_city[from];
		my_city[from] = INVALID;

		while (road) {
			next = road->next;

			if (my_group[road->to] == my_group[to]) {
				/* road inside the group */
				my_city[to] += ((u64)road->unit);
			} else {
				road->to = my_group[road->to];

				/* move to head */
				if (my_road[to] != NULL)
					road->next = my_road[to];
				else
					road->next = NULL;
				my_road[to] = road;
			}

			road = next;
		}

		my_road[from] = NULL;
	} else {
		prev = NULL;
		while (road) {
			if (my_group[road->to] == my_group[to]) {
				/* road inside the group */
				my_city[to] += ((u64)road->unit);

				if (prev == NULL)
					my_road[from] = road->next;
				else
					prev->next = road->next;
			} else {
				road->to = my_group[road->to];

				prev = road;
			}

			road = road->next;
		}
	}
}

u64 dfs_collect(int u)
{
	struct edge *road = my_road[u];
	u64 collected = 0;
	u64 max = 0;

#ifdef DEBUG
	printf("%s: u %d\n", __func__, u+1);
#endif

	my_dfs[u].d = my_time++;
	my_dfs[u].c = 1;

	while (road) {
#ifdef DEBUG
		printf("%s: edge (%d, %d)\n", __func__, u+1, road->to+1);
#endif

		if (my_dfs[road->to].c == 0) {
			my_dfs[road->to].pi = u;
			collected = ((u64)road->unit) + dfs_collect(road->to);
		} else {
			collected = ((u64)road->unit) + dfs_treasure[road->to];
		}

		if (collected > max)
			max = collected;

		road = road->next;
	}

	my_dfs[u].c = 2;
	my_dfs[u].f = my_time++;

	dfs_treasure[u] = my_city[u] + max;

#ifdef DEBUG
	printf("%s: u %d, treasure %llu\n", __func__, u+1, dfs_treasure[u]);
#endif


	return dfs_treasure[u];
}

void dfs_transpose(int u, int g)
{
	struct edge *road = my_road_t[u];

	my_dfs[u].d = my_time++;
	my_dfs[u].c = 1;

	while (road) {
		if (my_dfs[road->to].c == 0) {
			my_dfs[road->to].pi = u;
			dfs_transpose(road->to, g);
		}

		road = road->next;
	}

	my_dfs[u].c = 2;
	my_dfs[u].f = my_time++;

	/* group */
	my_group[u] = g;
}

void dfs_order(int u)
{
	struct edge *road = my_road[u];

	my_dfs[u].d = my_time++;
	my_dfs[u].c = 1;

	while (road) {
		if (my_dfs[road->to].c == 0) {
			my_dfs[road->to].pi = u;
			dfs_order(road->to);
		}

		road = road->next;
	}

	my_dfs[u].c = 2;
	my_dfs[u].f = my_time++;

	/* order */
	my_count++;
	my_order[city_num-my_count] = u;
}

u64 treasure(void)
{
	u64 max = 0;

	/* init */
	for (int i = 0; i < city_num; i++) {
		my_dfs[i].c = 0;
		my_dfs[i].pi = -1;
		my_dfs[i].d = -1;
		my_dfs[i].f = -1;
	}

	my_time = 0;
	my_count = 0;

	/* calculate the finish time */
	for (int i = 0; i < city_num; i++) {
		if (my_dfs[i].c == 0) {
			/* start from first city */
			dfs_order(i);
		}
	}

	/* init */
	for (int i = 0; i < city_num; i++) {
		my_dfs[i].c = 0;
		my_dfs[i].pi = -1;
		my_dfs[i].d = -1;
		my_dfs[i].f = -1;
	}

	my_time = 0;

	for (int i = 0; i < city_num; i++) {
		if (my_dfs[my_order[i]].c == 0) {
			/* start from city with highest finish time */
			dfs_transpose(my_order[i], my_order[i]);
		}
	}

#ifdef DEBUG
	printf("group:\n");
	for (int i = 0; i < city_num; i++)
		printf("%d: %d\n", i+1, my_group[i]+1);
#endif

	/* merge cities in same loop */
	for (int i = 0; i < city_num; i++) {
		merge_city(i, my_group[i]);
	}

#ifdef DEBUG
	printf("merged:\n");
	print_graph(my_road);
#endif

	/* init */
	for (int i = 0; i < city_num; i++) {
		if (my_city[i] == INVALID)
			my_dfs[i].c = 2; /* black */
		else
			my_dfs[i].c = 0;
		my_dfs[i].pi = -1;
		my_dfs[i].d = -1;
		my_dfs[i].f = -1;

		dfs_treasure[i] = 0;
	}

	for (int i = 0; i < city_num; i++) {
		if (my_dfs[i].c == 0) {
			dfs_collect(i);
		}
	}

	for (int i = 0; i < city_num; i++) {
		if (dfs_treasure[i] > max) {
			max = dfs_treasure[i];
		}
	}

	return max;
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *token;

	int count;
	int from, to, unit;

	u64 collected;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d %d", &city_num, &road_num);

	/* parse second line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get second line\n");
		goto exit;
	}

	/* O(n) */
	for (int i = 0; i < city_num; i++) {
		token = strtok(i == 0 ? line : NULL, " ");
		if (token == NULL) {
			/* should not happen */
			printf("Fail to get token for event type\n");
			continue;
		}
		my_city[i] = atoi(token);
	}

	for (int i = 0; i < city_num; i++) {
		my_road[i] = NULL;
		my_road_t[i] = NULL;
	}

	count = 0;

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		sscanf(line, "%d %d %d", &from, &to, &unit);

		/* add to G */
		my_buf[count].to = to-1;
		my_buf[count].unit = unit;

		if (my_road[from-1] != NULL)
			my_buf[count].next = my_road[from-1];
		else
			my_buf[count].next = NULL;
		my_road[from-1] = &my_buf[count];

		count++;

		/* add to G-Transpose */
		my_buf[count].to = from-1;
		my_buf[count].unit = unit;

		if (my_road_t[to-1] != NULL)
			my_buf[count].next = my_road_t[to-1];
		else
			my_buf[count].next = NULL;
		my_road_t[to-1] = &my_buf[count];

		count++;
	}

#ifdef DEBUG
	printf("init:\n");
	print_graph(my_road);
#endif

	collected = treasure();

	printf("%llu\n", collected);

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
