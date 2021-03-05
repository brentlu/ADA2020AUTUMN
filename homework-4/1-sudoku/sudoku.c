#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <string>
#include <vector>

using namespace std;

//#define DEBUG

static int my_abort = 0;
static clock_t my_start, my_end;
static double my_time_used;

#define FOR(i, way, to) for(int i = way[to] ; i != to ; i = way[i])

namespace DLX {
    int col_size, sz;
    vector<int> s, l, r, u, d, row, col, current;
    void Init(int n) {
        s.clear();
        l.clear();
        r.clear();
        u.clear();
        d.clear();
        row.clear();
        col.clear();
        current.clear();
        col_size = n;
        for (int i = 0; i <= n; ++ i) {
            u.emplace_back(i);
            d.emplace_back(i);
            l.emplace_back(i - 1);
            r.emplace_back(i + 1);
        }
        s.resize(n + 1, 0);
        row.resize(n + 1, 0);
        col.resize(n + 1, 0);
        r[n] = 0, l[0] = n, sz = n + 1;
    }
    void AddRow(int rr, vector<int> &sol) {
        int tmp = sz;
        for (auto to : sol) {
            l.emplace_back(sz - 1);
            r.emplace_back(sz + 1);
            d.emplace_back(to);
            u.emplace_back(u[to]);
            d[u[to]] = sz, u[to] = sz;
            row.emplace_back(rr);
            col.emplace_back(to);
            s[to] ++, sz ++;
        }
        r[sz - 1] = tmp, l[tmp] = sz - 1;
    }
    void Remove(int c) {
        l[r[c]] = l[c];
        r[l[c]] = r[c];
        FOR(i, d, c) FOR(j, r, i) {
            u[d[j]] = u[j];
            d[u[j]] = d[j];
            --s[col[j]];
        }
    }
    void Restore(int c) {
        FOR(i, u, c) FOR(j, l, i) {
            ++s[col[j]], u[d[j]] = d[u[j]] = j;
        }
        l[r[c]] = r[l[c]] = c;
    }
    void Dfs(int floor) {
        if(r[0] == 0) return;
        int c = r[0];
        FOR(i, r, 0) {
            if (s[i] <= s[c]) c = i;
            if (s[i] == 0) return;
        }
        Remove(c);
        FOR(i, d, c) {
            FOR(j, r, i) Remove(col[j]);
            current.emplace_back(row[i]);

            my_end = clock();
            my_time_used = ((double) (my_end - my_start)) / CLOCKS_PER_SEC;
            if (my_time_used > 0.4) {
                my_abort = 1;
                return;
            }

            Dfs(floor + 1);
            if (r[0] == 0) return;
            current.pop_back();
            FOR(j, l, i) Restore(col[j]);

        }
        Restore(c);
    }
    vector<int> Solver() {
        my_start = clock();
        return Dfs(0), current;
    }
}

#undef FOR

using namespace DLX;

typedef unsigned long long u64;

#define MASK(i) (0x1 << (i-1))

#define LINE_SIZE 16
#define BOARD_LEN 9

struct cell {
	int row;
	int column;

	struct cage *to_cage;
	struct cell *prev;


	/* runtime data */
	int value;
	unsigned int masks[12];
	int mask_num;
};

struct cage {
	char color;
	int target;

	struct cell *cells[BOARD_LEN];
	int cell_num;

	struct comb *to_comb;

	unsigned int map;
};

struct comb {
	unsigned int masks[12];
	int mask_num;
	unsigned int valid_mask;
};

static char line[LINE_SIZE];

static struct cell my_cells[BOARD_LEN][BOARD_LEN];
static struct cage my_cages[BOARD_LEN*BOARD_LEN];
static int cage_num;

static vector<int> my_constraints[BOARD_LEN][BOARD_LEN][BOARD_LEN];
static int constraint_num;

static vector<int> my_cage_constraint[BOARD_LEN*BOARD_LEN*BOARD_LEN];
static int cage_constraint_num;

