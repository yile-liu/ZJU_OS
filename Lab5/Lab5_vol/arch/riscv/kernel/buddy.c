#include "buddy.h"

extern text_start;

buddy my_buddy;
unsigned MAX_BITMAP[8192];

void InitBitMap(int ind, int size)
{
    my_buddy.bitmap[ind] = size;
    MAX_BITMAP[ind] = size;
    if (ind < 4096)
    {
        InitBitMap(2 * ind, size / 2);
        InitBitMap(2 * ind + 1, size / 2);
    }
}

void init_buddy_system(void)
{
    my_buddy.size = 0;
    InitBitMap(1, 4096);
}

int FindFitPlace(int ind, int size)
{
    if (size == 0)
    {
        exit(2);
    }

    int res = -1;
    if (ind >= 4096)
    {
        if (my_buddy.bitmap[ind] != 0)
        {
            res = ind;
        }
        else
        {
            exit(3);
        }
    }
    else
    {
        if (my_buddy.bitmap[2 * ind] >= size)
        {
            res = FindFitPlace(2 * ind, size);
        }
        else if (my_buddy.bitmap[2 * ind + 1] >= size)
        {
            res = FindFitPlace(2 * ind + 1, size);
        }
        else if (my_buddy.bitmap[ind] == size)
        {
            res = ind;
        }
        else
        {
            exit(5);
        }
    }
    return res;
}

void UpdBitmapAlloc(int ind)
{
    my_buddy.bitmap[ind] = 0;
    ind /= 2;
    int left_child_size, right_child_size, max_child_size;
    while (ind > 0)
    {
        left_child_size = my_buddy.bitmap[ind * 2];
        right_child_size = my_buddy.bitmap[ind * 2 + 1];
        max_child_size = left_child_size > right_child_size ? left_child_size : right_child_size;
        my_buddy.bitmap[ind] = max_child_size;
        ind /= 2;
    }
}

void *alloc_pages(int size_req)
{

    // 查找合适的最小连续空间
    int size_fit = 1;
    while (size_fit < size_req)
    {
        size_fit *= 2;
    }
    // 维护size
    my_buddy.size += size_fit;

    int ind_in_bitmap = FindFitPlace(1, size_fit);
    if (ind_in_bitmap == -1)
    {
        exit(1);
    }
    // 更新bitmap信息
    UpdBitmapAlloc(ind_in_bitmap);
    // 现在拿到了index，怎么转换成虚拟地址？
    while (ind_in_bitmap < 4096)
    {
        ind_in_bitmap *= 2;
    }
    ind_in_bitmap -= 4096;
    // TODO:Untested
    unsigned long long res = (unsigned long long)&text_start;
    // unsigned long long res = 0x80000000;
    res += ind_in_bitmap * 0x1000; // PAGE_SIZE=4096
    // memset(res, 0, 4096);
    return res;
}

void UpdBitmapFree(int ind_to_upd)
{
    if (ind_to_upd == 0)
    {
        exit(3);
    }

    if (ind_to_upd * 2 >= 8192)
    {
        my_buddy.bitmap[ind_to_upd] = 1;
    }
    else
    {
        int left_child_size = my_buddy.bitmap[ind_to_upd * 2];
        int left_child_max = (left_child_size == MAX_BITMAP[ind_to_upd * 2]);
        int right_child_size = my_buddy.bitmap[ind_to_upd * 2 + 1];
        int right_child_max = (right_child_size == MAX_BITMAP[ind_to_upd * 2 + 1]);

        if (left_child_max && right_child_max)
        {
            my_buddy.bitmap[ind_to_upd] = MAX_BITMAP[ind_to_upd];
        }
        else
        {
            my_buddy.bitmap[ind_to_upd] =
                left_child_size > right_child_size ? left_child_size : right_child_size;
        }
    }

    if (ind_to_upd > 1)
    {
        UpdBitmapFree(ind_to_upd / 2);
    }
}

void free_pages(void *ptr)
{
    // 拿到了地址，怎么转换成bitmap下标？
    // TODO:
    unsigned long long offset = ptr - (unsigned long long)&text_start;
    int ind_to_upd = offset >> 12;

    // 更新bitmap
    UpdBitmapFree(ind_to_upd + 4096);
}