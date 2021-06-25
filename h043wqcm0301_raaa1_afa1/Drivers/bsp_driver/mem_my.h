#ifndef   _MEM__MY_H_
#define   _MEM__MY_H_

#include"stdint.h"
typedef struct memhead
{
	unsigned int size;
	struct memhead *next;
}memhead;


extern void init_memory(void *p, unsigned int size);
extern void * my_malloc(unsigned int size);
extern void my_free(void *p);
extern unsigned int  get_mem_info(unsigned int * pnum, unsigned int * pmax);




extern void mem_set(void * p, uint8_t value, uint32_t size);
extern void mem_copy(void * dst, void * src, uint32_t size);
extern void mem_set2(void * p, uint16_t value, uint32_t size);
extern void mem_copy4(void * dst, void * src, uint32_t size);
extern void mem_copy2(void * dst, void * src, uint32_t size);


#endif
