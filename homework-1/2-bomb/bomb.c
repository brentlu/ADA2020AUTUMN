#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <cassert>

namespace Memory {
    constexpr int N = 200000; // Number of cells, change this if you need more cells

    long long arr[N + 1]; // Note: This is NOT the memory itself, please don't try to access
                          //       this array directly unless you know what you're doing

    /* Start of internal functions */
    void _ins(int x, int k) {
        while (x <= N) arr[x] += k, x += x & -x;
    }

    long long _qr(int x) {
        long long ret = 0;
        while (x) ret += arr[x], x -= x & -x;
        return ret;
    }
    /* End of internal functions */

    // Explicitly fill the whole memory with zero
    // Complexity: O(N)
    void reset() {
        for (int i = N; i >= 0; --i)
            arr[i] = 0;
    }

    // Add k to every cell in range [l, r]
    // Complexity: O(log(N))
    void add(int l, int r, int k) {
        assert(1 <= l && l <= r && r <= N && "add: the argument should satisfy 1 <= l <= r <= N");
        _ins(l, k);
        _ins(r + 1, -k);
    }

    // Get the value in cell x
    // Complexity: O(log(N))
    long long get(int x) {
        assert(1 <= x && x <= N && "get: x should be a positive integer in range [1, N]");
        return _qr(x);
    }
}

//#define DEBUG

typedef signed long long s64;
typedef unsigned long long u64; 

#define LINE_SIZE 64
#define EVENT_MAX (100000)

struct event {
	char type;	/* 'P': player, 'A': bomb */

	int left;	/* location */
	int right;	/* index */
	int power;	/* defence */
	int damage;
};

struct event my_events[EVENT_MAX];
int event_num = 0;

struct event *my_tmp1[EVENT_MAX];
struct event *my_tmp2[EVENT_MAX];

u64 player[EVENT_MAX];
int player_num = 0;

void new_bomb(int left, int right, int power, int damage)
{
	my_events[event_num].type = 'A';
	my_events[event_num].left = left;
	my_events[event_num].right = right;
	my_events[event_num].power = power;
	my_events[event_num].damage = damage;

	event_num++;
}

void new_player(int location, int defence)
{
	my_events[event_num].type = 'P';
	my_events[event_num].left = location;
	my_events[event_num].power = defence;
	my_events[event_num].right = player_num;
	event_num++;

	player[player_num] = 0;
	player_num++;
}

void merge(struct event **dst, struct event **src, int begin, int middle, int end, int l_player)
{
	int i = begin, j = middle + 1, k;
	int count = 0;

#ifdef DEBUG_MERGE_SORT
	printf("%s: begin %d, middle %d, end %d, l_player %d\n", __func__, begin, middle, end, l_player);
#endif

	/* (begin, middle) to (middle + 1, end) */
	for (k = begin; k <= end; k++) {
		if ((i <= middle) &&
		    ((j > end) || (src[i]->power > src[j]->power))) {

			if (src[i]->type == 'P') {
				player[src[i]->right] += Memory::get(src[i]->left);
				l_player--;
			}

			dst[k] = src[i];
			i += 1;
		} else {

			if (l_player != 0 && src[j]->type == 'A') {
				Memory::add(src[j]->left, src[j]->right, src[j]->damage);
				count++;
			}

			dst[k] = src[j];
			j += 1;
		}
	}

	/* reset */
	/* Memory::reset(); too slow */

	for (int k = middle + 1; k <= end; k++) {
		if (count == 0)
			break;

		if (src[k]->type == 'A') {
			Memory::add(src[k]->left, src[k]->right, (-1) * src[k]->damage);
			count--;
		}
	}
}

int split_merge(struct event **dst, struct event **src, int begin, int end)
{
	int middle;
	int l_player, r_player;

#ifdef DEBUG_MERGE_SORT
	printf("%s: begin %d, end %d\n", __func__, begin, end);
#endif

	/* base case */
	if (begin == end) {
		if (src[begin]->type == 'P')
			return 1;
		return 0;
	}

	/* recursive case */
	middle = (begin + end) / 2;

	l_player = split_merge(src, dst, begin, middle);
	r_player = split_merge(src, dst, middle + 1, end);

	merge(dst, src, begin, middle, end, l_player);

	return l_player + r_player;
}

void merge_sort_events(struct event **dst, struct event **src, int begin, int end)
{
#ifdef DEBUG
	printf("Events sorted by power: begin %d, end %d\n", begin, end);
#endif

	if (end <= begin)
		return;

	memcpy(dst + begin, src + begin, (end - begin + 1) * sizeof(struct event *));

	/* dst is the sorted result */
	split_merge(dst, src, begin, end);

#ifdef DEBUG
	for (int i = begin + 1; i <= end; i++) {
		if (dst[i].power > dst[i - 1].power)
			printf("Fail to sort bombs\n");
	}
#endif
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char line[LINE_SIZE];

	int cell_num, event_num;
	int location, defence;
	int left, right, power, damage;

	int count = 0;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d %d", &cell_num, &event_num);

	Memory::reset();

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		switch (line[0]) {
		case 'P':
			sscanf(line, "P %d %d", &location, &defence);

#ifdef DEBUG
			//printf("P: loc %d, def %d\n", loc, def);

			if ((loc < 1) || (loc > (cell_num + 1))) {
				printf("Indalid player: location %d, def %d\n", location, defence);
				continue;
			}
#endif

			new_player(location, defence);
			break;
		case 'A':
			sscanf(line, "A %d %d %d %d", &left, &right, &power, &damage);

#ifdef DEBUG
			//printf("A: left %d, right %d, power %d, damage %d\n", left, right, power, damage);

			if (left < 1) {
				printf("Indalid event: left %d, right %d, power %d, damage %d\n", left, right, power, damage);
				continue;
			}
#endif

			new_bomb(left, right, power, damage);
			break;
		default:
			printf("Unknown event, line %s\n", line);
			break;
		}

		count++;
	}

#ifdef DEBUG
	if (event_num != count) {
		printf("Data corrpution, event_num %d, count %d\n", event_num, count);
	}
#endif

	for (int i = 0; i < event_num; i++)
		my_tmp1[i] = &my_events[i];

	merge_sort_events(my_tmp2, my_tmp1, 0, event_num-1);

	for (int i = 0; i < player_num; i++) {
		printf("%lld\n", player[i]);
	}

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
