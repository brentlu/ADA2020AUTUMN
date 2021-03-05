#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64;

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LINE_SIZE (32)
#define N (500000)

#define INVALID (0xFFFFFFFFFFFFFFFF)

struct road {
	int to;
	u64 value;

	u64 subtree;

	struct road *next;
};

/* buffers */
static char line[LINE_SIZE];
static struct road my_buf[3*N];
static int my_buf_count;

static int city_num;

static struct road *my_roads[N];

struct dfs {
	int c;  /* color 0:white, 1:gray, 2:black */
	int pi;
	int d;  /* discover time */
	int f;  /* finish time */
};

static int max_u;
static u64 max_value;

/* dfs structure */
static struct dfs my_dfs[N];
static int my_time;

static struct road *my_tree[N];

void print_graph(struct road *roads[])
{
	for (int i = 0; i < city_num; i++) {
		struct road *r = roads[i];

		printf("%d:", i+1);
		while (r) {
			printf(" ->%d[%llu, %llu]", r->to+1, r->value, r->subtree);
			r = r->next;
		}
		printf("\n");
	}
}

struct road *sorted_merge(struct road *a, struct road *b)
{
	struct road *result = NULL;

	if (a == NULL)
		return b;
	else if (b == NULL)
		return a;

	if (a->value + a->subtree > b->value + b->subtree) {
		result = a;
		result->next = sorted_merge(a->next, b);
	} else {
		result = b;
		result->next = sorted_merge(a, b->next);
	}

	return result;
}

void front_back_split(struct road *source, struct road **front, struct road **back)
{
	struct road *fast, *slow;

	slow = source;
	fast = source->next;

	while (fast != NULL) {
		fast = fast->next;
		if (fast != NULL) {
			slow = slow->next;
			fast = fast->next;
		}
	}

	*front = source;
	*back = slow->next;
	slow->next = NULL;
}

void merge_sort(struct road ** roads)
{
	struct road *head = *roads, *a, *b;

	if ((head == NULL) || (head->next == NULL))
		return;

	front_back_split(head, &a, &b);

	merge_sort(&a);
	merge_sort(&b);

	*roads = sorted_merge(a, b);
}

void merge(int u, int v)
{
	struct road *a = my_tree[u], *b = my_tree[v], *c = NULL;

	while (a != NULL && b != NULL) {
		if (a->value + a->subtree > b->value + b->subtree) {
			if (c == NULL)
				my_tree[u] = a;
			else
				c->next = a;
			c = a;
			a = a->next;
		} else {
			if (c == NULL)
				my_tree[u] = b;
			else
				c->next = b;
			c = b;
			b = b->next;
		}
	}

	if (a != NULL) {
		if (c == NULL)
			my_tree[u] = a;
		else
			c->next = a;
	} else if (b != NULL) {
		if (c == NULL)
			my_tree[u] = b;
		else
			c->next = b;
	}
}

void invalidate(int u)
{
	struct road *r = my_tree[u], *s;

	if (r != NULL) {
		/* first */
		invalidate(r->to);

		/* remove */
		my_tree[u] = r->next;

		if (u != max_u)
			merge(max_u, u);
	}
}

u64 prepare_tree(int u)
{
	struct road *r = my_roads[u];
	u64 subtree, max_value = 0;

	my_dfs[u].d = my_time++;
	my_dfs[u].c = 1;

	while (r) {
		if (my_dfs[r->to].c == 0) {
			my_dfs[r->to].pi = u;
			subtree = prepare_tree(r->to);
			if (subtree + r->value > max_value)
				max_value = subtree + r->value;

			/* add to tree */
			my_buf[my_buf_count].to = r->to;
			my_buf[my_buf_count].value = r->value;
			my_buf[my_buf_count].subtree = subtree;

			my_buf[my_buf_count].next = my_tree[u];
			my_tree[u] = &my_buf[my_buf_count];

			my_buf_count++;
		}

		r = r->next;
	}

	my_dfs[u].c = 2;
	my_dfs[u].f = my_time++;

	merge_sort(&my_tree[u]);

	return max_value;
}

void find_root(int u, u64 value)
{
	struct road *r = my_roads[u];

	my_dfs[u].d = my_time++;
	my_dfs[u].c = 1;

	while (r) {
		if (my_dfs[r->to].c == 0) {
			my_dfs[r->to].pi = u;
			find_root(r->to, value + r->value);
		}

		r = r->next;
	}

	my_dfs[u].c = 2;
	my_dfs[u].f = my_time++;

	if (value > max_value) {
		max_u = u;
		max_value = value;
	}
}

static void kingdom(void)
{
	u64 imp = 0;
	int count = 0;

	/* init */
	for (int i = 0; i < city_num; i++) {
		my_dfs[i].c = 0;
		my_dfs[i].pi = -1;
		my_dfs[i].d = -1;
		my_dfs[i].f = -1;
	}

	my_time = 0;
	max_value = 0;

	/* could start from any city, we use 0 here */
	find_root(0, 0);

#ifdef DEBUG
	/* root is found */
	printf("max u %d, value %llu\n", max_u+1, max_value);
#endif

	/* init */
	for (int i = 0; i < city_num; i++) {
		my_dfs[i].c = 0;
		my_dfs[i].pi = -1;
		my_dfs[i].d = -1;
		my_dfs[i].f = -1;
	}

	my_time = 0;

	for (int i = 0; i < city_num; i++) {
		my_tree[i] = NULL;
	}

	prepare_tree(max_u);

#ifdef DEBUG
	printf("tree:\n");
	print_graph(my_tree);
#endif

	printf("0");
	count++;

	while (my_tree[max_u] != NULL) {
		imp += (my_tree[max_u]->value +  my_tree[max_u]->subtree);
		printf(" %llu", imp);

		invalidate(max_u);

		merge_sort(&my_tree[max_u]);

		count++;

#ifdef DEBUG
		printf("\ntree:\n");
		print_graph(my_tree);
//		if (count == 3)
//			break;
#endif
	}

	for (int i = count; i < city_num; i++)
		printf(" %llu", imp);

	printf("\n");
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *token;

	int count;
	int from, to, value;

	u64 collected;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d", &city_num);

	for (int i = 0; i < city_num; i++) {
		my_roads[i] = NULL;
	}

	count = 0;

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		sscanf(line, "%d %d %d", &from, &to, &value);

		/* add to G */
		my_buf[count].to = to-1;
		my_buf[count].value = value;

		if (my_roads[from-1] != NULL)
			my_buf[count].next = my_roads[from-1];
		else
			my_buf[count].next = NULL;
		my_roads[from-1] = &my_buf[count];

		count++;

		/* bi-directional */
		my_buf[count].to = from-1;
		my_buf[count].value = value;

		if (my_roads[to-1] != NULL)
			my_buf[count].next = my_roads[to-1];
		else
			my_buf[count].next = NULL;
		my_roads[to-1] = &my_buf[count];

		count++;
	}

	my_buf_count = count;

#ifdef DEBUG
	printf("init:\n");
	print_graph(my_roads);
#endif

	kingdom();

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