static struct comb my_comb_1[] = {
	{.masks = {0x1}, .mask_num = 1, .valid_mask = 0x1}, /* 1 */
	{.masks = {0x2}, .mask_num = 1, .valid_mask = 0x2}, /* 2 */
	{.masks = {0x4}, .mask_num = 1, .valid_mask = 0x4}, /* 3 */
	{.masks = {0x8}, .mask_num = 1, .valid_mask = 0x8}, /* 4 */
	{.masks = {0x10}, .mask_num = 1, .valid_mask = 0x10}, /* 5 */
	{.masks = {0x20}, .mask_num = 1, .valid_mask = 0x20}, /* 6 */
	{.masks = {0x40}, .mask_num = 1, .valid_mask = 0x40}, /* 7 */
	{.masks = {0x80}, .mask_num = 1, .valid_mask = 0x80}, /* 8 */
	{.masks = {0x100}, .mask_num = 1, .valid_mask = 0x100}, /* 9 */
};

static struct comb my_comb_2[] = {
	{.masks = {0x3}, .mask_num = 1, .valid_mask = 0x3}, /* 3 */
	{.masks = {0x5}, .mask_num = 1, .valid_mask = 0x5}, /* 4 */
	{.masks = {0x9, 0x6}, .mask_num = 2, .valid_mask = 0xf}, /* 5 */
	{.masks = {0x11, 0xa}, .mask_num = 2, .valid_mask = 0x1b}, /* 6 */
	{.masks = {0x21, 0x12, 0xc}, .mask_num = 3, .valid_mask = 0x3f}, /* 7 */
	{.masks = {0x41, 0x22, 0x14}, .mask_num = 3, .valid_mask = 0x77}, /* 8 */
	{.masks = {0x81, 0x42, 0x24, 0x18}, .mask_num = 4, .valid_mask = 0xff}, /* 9 */
	{.masks = {0x101, 0x82, 0x44, 0x28}, .mask_num = 4, .valid_mask = 0x1ef}, /* 10 */
	{.masks = {0x102, 0x84, 0x48, 0x30}, .mask_num = 4, .valid_mask = 0x1fe}, /* 11 */
	{.masks = {0x104, 0x88, 0x50}, .mask_num = 3, .valid_mask = 0x1dc}, /* 12 */
	{.masks = {0x108, 0x90, 0x60}, .mask_num = 3, .valid_mask = 0x1f8}, /* 13 */
	{.masks = {0x110, 0xa0}, .mask_num = 2, .valid_mask = 0x1b0}, /* 14 */
	{.masks = {0x120, 0xc0}, .mask_num = 2, .valid_mask = 0x1e0}, /* 15 */
	{.masks = {0x140}, .mask_num = 1, .valid_mask = 0x140}, /* 16 */
	{.masks = {0x180}, .mask_num = 1, .valid_mask = 0x180}, /* 17 */
};

static struct comb my_comb_3[] = {
	{.masks = {0x7}, .mask_num = 1, .valid_mask = 0x7}, /* 6 */
	{.masks = {0xb}, .mask_num = 1, .valid_mask = 0xb}, /* 7 */
	{.masks = {0x13, 0xd}, .mask_num = 2, .valid_mask = 0x1f}, /* 8 */
	{.masks = {0x23, 0x15, 0xe}, .mask_num = 3, .valid_mask = 0x3f}, /* 9 */
	{.masks = {0x43, 0x25, 0x19, 0x16}, .mask_num = 4, .valid_mask = 0x7f}, /* 10 */
	{.masks = {0x83, 0x45, 0x29, 0x26, 0x1a}, .mask_num = 5, .valid_mask = 0xff}, /* 11 */
	{.masks = {0x103, 0x85, 0x49, 0x31, 0x46, 0x2a, 0x1c}, .mask_num = 7, .valid_mask = 0x1ff}, /* 12 */
	{.masks = {0x105, 0x89, 0x51, 0x86, 0x4a, 0x32, 0x2c}, .mask_num = 7, .valid_mask = 0x1ff}, /* 13 */
	{.masks = {0x109, 0x91, 0x61, 0x106, 0x8a, 0x52, 0x4c, 0x34}, .mask_num = 8, .valid_mask = 0x1ff}, /* 14 */
	{.masks = {0x111, 0xa1, 0x10a, 0x92, 0x62, 0x8c, 0x54, 0x38}, .mask_num = 8, .valid_mask = 0x1ff}, /* 15 */
	{.masks = {0x121, 0xc1, 0x112, 0xa2, 0x10c, 0x94, 0x64, 0x58}, .mask_num = 8, .valid_mask = 0x1ff}, /* 16 */
	{.masks = {0x141, 0x122, 0xc2, 0x114, 0xa4, 0x98, 0x68}, .mask_num = 7, .valid_mask = 0x1ff}, /* 17 */
	{.masks = {0x181, 0x142, 0x124, 0xc4, 0x118, 0xa8, 0x70}, .mask_num = 7, .valid_mask = 0x1ff}, /* 18 */
	{.masks = {0x182, 0x144, 0x128, 0xc8, 0xb0}, .mask_num = 5, .valid_mask = 0x1fe}, /* 19 */
	{.masks = {0x184, 0x148, 0x130, 0xd0}, .mask_num = 4, .valid_mask = 0x1fc}, /* 20 */
	{.masks = {0x188, 0x150, 0xe0}, .mask_num = 3, .valid_mask = 0x1f8}, /* 21 */
	{.masks = {0x190, 0x160}, .mask_num = 2, .valid_mask = 0x1f0}, /* 22 */
	{.masks = {0x1a0}, .mask_num = 1, .valid_mask = 0x1a0}, /* 23 */
	{.masks = {0x1c0}, .mask_num = 1, .valid_mask = 0x1c0}, /* 24 */
};

