#!/usr/bin/python3

import re
import sys

def main():

    for line in sys.stdin:
        result = line.split(':')

        sum = int(result[0])
        #print('sum=%d' % (sum))

        result[1] = result[1].lstrip(' ')
        result[1] = result[1].rstrip('\n')

        mask_num = 0
        valid_mask = 0

        print('	{.masks = {', end = '')

        num_table = [set() for x in range(9)]
        mask_table = [set() for x in range(12)]
        full_table = set()
        idx = 0;

        for comb in result[1].split(' '):
            mask = 0;
            for ch in comb:
                #print('ch=%c' % (ch));
                mask = mask | (0x1 << (int(ch)-1))

                num_table[int(ch)-1].add(idx)
                mask_table[mask_num].add(int(ch)-1)
                full_table.add(idx)
                idx += 1;

            valid_mask = valid_mask | mask

            #print('comb=%s, mask=0x%x' % (comb, mask))
            if mask_num == 0:
                print('0x%x' % (mask), end = '')
            else:
                print(', 0x%x' % (mask), end = '')

            mask_num += 1

        print('}, .constraints = {', end = '')

        for i in range(mask_num):
            mask = 0;

            my_set = full_table
            for j in mask_table[i]:
                my_set = my_set.difference(num_table[int(j)])

            #print('\nfull_table %s, mask_table %s, my_set %s' % (full_table, mask_table[i], my_set))

            for constraint in my_set:
                #print('con %d' % (int(constraint)))
                mask = mask | (0x1 << int(constraint))

            if i == 0:
                print('0x%x' % (mask), end = '')
            else:
                print(', 0x%x' % (mask), end = '')

        print('}, .mask_num = %d, .valid_mask = 0x%x}, /* %d */' % (mask_num, valid_mask, sum))

    return

if __name__ == '__main__':
    main()
