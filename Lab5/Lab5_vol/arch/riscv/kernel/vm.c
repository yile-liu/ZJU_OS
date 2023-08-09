#include "vm.h"

#include "buddy.h"
#include "put.h"
#include "slub.h"

extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;

#define ValidBit(i) ((i)&1)
#define PPN(i) (((i) >> 10) & 0xfffffffffff)
#define offset (0xffffffe000000000 - 0x80000000)

void create_mapping(uint64_t *pgtbl, uint64_t va, uint64_t pa, uint64_t sz,
                    int perm)
{
    /* your code */
    static int page_cnt = 1;

    uint64_t *pgtbl2;
    uint64_t *pgtbl3;

    // int prev_vpn1 = 12345678;
    // int prev_vpn2 = 12345678;
    // int prev_vpn3 = 12345678;

    while (sz > 0)
    {
        int VPN1 = (va & 0x7FC0000000) >> 30;
        int VPN2 = (va & 0x003FE00000) >> 21;
        int VPN3 = (va & 0x00001FF000) >> 12;

        // if(prev_vpn1 != VPN1){
        //     printf("%d %d\n", prev_vpn1, VPN1);
        // }
        // if(prev_vpn2 != VPN2){
        //     printf("\t%d %d\n", prev_vpn2, VPN2);
        // }
        // if(prev_vpn3 != VPN3){
        //     printf("\t\t%d %d\n", prev_vpn3, VPN3);
        // }

        // prev_vpn1 = VPN1;
        // prev_vpn2 = VPN2;
        // prev_vpn3 = VPN3;

        uint64_t content = pgtbl[VPN1];
        if ((content & 0x1) == 0)
        {
            pgtbl2 = alloc_pages(1);
            pgtbl[VPN1] = 1 + (((uint64_t)pgtbl2 >> 12) << 10);
        }
        else
        {
            pgtbl2 = ((content >> 10) << 12);
        }

        content = pgtbl2[VPN2];
        if ((content & 0x1) == 0)
        {
            pgtbl3 = alloc_pages(1);
            pgtbl2[VPN2] = 1 + (((uint64_t)pgtbl3 >> 12) << 10);
        }
        else
        {
            pgtbl3 = ((content >> 10) << 12);
        }

        pgtbl3[VPN3] = 1 + (perm << 1) + ((pa >> 12) << 10);
        sz -= 0x1000;
        va += 0x1000;
        pa += 0x1000;
    }
    // puts("****create mapping out****\n");
}

void paging_init()
{
    init_buddy_system();
    // unsigned long long kernel_space = &_end - &text_start;
    // int plus_one = kernel_space % 0x1000 ? 1 : 0;
    // unsigned long long kernel_page_cnt = kernel_space / 0x1000 + plus_one;
    // alloc_pages(kernel_page_cnt);
    alloc_pages(32);

    void *pgtbl = alloc_pages(1);
    create_mapping(pgtbl, 0xffffffe000000000, 0x80000000, 0x1000000, 7);
    create_mapping(pgtbl, 0x80000000, 0x80000000, 0x1000000, 7);
    create_mapping(pgtbl, 0x10000000, 0x10000000, 0x1000, 0x3);
}