static struct comb my_comb_4[] = {
	{.masks = {0xf}, .mask_num = 1, .valid_mask = 0xf}, /* 10 */
	{.masks = {0x17}, .mask_num = 1, .valid_mask = 0x17}, /* 11 */
	{.masks = {0x27, 0x1b}, .mask_num = 2, .valid_mask = 0x3f}, /* 12 */
	{.masks = {0x47, 0x2b, 0x1d}, .mask_num = 3, .valid_mask = 0x7f}, /* 13 */
	{.masks = {0x87, 0x4b, 0x33, 0x2d, 0x1e}, .mask_num = 5, .valid_mask = 0xff}, /* 14 */
	{.masks = {0x107, 0x8b, 0x53, 0x4d, 0x35, 0x2e}, .mask_num = 6, .valid_mask = 0x1ff}, /* 15 */
	{.masks = {0x10b, 0x93, 0x63, 0x8d, 0x55, 0x39, 0x4e, 0x36}, .mask_num = 8, .valid_mask = 0x1ff}, /* 16 */
	{.masks = {0x113, 0xa3, 0x10d, 0x95, 0x65, 0x59, 0x8e, 0x56, 0x3a}, .mask_num = 9, .valid_mask = 0x1ff}, /* 17 */
	{.masks = {0x123, 0xc3, 0x115, 0xa5, 0x99, 0x69, 0x10e, 0x96, 0x66, 0x5a, 0x3c}, .mask_num = 11, .valid_mask = 0x1ff}, /* 18 */
	{.masks = {0x143, 0x125, 0xc5, 0x119, 0xa9, 0x71, 0x116, 0xa6, 0x9a, 0x6a, 0x5c}, .mask_num = 11, .valid_mask = 0x1ff}, /* 19 */
	{.masks = {0x183, 0x145, 0x129, 0xc9, 0xb1, 0x126, 0xc6, 0x11a, 0xaa, 0x72, 0x9c, 0x6c}, .mask_num = 12, .valid_mask = 0x1ff}, /* 20 */
	{.masks = {0x185, 0x149, 0x131, 0xd1, 0x146, 0x12a, 0xca, 0xb2, 0x11c, 0xac, 0x74}, .mask_num = 11, .valid_mask = 0x1ff}, /* 21 */
	{.masks = {0x189, 0x151, 0xe1, 0x186, 0x14a, 0x132, 0xd2, 0x12c, 0xcc, 0xb4, 0x78}, .mask_num = 11, .valid_mask = 0x1ff}, /* 22 */
	{.masks = {0x191, 0x161, 0x18a, 0x152, 0xe2, 0x14c, 0x134, 0xd4, 0xb8}, .mask_num = 9, .valid_mask = 0x1ff}, /* 23 */
	{.masks = {0x1a1, 0x192, 0x162, 0x18c, 0x154, 0xe4, 0x138, 0xd8}, .mask_num = 8, .valid_mask = 0x1ff}, /* 24 */
	{.masks = {0x1c1, 0x1a2, 0x194, 0x164, 0x158, 0xe8}, .mask_num = 6, .valid_mask = 0x1ff}, /* 25 */
	{.masks = {0x1c2, 0x1a4, 0x198, 0x168, 0xf0}, .mask_num = 5, .valid_mask = 0x1fe}, /* 26 */
	{.masks = {0x1c4, 0x1a8, 0x170}, .mask_num = 3, .valid_mask = 0x1fc}, /* 27 */
	{.masks = {0x1c8, 0x1b0}, .mask_num = 2, .valid_mask = 0x1f8}, /* 28 */
	{.masks = {0x1d0}, .mask_num = 1, .valid_mask = 0x1d0}, /* 29 */
	{.masks = {0x1e0}, .mask_num = 1, .valid_mask = 0x1e0}, /* 30 */
};

