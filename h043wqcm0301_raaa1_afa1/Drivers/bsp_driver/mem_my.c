#include "mem_my.h"
#include "stdio.h"
#include "stdint.h"
#include "main.h"
#include "fatfs.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"
//#include "sys.h"

memhead * g_mem;

void init_memory(void *p, unsigned int size);
void * my_malloc(unsigned int size);
 void my_free(void *p);
 unsigned int  get_mem_info(unsigned int * pnum, unsigned int * pmax);
 void mem_set(void * p, uint8_t value, uint32_t size);
 void mem_copy(void * dst, void * src, uint32_t size);
 void mem_set2(void * p, uint16_t value, uint32_t size);
 void mem_copy4(void * dst, void * src, uint32_t size);
 void mem_copy2(void * dst, void * src, uint32_t size);

void init_memory(void *p, unsigned int size)
{
	memhead *h;
	h = (memhead *)p;
	h->size = size;
	h->next = NULL;
	g_mem = h;
}


static memhead *mem_break(memhead *p, unsigned int  size)
{
	memhead *d = (memhead *)((unsigned char *)p + size);

	d->size = p->size - size;
	d->next = p->next;
	p->size = size;
	return d;
}



static void * malloc_irq(unsigned int  size)
{
	memhead **pp, *p;

	if (size & 3)
	{
		size = ((size + 4) & 0xFFFFFFFC);
	}
	
	size += sizeof(unsigned int);

	if (size < sizeof(memhead))size = sizeof(memhead);
	//看看空内存块链表中是否有适合的可分配空间。

	for (pp = &g_mem; (p = *pp) != NULL; pp = &p->next)
	{
		if (size <= p->size)
		{
			*pp = (p->size >= size + sizeof(memhead)) ? mem_break(p, size) : p->next;
			return (&p->next);
		}
	}
	//M("my_malloc Error!");
	//D_Reset();

	return NULL;
}



void * my_malloc (unsigned int size)
{
	void *p;
	__set_PRIMASK(1);
	p = malloc_irq(size);
	__set_PRIMASK(0);
	return p;
}

static void mem_merge(memhead *p1, memhead *p2)
{
	if ((memhead *)((unsigned char *)p1 + p1->size) == p2)
	{
		p1->size += p2->size;
		p1->next = p2->next;
	}
	else
	{
		p1->next = p2;
	}
}



void free_irq(void *p)
{
	memhead *x, *y, *z;

	y = (memhead *)((unsigned char *)p - sizeof(unsigned int));
	if (!g_mem)
	{
		y->next = NULL;
		g_mem = y;
	}
	else if (y < g_mem)
	{
		mem_merge(y, g_mem);
		g_mem = y;
	}
	else
	{
		for (x = g_mem; (z = x->next) != NULL && z < y; x = z);

		if (z)
		{
			mem_merge(y, z);
		}
		else
		{
			y->next = NULL;
		}
		mem_merge(x, y);
	}
}



void my_free(void *p)
{
	__set_PRIMASK(1);
	free_irq(p);
	__set_PRIMASK(0);
	//T_EXIT_CRITICAL();
}


unsigned int  get_mem_info(unsigned int * pnum, unsigned int * pmax)
{
	unsigned int size = 0, num = 0, max = 0;
	memhead *p;
	//T_ENTER_CRITICAL();
	__set_PRIMASK(1);
	for (p = g_mem; p; p = p->next)
	{
		size += p->size;
		if (max < p->size)
		{
			max = p->size;
		}
		num++;
	}
	__set_PRIMASK(0);
	//T_EXIT_CRITICAL();
	*pnum = num;
	*pmax = max;
	return size;
}


void mem_set(void * p, uint8_t value, uint32_t size)
{
	uint32_t i;
	uint8_t *pt = (uint8_t *) p;
	for(i = 0;i<size;i++)
	{
		*pt++ = value;
	}
}

void mem_set2(void * p, uint16_t value, uint32_t size)
{
	uint32_t i;
	uint16_t * pt = (uint16_t *) p;
	
	for(i = 0;i<size;i++)
	{
		*pt++ = value;
	}
	
}

void mem_copy(void * dst, void * src, uint32_t size)
{
	uint8_t *pd = (uint8_t *) dst;
	uint8_t *ps = (uint8_t *) src;
	uint32_t i;
	for(i = 0;i < size; i++)
	{
		*pd++ = *ps++;
	}
}

void mem_copy2(void * dst, void * src, uint32_t size)
{
	uint16_t *pd = (uint16_t *) dst;
	uint16_t *ps = (uint16_t *) src;
	uint32_t i,n;
	n = size/2;
	for(i = 0;i < n; i++)
	{
		*(pd++) = *(ps++);
		
		
	}
}

void mem_copy4(void * dst, void * src, uint32_t size)
{
	uint32_t *pd = (uint32_t *) dst;
	uint32_t *ps = (uint32_t *) src;
	uint32_t i,n;
	n = size/4;
	for(i = 0;i < n; i++)
	{
		(*(pd+i)) = (*(ps+i));
	}
	
	
}
