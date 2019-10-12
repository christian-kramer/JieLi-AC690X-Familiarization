#ifndef LIST_H
#define LIST_H

#include "typedef.h"

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
 #ifdef offsetof
 #undef offsetof
 #endif
 #ifdef container_of
 #undef container_of
 #endif

 #define list_offsetof(type, memb) \
	((unsigned long)(&((type *)0)->memb))

 #define container_of(ptr, type, memb) \
	((type *)((char *)ptr - list_offsetof(type, memb)))


struct list_head {
	struct list_head *next, *prev;
};


#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)


/**
	 * list_first_entry - get the first element from a list
	 * @ptr:	the list head to take the element from.
	 * @type:	the type of the struct this is embedded in.
	 * @member: the name of the list_struct within the struct.
	 *
	 * Note, that list is expected to be not empty.
	 */
/* REF_LIST: spi.c */
#define list_first_entry(ptr, type, member) \
		list_entry((ptr)->next, type, member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next;  pos != (head); \
        	pos = pos->next)

/**
 * list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop counter.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
 #define list_for_each_entry(pos, head, member)				\
 for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

/**
	 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
	 * @pos:	the type * to use as a loop cursor.
	 * @n:		another type * to use as temporary storage
	 * @head:	the head for your list.
	 * @member: the name of the list_struct within the struct.
	 */
#define list_for_each_entry_safe(pos, n, head, member)			\
		for (pos = list_entry((head)->next, typeof(*pos), member),	\
			n = list_entry(pos->member.next, typeof(*pos), member); \
			 &pos->member != (head);					\
			 pos = n, n = list_entry(n->member.next, typeof(*n), member))


/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
/* REF_LIST: spi.c */
_INLINE_
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
_INLINE_
static inline void __list_add(struct list_head *_new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
/* REF_LIST: spi.c */
_INLINE_
static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}

_INLINE_
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    if(prev==0 || next==0)
    {
        return;
    }
    //ASSERT(prev!=NULL || next!=NULL)
	next->prev = prev;
	prev->next = next;
}

_INLINE_
static inline void __list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/* REF_LIST: spi.c */
_INLINE_
static inline void list_del(struct list_head *entry) //修改过的list_del，这里与list_del_init一样
{
	__list_del(entry->prev, entry->next);
	entry->next = entry;
	entry->prev = entry;
}

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
_INLINE_
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

_INLINE_
static inline void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}
/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
_INLINE_
static inline void list_move_tail(struct list_head *list,
				  struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
_INLINE_
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

#if 0
_INLINE_
static inline void __list_splice(const struct list_head *list,
				 struct list_head *prev,
				 struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
_INLINE_
static inline void list_splice_tail_init(struct list_head *list,
					 struct list_head *head)
{
	if (!list_empty(list)) {
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
_INLINE_
static inline int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
_INLINE_
static inline void list_splice_tail(struct list_head *list,
				struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}
#endif

#endif