static struct comb my_comb_5[] = {
	{.masks = {0x1f}, .mask_num = 1, .valid_mask = 0x1f}, /* 15 */
	{.masks = {0x2f}, .mask_num = 1, .valid_mask = 0x2f}, /* 16 */
	{.masks = {0x4f, 0x37}, .mask_num = 2, .valid_mask = 0x7f}, /* 17 */
	{.masks = {0x8f, 0x57, 0x3b}, .mask_num = 3, .valid_mask = 0xff}, /* 18 */
	{.masks = {0x10f, 0x97, 0x67, 0x5b, 0x3d}, .mask_num = 5, .valid_mask = 0x1ff}, /* 19 */
	{.masks = {0x117, 0xa7, 0x9b, 0x6b, 0x5d, 0x3e}, .mask_num = 6, .valid_mask = 0x1ff}, /* 20 */
	{.masks = {0x127, 0xc7, 0x11b, 0xab, 0x73, 0x9d, 0x6d, 0x5e}, .mask_num = 8, .valid_mask = 0x1ff}, /* 21 */
	{.masks = {0x147, 0x12b, 0xcb, 0xb3, 0x11d, 0xad, 0x75, 0x9e, 0x6e}, .mask_num = 9, .valid_mask = 0x1ff}, /* 22 */
	{.masks = {0x187, 0x14b, 0x133, 0xd3, 0x12d, 0xcd, 0xb5, 0x79, 0x11e, 0xae, 0x76}, .mask_num = 11, .valid_mask = 0x1ff}, /* 23 */
	{.masks = {0x18b, 0x153, 0xe3, 0x14d, 0x135, 0xd5, 0xb9, 0x12e, 0xce, 0xb6, 0x7a}, .mask_num = 11, .valid_mask = 0x1ff}, /* 24 */
	{.masks = {0x193, 0x163, 0x18d, 0x155, 0xe5, 0x139, 0xd9, 0x14e, 0x136, 0xd6, 0xba, 0x7c}, .mask_num = 12, .valid_mask = 0x1ff}, /* 25 */
	{.masks = {0x1a3, 0x195, 0x165, 0x159, 0xe9, 0x18e, 0x156, 0xe6, 0x13a, 0xda, 0xbc}, .mask_num = 11, .valid_mask = 0x1ff}, /* 26 */
	{.masks = {0x1c3, 0x1a5, 0x199, 0x169, 0xf1, 0x196, 0x166, 0x15a, 0xea, 0x13c, 0xdc}, .mask_num = 11, .valid_mask = 0x1ff}, /* 27 */
	{.masks = {0x1c5, 0x1a9, 0x171, 0x1a6, 0x19a, 0x16a, 0xf2, 0x15c, 0xec}, .mask_num = 9, .valid_mask = 0x1ff}, /* 28 */
	{.masks = {0x1c9, 0x1b1, 0x1c6, 0x1aa, 0x172, 0x19c, 0x16c, 0xf4}, .mask_num = 8, .valid_mask = 0x1ff}, /* 29 */
	{.masks = {0x1d1, 0x1ca, 0x1b2, 0x1ac, 0x174, 0xf8}, .mask_num = 6, .valid_mask = 0x1ff}, /* 30 */
	{.masks = {0x1e1, 0x1d2, 0x1cc, 0x1b4, 0x178}, .mask_num = 5, .valid_mask = 0x1ff}, /* 31 */
	{.masks = {0x1e2, 0x1d4, 0x1b8}, .mask_num = 3, .valid_mask = 0x1fe}, /* 32 */
	{.masks = {0x1e4, 0x1d8}, .mask_num = 2, .valid_mask = 0x1fc}, /* 33 */
	{.masks = {0x1e8}, .mask_num = 1, .valid_mask = 0x1e8}, /* 34 */
	{.masks = {0x1f0}, .mask_num = 1, .valid_mask = 0x1f0}, /* 35 */
};

