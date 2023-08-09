#include "vm.h"
#include "put.h"

extern unsigned long long text_start;
extern unsigned long long rodata_start;
extern unsigned long long data_start;
extern unsigned long long _end;

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
            pgtbl2 = &_end + 0x1000 * page_cnt;
            page_cnt++;
            pgtbl[VPN1] = 1 + (((uint64_t)pgtbl2 >> 12) << 10);
        }else{
            pgtbl2 == ((content >> 10) << 12);
        }

        content = pgtbl2[VPN2];
        if ((content & 0x1) == 0)
        {
            pgtbl3 = &_end + 0x1000 * page_cnt;
            page_cnt++;
            pgtbl2[VPN2] = 1 + (((uint64_t)pgtbl3 >> 12) << 10);
        }else{
            pgtbl3 == ((content >> 10) << 12);
        }

        pgtbl3[VPN3] = 1 + (perm << 1) + ((pa >> 12) << 10);
        sz -= 0x1000;
        va += 0x1000;
        pa += 0x1000;
    }
    puts("****create mapping out****\n");
}

void try_mem_access(uint64_t *pgtbl, uint64_t vmem_addr){
    int VPN1 = (vmem_addr & 0x7FC0000000) >> 30;
    int VPN2 = (vmem_addr & 0x003FE00000) >> 21;
    int VPN3 = (vmem_addr & 0x00001FF000) >> 12;
    printf("%d\n", VPN1);
    printf("%d\n", VPN2);
    printf("%d\n", VPN3);

    printf("pgtbl[VPN1] = %d\n", pgtbl[VPN1]);
    printf("pgtbl2[VPN1] = %d\n", pgtbl[VPN1]);
    printf("pgtbl[VPN1] = %d\n", pgtbl[VPN1]);
}

void paging_init()
{
    /* your code */
    create_mapping(&_end, 0xffffffe000000000, 0x80000000, 0x1000000, 0x7);
    try_mem_access(&_end, 0xffffffe0000000d0);
    create_mapping(&_end, 0x80000000, 0x80000000, 0x1000000, 0x7);
    create_mapping(&_end, 0x10000000, 0x10000000, 0x1000, 0x3);

    create_mapping(&_end, (uint64_t)&text_start, (uint64_t)&text_start, (uint64_t)&rodata_start - (uint64_t)&text_start, 5);
    create_mapping(&_end, (uint64_t)&rodata_start, (uint64_t)&rodata_start, (uint64_t)&data_start - (uint64_t)&rodata_start, 1);
    create_mapping(&_end, (uint64_t)&data_start, (uint64_t)&data_start, 0x1000000 - ((uint64_t)&data_start - (uint64_t)&text_start), 3);
}