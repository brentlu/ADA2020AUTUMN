#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DEBUG
//#define DEBUG_MERGE_SORT

typedef signed long long s64;
typedef unsigned long long u64; 

#define LINE_SIZE (2 * 16)

#define MIN2(a, b) (((a) > (b)) ? (b) : (a))
#define MIN3(a, b, c) (((a) > (b)) ? ((((b) > (c)) ? (c) : (b))) : ((((a) > (c)) ? (c) : (a))))

struct city {
	int x;
	int y;
};

struct city *my_city = NULL;
struct city *my_city_sort_x = NULL;
struct city *my_city_sort_y = NULL;
int city_num;

s64 calculate_distance(struct city *cities, int city1, int city2)
{
	s64 diff1, diff2;

	/*
	 * profit = -(Ax-Bx)^2 - (Ay-By)^2
	 */

	diff1 = (cities[city1].x - cities[city2].x);
	diff1 *= diff1;

	diff2 = (cities[city1].y - cities[city2].y);
	diff2 *= diff2;

	return (diff1 + diff2);
}

int sort_x = 0;

void merge(struct city *src, struct city *dst, int begin, int middle, int end)
{
	int i = begin, j = middle + 1, k;

#ifdef DEBUG_MERGE_SORT
	printf("%s: (%d, %d, %d)\n", __func__, begin, middle, end);
#endif

	/* (begin, middle) to (middle + 1, end) */
	for (k = begin; k <= end; k++) {
		if (sort_x != 0) {
			if ((i <= middle) && ((j > end) || src[i].x <= src[j].x)) {
				dst[k] = src[i];
				i += 1;
			} else {
				dst[k] = src[j];
				j += 1;
			}
		} else {
			if ((i <= middle) && ((j > end) || src[i].y <= src[j].y)) {
				dst[k] = src[i];
				i += 1;
			} else {
				dst[k] = src[j];
				j += 1;
			}
		}
	}
}

void split_merge(struct city *src, struct city *dst, int begin, int end)
{
	int middle;

#ifdef DEBUG_MERGE_SORT
	printf("%s: (%d, %d)\n", __func__, begin, end);
#endif

	/* base case */
	if (begin == end)
		return;

	/* recursive case */
	middle = (begin + end) / 2;

	split_merge(dst, src, begin, middle);
	split_merge(dst, src, middle + 1, end);

	merge(src, dst, begin, middle, end);
}

void merge_sort(struct city *src, struct city *dst, int begin, int end, int sort)
{
	sort_x = sort;

#ifdef DEBUG_MERGE_SORT
	printf("Cities sorted by %c: begin %d, end %d\n", sort != 0 ? 'x' :'y', begin, end);
#endif

	memcpy(dst + begin, src + begin, (end - begin + 1) * sizeof(struct city));

	/* dst is the sorted result */
	split_merge(src, dst, begin, end);

#ifdef DEBUG_MERGE_SORT
	for (int i = begin; i <= end; i++) {
		printf("  (%d, %d)\n", dst[i].x, dst[i].y);
	}
#endif

#ifdef DEBUG
	if (sort != 0) {
		for (int i = begin + 1; i < end; i++) {
			if (dst[i].x < dst[i - 1].x)
				printf("Fail to sort cities\n");
		}
	} else {
		for (int i = begin + 1; i < end; i++) {
			if (dst[i].y < dst[i - 1].y)
				printf("Fail to sort cities\n");
		}
	}
#endif
}