static struct comb my_comb_6[] = {
	{.masks = {0x3f}, .mask_num = 1, .valid_mask = 0x3f}, /* 21 */
	{.masks = {0x5f}, .mask_num = 1, .valid_mask = 0x5f}, /* 22 */
	{.masks = {0x9f, 0x6f}, .mask_num = 2, .valid_mask = 0xff}, /* 23 */
	{.masks = {0x11f, 0xaf, 0x77}, .mask_num = 3, .valid_mask = 0x1ff}, /* 24 */
	{.masks = {0x12f, 0xcf, 0xb7, 0x7b}, .mask_num = 4, .valid_mask = 0x1ff}, /* 25 */
	{.masks = {0x14f, 0x137, 0xd7, 0xbb, 0x7d}, .mask_num = 5, .valid_mask = 0x1ff}, /* 26 */
	{.masks = {0x18f, 0x157, 0xe7, 0x13b, 0xdb, 0xbd, 0x7e}, .mask_num = 7, .valid_mask = 0x1ff}, /* 27 */
	{.masks = {0x197, 0x167, 0x15b, 0xeb, 0x13d, 0xdd, 0xbe}, .mask_num = 7, .valid_mask = 0x1ff}, /* 28 */
	{.masks = {0x1a7, 0x19b, 0x16b, 0xf3, 0x15d, 0xed, 0x13e, 0xde}, .mask_num = 8, .valid_mask = 0x1ff}, /* 29 */
	{.masks = {0x1c7, 0x1ab, 0x173, 0x19d, 0x16d, 0xf5, 0x15e, 0xee}, .mask_num = 8, .valid_mask = 0x1ff}, /* 30 */
	{.masks = {0x1cb, 0x1b3, 0x1ad, 0x175, 0xf9, 0x19e, 0x16e, 0xf6}, .mask_num = 8, .valid_mask = 0x1ff}, /* 31 */
	{.masks = {0x1d3, 0x1cd, 0x1b5, 0x179, 0x1ae, 0x176, 0xfa}, .mask_num = 7, .valid_mask = 0x1ff}, /* 32 */
	{.masks = {0x1e3, 0x1d5, 0x1b9, 0x1ce, 0x1b6, 0x17a, 0xfc}, .mask_num = 7, .valid_mask = 0x1ff}, /* 33 */
	{.masks = {0x1e5, 0x1d9, 0x1d6, 0x1ba, 0x17c}, .mask_num = 5, .valid_mask = 0x1ff}, /* 34 */
	{.masks = {0x1e9, 0x1e6, 0x1da, 0x1bc}, .mask_num = 4, .valid_mask = 0x1ff}, /* 35 */
	{.masks = {0x1f1, 0x1ea, 0x1dc}, .mask_num = 3, .valid_mask = 0x1ff}, /* 36 */
	{.masks = {0x1f2, 0x1ec}, .mask_num = 2, .valid_mask = 0x1fe}, /* 37 */
	{.masks = {0x1f4}, .mask_num = 1, .valid_mask = 0x1f4}, /* 38 */
	{.masks = {0x1f8}, .mask_num = 1, .valid_mask = 0x1f8}, /* 39 */
};

