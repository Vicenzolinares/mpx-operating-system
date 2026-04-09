#include <lib_mem.h>

MCB_list flist = { .head = NULL, .tail = NULL };
MCB_list *free_list = &flist;
MCB_list alist = { .head = NULL, .tail = NULL };
MCB_list *allocated_list = &alist;

static void list_remove(MCB_list *list, MCB *node)
{
	if (node->prev != NULL)
	{
		node->prev->next = node->next;
	}
	else
	{
		list->head = node->next;
	}

	if (node->next != NULL)
	{
		node->next->prev = node->prev;
	}
	else
	{
		list->tail = node->prev;
	}

	node->next = NULL;
	node->prev = NULL;
}

static void list_add_tail(MCB_list *list, MCB *node)
{
	node->next = NULL;
	node->prev = list->tail;

	if (list->tail != NULL)
	{
		list->tail->next = node;
	}
	else
	{
		list->head = node;
	}

	list->tail = node;
}

static void coalesce_with_neighbors(MCB *block)
{
	for (;;)
	{
		MCB *succ = NULL;

		for (MCB *n = free_list->head; n != NULL; n = n->next)
		{
			if ((void *)n == (void *)((char *)block->start_address + block->size))
			{
				succ = n;
				break;
			}
		}

		if (succ != NULL)
		{
			block->size += sizeof(MCB) + succ->size;
			list_remove(free_list, succ);
			continue;
		}

		MCB *pred = NULL;

		for (MCB *n = free_list->head; n != NULL; n = n->next)
		{
			if ((void *)block == (void *)((char *)n->start_address + n->size))
			{
				pred = n;
				break;
			}
		}

		if (pred != NULL)
		{
			pred->size += sizeof(MCB) + block->size;
			list_remove(free_list, block);
			block = pred;
			continue;
		}

		break;
	}
}

void *initialize_heap(size_t size)
{
	void *heap = kmalloc(size + sizeof(MCB), 0, NULL);

	if (heap == NULL)
	{
		return NULL;
	}

	MCB *initial_block = (MCB *)heap;
	initial_block->start_address = (void *)((char *)heap + sizeof(MCB));
	initial_block->size = size;
	initial_block->next = NULL;
	initial_block->prev = NULL;

	free_list->head = initial_block;
	free_list->tail = initial_block;
	allocated_list->head = NULL;
	allocated_list->tail = NULL;

	return heap;
}

void *allocate_memory(size_t size)
{
	if (size == 0)
	{
		return NULL;
	}

	for (MCB *cur = free_list->head; cur != NULL; cur = cur->next)
	{
		if (cur->size < size)
		{
			continue;
		}

		list_remove(free_list, cur);

		if (cur->size >= size + sizeof(MCB))
		{
			MCB *rest = (MCB *)((char *)cur->start_address + size);
			rest->start_address = (void *)((char *)rest + sizeof(MCB));
			rest->size = cur->size - size - sizeof(MCB);
			rest->next = NULL;
			rest->prev = NULL;
			cur->size = size;
			list_add_tail(free_list, rest);
		}

		list_add_tail(allocated_list, cur);
		return cur->start_address;
	}

	return NULL;
}

int free_memory(void *start_address)
{
	if (start_address == NULL)
	{
		return -1;
	}

	for (MCB *cur = allocated_list->head; cur != NULL; cur = cur->next)
	{
		if (cur->start_address != start_address)
		{
			continue;
		}

		list_remove(allocated_list, cur);
		list_add_tail(free_list, cur);
		coalesce_with_neighbors(cur);
		return 0;
	}

	return -1;
}