s64 closest_pair(int begin, int end)
{
	s64 dist1, dist2, dist3;

	int middle, i;
	u64 l_dist, r_dist, min_dist;
	s64 dist;
	int cross_begin, cross_end;

#ifdef DEBUG
	printf("%s: (%d, %d)\n", __func__, begin, end);
#endif

	/* base case */
	switch (end - begin) {
	case 1: /* 2 points */
		return calculate_distance(my_city_sort_x, begin, end);
	case 2: /* 3 points */
		dist1 = calculate_distance(my_city_sort_x, begin, begin + 1);
		dist2 = calculate_distance(my_city_sort_x, begin, begin + 2);
		dist3 = calculate_distance(my_city_sort_x, begin + 1, begin + 2);

		return MIN3(dist1, dist2, dist3);
	default:
		break;
	}

	/* recursive case */
	middle = (begin + end) / 2;

	l_dist = closest_pair(begin, middle);
	r_dist = closest_pair(middle + 1, end);

	min_dist = MIN2(l_dist, r_dist);

	/* (begin, middle) */
	for (i = middle; i >= begin; i--) {
		dist = my_city_sort_x[middle + 1].x - my_city_sort_x[i].x;
		dist *= dist;

		if (dist >= min_dist)
			break;
	}

	if (i == middle)
		return min_dist;

	cross_begin = i + 1;

	/* (middle + 1, end) */
	for (i = middle + 1; i <= end; i++) {
		dist = my_city_sort_x[middle].x - my_city_sort_x[i].x;
		dist *= dist;

		if (dist >= min_dist)
			break;
	}

	if (i == middle + 1)
		return min_dist;

	cross_end = i - 1;

	memcpy(my_city + cross_begin, my_city_sort_x + cross_begin, (cross_end - cross_begin + 1) * sizeof(struct city));

	/* sort by y */
	merge_sort(my_city, my_city_sort_y, cross_begin, cross_end, 0);

	for (int i = cross_begin; i < cross_end; i++) {
		for (int j = 1; j <= 7; j++) {
			if (i + j > cross_end)
				break;

			dist = calculate_distance(my_city_sort_y, i, i + j);
			if (dist < min_dist)
				min_dist = dist;
		}
	}

#ifdef DEBUG
	printf("  xp: dist %llu, (%d, %d), (%d, %d)\n", my_pair->dist, my_pair->city1.x, my_pair->city1.y, my_pair->city2.x, my_pair->city2.y);
#endif
	return min_dist;
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char line[LINE_SIZE];

	int city_x, city_y;
	int city_idx = 0;

	u64 dist;

	start = clock();

	/* parse first line */
	if (fgets(line, LINE_SIZE, stdin) == NULL) {
		printf("Fail to get first line\n");
		goto exit;
	}

	sscanf(line, "%d", &city_num);
#ifdef DEBUG
	printf("Total cities: %d\n", city_num);
#endif

	my_city = (struct city *)malloc(sizeof(struct city) * city_num);
	if (my_city == NULL) {
		printf("Fail to allocate memory\n");
		goto exit;
	}

	my_city_sort_x = (struct city *)malloc(sizeof(struct city) * city_num);
	if (my_city_sort_x == NULL) {
		printf("Fail to allocate memory\n");
		goto exit;
	}

	my_city_sort_y = (struct city *)malloc(sizeof(struct city) * city_num);
	if (my_city_sort_y == NULL) {
		printf("Fail to allocate memory\n");
		goto exit;
	}

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		/* get city position */
		sscanf(line, "%d %d", &city_x, &city_y);

		if ((city_x == 0) && (city_y == 0)) {
#ifdef DEBUG
			printf("Invalid city (%d, %d)\n", city_x, city_y);
#endif
			continue;
		}

#ifdef DEBUG
		//printf("City %d: (%d, %d)\n", city_idx, city_x, city_y);
#endif

		my_city[city_idx].x = city_x;
		my_city[city_idx].y = city_y;

		city_idx++;
	}

#ifdef DEBUG
	if (city_num != city_idx) {
		printf("Data corrpution, city_num %d, city_idx %d\n", city_num, city_idx);
	}
#endif

	/* sort by x */
#ifdef DEBUG_MERGE_SORT
	printf("Cities sorted by x: begin %d, end %d\n", 0, city_idx - 1);
#endif

	merge_sort(my_city, my_city_sort_x, 0, city_idx - 1, 1);

#ifdef DEBUG_MERGE_SORT
	for (int i = 0; i < city_idx; i++) {
		printf("  (%d, %d)\n", my_city_sort_x[i].x, my_city_sort_x[i].y);
	}
#endif

#ifdef DEBUG
	for (int i = 1; i < city_idx; i++) {
		if (my_city_sort_x[i].x < my_city_sort_x[i - 1].x)
			printf("Fail to sort cities\n");
	}
#endif

	dist = closest_pair(0, city_idx - 1);

#ifdef DEBUG
	printf("  City (%d, %d), (%d, %d)\n", my_pair.city1.x, my_pair.city1.y, my_pair.city2.x, my_pair.city2.y);
#endif

	//printf("  City (%d, %d), (%d, %d) with profit: %lld\n", my_pair.city1.x, my_pair.city1.y, my_pair.city2.x, my_pair.city2.y, my_pair.dist);

	printf("%lld\n", (-1) * (s64)dist);

exit:
	if (my_city != NULL)
		free(my_city);

	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