static struct comb my_comb_7[] = {
	{.masks = {0x7f}, .mask_num = 1, .valid_mask = 0x7f}, /* 28 */
	{.masks = {0xbf}, .mask_num = 1, .valid_mask = 0xbf}, /* 29 */
	{.masks = {0x13f, 0xdf}, .mask_num = 2, .valid_mask = 0x1ff}, /* 30 */
	{.masks = {0x15f, 0xef}, .mask_num = 2, .valid_mask = 0x1ff}, /* 31 */
	{.masks = {0x19f, 0x16f, 0xf7}, .mask_num = 3, .valid_mask = 0x1ff}, /* 32 */
	{.masks = {0x1af, 0x177, 0xfb}, .mask_num = 3, .valid_mask = 0x1ff}, /* 33 */
	{.masks = {0x1cf, 0x1b7, 0x17b, 0xfd}, .mask_num = 4, .valid_mask = 0x1ff}, /* 34 */
	{.masks = {0x1d7, 0x1bb, 0x17d, 0xfe}, .mask_num = 4, .valid_mask = 0x1ff}, /* 35 */
	{.masks = {0x1e7, 0x1db, 0x1bd, 0x17e}, .mask_num = 4, .valid_mask = 0x1ff}, /* 36 */
	{.masks = {0x1eb, 0x1dd, 0x1be}, .mask_num = 3, .valid_mask = 0x1ff}, /* 37 */
	{.masks = {0x1f3, 0x1ed, 0x1de}, .mask_num = 3, .valid_mask = 0x1ff}, /* 38 */
	{.masks = {0x1f5, 0x1ee}, .mask_num = 2, .valid_mask = 0x1ff}, /* 39 */
	{.masks = {0x1f9, 0x1f6}, .mask_num = 2, .valid_mask = 0x1ff}, /* 40 */
	{.masks = {0x1fa}, .mask_num = 1, .valid_mask = 0x1fa}, /* 41 */
	{.masks = {0x1fc}, .mask_num = 1, .valid_mask = 0x1fc}, /* 42 */
};

static struct comb my_comb_8[] = {
	{.masks = {0xff}, .mask_num = 1, .valid_mask = 0xff}, /* 36 */
	{.masks = {0x17f}, .mask_num = 1, .valid_mask = 0x17f}, /* 37 */
	{.masks = {0x1bf}, .mask_num = 1, .valid_mask = 0x1bf}, /* 38 */
	{.masks = {0x1df}, .mask_num = 1, .valid_mask = 0x1df}, /* 39 */
	{.masks = {0x1ef}, .mask_num = 1, .valid_mask = 0x1ef}, /* 40 */
	{.masks = {0x1f7}, .mask_num = 1, .valid_mask = 0x1f7}, /* 41 */
	{.masks = {0x1fb}, .mask_num = 1, .valid_mask = 0x1fb}, /* 42 */
	{.masks = {0x1fd}, .mask_num = 1, .valid_mask = 0x1fd}, /* 43 */
	{.masks = {0x1fe}, .mask_num = 1, .valid_mask = 0x1fe}, /* 44 */
};

static struct comb my_comb_9[] = {
	{.masks = {0x1ff}, .mask_num = 1, .valid_mask = 0x1ff}, /* 45 */
};


static struct comb *my_combs[] = {
	my_comb_1, my_comb_2, my_comb_3, my_comb_4, my_comb_5, my_comb_6, my_comb_7, my_comb_8, my_comb_9,
};

static int my_comb_offset[] = {
	1, 3, 6, 10, 15, 21, 28, 36, 45,
};

static unsigned int row_map[BOARD_LEN];
static unsigned int column_map[BOARD_LEN];
static unsigned int box_map[BOARD_LEN];

int find_next(int row, int column, int start)
{
	int next = start, box = (3*(column/3)) + (row/3);
	struct cell *ce = &my_cells[row][column];
	struct cell *prev = ce->prev;
	struct cage *ca = ce->to_cage;
	struct comb *co = ca->to_comb;
	unsigned int map = row_map[row] | column_map[column] | box_map[box] | ca->map, tmp;

	while (next <= BOARD_LEN) {

		/* check row, column, box, cage */
		if (map & MASK(next)) {
			/* already used in somewhere */
			next++;
			continue;
		}

		//if (co->valid_mask & MASK(next)) {
			tmp = ca->map | MASK(next);

			ce->mask_num = 0;

			if (prev == NULL) {
				for (int i = 0; i < co->mask_num; i++) {
					if ((tmp & co->masks[i]) == tmp) {
						ce->masks[ce->mask_num] = co->masks[i];
						ce->mask_num++;
					}
				}
			} else {
				for (int i = 0; i < prev->mask_num; i++) {
					if ((tmp & prev->masks[i]) == tmp) {
						/* copy mask from previous cell in the cage */
						ce->masks[ce->mask_num] = prev->masks[i];
						ce->mask_num++;
					}
				}
			}

			if (ce->mask_num != 0)
				return next;
		//}


		next++;
	}

	return -1;
}

int sudoku_bt(int row, int column)
{
	int next = 1, box = (3*(column/3)) + (row/3), ret;

	while (next <= BOARD_LEN) {
		next = find_next(row, column, next);

		if (next == -1)
			return -1;

		my_cells[row][column].value = next;

		if ((row == BOARD_LEN-1) && (column == BOARD_LEN-1))
			return 0;

		row_map[row] |= MASK(next);
		column_map[column] |= MASK(next);
		box_map[box] |= MASK(next);
		my_cells[row][column].to_cage->map |= MASK(next);

		if (column == BOARD_LEN-1)
			ret = sudoku_bt(row+1, 0);
		else
			ret = sudoku_bt(row, column+1);

		if (ret == 0)
			return ret;

		row_map[row] &= ~MASK(next);
		column_map[column] &= ~MASK(next);
		box_map[box] &= ~MASK(next);
		my_cells[row][column].to_cage->map &= ~MASK(next);

		///my_cells[row][column].value = -1;

		next++;
	}

	return -1;
}

int sudoku(int c)
{
	int row, column, value;
	vector<int> result;

	Init(constraint_num);

	for (int i = 0; i < cage_num; i++) {
		unsigned int mask = my_cages[i].to_comb->valid_mask;

		for (value = 0; value < BOARD_LEN; value++) {
			if (mask & (0x1 << value)) {
				for (int j = 0; j < my_cages[i].cell_num; j++) {
					row = my_cages[i].cells[j]->row;
					column = my_cages[i].cells[j]->column;

					AddRow((row*81)+(column*9)+value+1, my_constraints[row][column][value]);
				}
			}
		}
	}

	/* pseudo cell */
	for (int i = 0; i < cage_constraint_num; i++) {
		AddRow((BOARD_LEN*BOARD_LEN*BOARD_LEN)+i+1, my_cage_constraint[i]);
	}

	result = Solver();

	if (my_abort != 0) {
		return sudoku_bt(0, 0);
	}

	if (result.empty())
		return -1;

	for (int i = 0; i < result.size(); i++) {
		if (result[i] > BOARD_LEN*BOARD_LEN*BOARD_LEN)
			continue;

		row = (result[i]-1)/81;
		column = (result[i]-1-(row*81))/9;
		value = ((result[i]-1)%9)+1;

		my_cells[row][column].value = value;
	}

	return 0;
}

void init_constraints(void)
{
	int row, column, value, idx = 0;

	int count;

	/* One value per cell constraint */
	for (row = 0; row < BOARD_LEN; row++) {
		for (column = 0; column < BOARD_LEN; column++) {
			idx++;
			for (value = 0; value < BOARD_LEN; value++) {
				my_constraints[row][column][value].emplace_back(idx);
			}
		}
	}

	/* Row constraint */
	for (row = 0; row < BOARD_LEN; row++) {
		for (value = 0; value < BOARD_LEN; value++) {
			idx++;
			for (column = 0; column < BOARD_LEN; column++) {
				my_constraints[row][column][value].emplace_back(idx);
			}
		}
	}

	/* Column constraint */
	for (column = 0; column < BOARD_LEN; column++) {
		for (value = 0; value < BOARD_LEN; value++) {
			idx++;
			for (row = 0; row < BOARD_LEN; row++) {
				my_constraints[row][column][value].emplace_back(idx);
			}
		}
	}

	/* Box Constraint */
	for (value = 0; value < BOARD_LEN; value++) {
		for (row = 0; row < BOARD_LEN; row++) {
			for (column = 0; column < BOARD_LEN; column++) {
				my_constraints[row][column][value].emplace_back(idx+(3*(row/3))+(column/3)+1);
			}
		}

		idx += 9;
	}

	cage_constraint_num = 0;

	/* Cage Constraint */
	for (int ca = 0; ca < cage_num; ca++) {
		unsigned int *masks = my_cages[ca].to_comb->masks;
		int mask_num = my_cages[ca].to_comb->mask_num;
		unsigned int valid_mask = my_cages[ca].to_comb->valid_mask;
		int constraints[BOARD_LEN];

#ifdef DEBUG
		printf("cage %d: num %d, target %d\n", ca, my_cages[ca].cell_num, my_cages[ca].target);
#endif

		if (my_cages[ca].cell_num == 1)
			continue;

		count = 0;
		for (int value = 0; value < BOARD_LEN; value++) {
			if ((0x1 << value) & valid_mask) {
				for (int ce = 0; ce < my_cages[ca].cell_num; ce++) {
					row = my_cages[ca].cells[ce]->row;
					column = my_cages[ca].cells[ce]->column;

					my_constraints[row][column][value].emplace_back(idx+1+count);
					constraints[value] = idx+1+count;
#ifdef DEBUG
					printf("    add %d to [%d][%d][%d]\n", idx+1+count, row, column, value);
#endif
				}
				count++;
			}
		}

		if (mask_num != 1) {
			for (int ma = 0; ma < mask_num; ma++) {
				unsigned key = valid_mask & ~(masks[ma]);

#ifdef DEBUG
				printf("    add");
#endif
				for (int value = 0; value < BOARD_LEN; value++) {
					if ((0x1 << value) & key) {
						my_cage_constraint[cage_constraint_num].emplace_back(constraints[value]);
#ifdef DEBUG
						printf(" %d", constraints[value]);
#endif
					}
				}
#ifdef DEBUG
				printf(" to [%d]\n", cage_constraint_num);
#endif
				cage_constraint_num++;
			}
		}

		idx += count;
	}

	constraint_num = idx;
#ifdef DEBUG
	printf("constraint_num: %d, cage_constraint_num %d\n", constraint_num, cage_constraint_num);
#endif
}

int main(int argc, char **argv)
{
	clock_t start, end;
	double time_used;

	char *token;

	int row;
	char color;
	int target, ret;

	start = clock();

	for (int i = 0; i < BOARD_LEN; i++) {
		for (int j = 0; j < BOARD_LEN; j++) {
			my_cells[i][j].row = i;
			my_cells[i][j].column = j;
			my_cells[i][j].value = -1;
		}

		row_map[i] = 0x0;
		column_map[i] = 0x0;
		box_map[i] = 0x0;
	}

	for (int i = 0; i < BOARD_LEN*BOARD_LEN; i++) {
		my_cages[i].color = 0;
		my_cages[i].cell_num = 0;
		my_cages[i].map = 0x0;
	}

	cage_num = 0;

	row = 0;

	/* parse each line */
	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		for (int column = 0; column < BOARD_LEN; column++) {
			int i;

			for (i = 0; i < cage_num; i++) {
				if (line[column] == my_cages[i].color) {
					my_cages[i].cells[my_cages[i].cell_num] = &my_cells[row][column];

					my_cells[row][column].to_cage = &my_cages[i];
					if (my_cages[i].cell_num == 0)
						my_cells[row][column].prev = NULL;
					else
						my_cells[row][column].prev = my_cages[i].cells[my_cages[i].cell_num-1];

					my_cages[i].cell_num++;
					break;
				}
			}
			if (i == cage_num) {
				my_cages[i].color = line[column];

				my_cages[i].cells[my_cages[i].cell_num] = &my_cells[row][column];

				my_cells[row][column].to_cage = &my_cages[i];
				if (my_cages[i].cell_num == 0)
					my_cells[row][column].prev = NULL;
				else
					my_cells[row][column].prev = my_cages[i].cells[my_cages[i].cell_num-1];

				my_cages[i].cell_num++;
				cage_num++;
			}
		}

		row++;

		if (row == BOARD_LEN)
			break;
	}

	while (fgets(line, LINE_SIZE, stdin) != NULL) {
		int i;

		sscanf(line, "%c %d", &color, &target);

		for (i = 0; i < cage_num; i++) {
			if (color == my_cages[i].color) {
				my_cages[i].target = target;
				my_cages[i].to_comb = my_combs[my_cages[i].cell_num-1];
				my_cages[i].to_comb += my_cages[i].target - my_comb_offset[my_cages[i].cell_num-1];
				break;
			}
		}
		if (i == cage_num) {
			printf("color not found\n");
		}
	}

#ifdef DEBUG
	printf("init\n");
	for (int i = 0; i < BOARD_LEN; i++) {
		for (int j = 0; j < BOARD_LEN; j++) {
			printf("%2d ", my_cells[i][j].to_cage->target);
		}
		printf("\n");
	}
#endif

	init_constraints();

	sudoku(0);

	for (int i = 0; i < BOARD_LEN; i++) {
		for (int j = 0; j < BOARD_LEN; j++) {
			printf("%d", my_cells[i][j].value);
		}
		printf("\n");
	}

exit:
	end = clock();
	time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

#ifdef DEBUG
	printf("time used: %f\n", time_used);
#endif

	return 0;
}
