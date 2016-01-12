/*
 * sk_iterator_utils.c
 *
 *  Created on: Oct 2, 2015
 *      Author: solisknight
 */

#include "sk_iterator_utils.h"

#include "utils.h"

// UINT_64
#include "sk_types/sk_types.h"

bool argv_const_has_next(const sk_const_iterator *it);

const void *argv_const_next(sk_const_iterator *it);

void argv_const_destroy(void *p);

bool null_terminated_array_const_has_next(const sk_const_iterator *it);

const void *null_terminated_array_const_next(sk_const_iterator *it);

void null_terminated_array_const_destroy(void *p);

bool argv_const_begin(sk_const_iterator *dest, int argc, char **argv)
{
    if (!dest || !argv)
    {
        return false;
    }

    dest->collection = argv;
    dest->data = ALLOC(int, 1);
    *((int *)dest->data) = argc;

    dest->has_next = argv_const_has_next;
    dest->next = argv_const_next;
    dest->destroy = argv_const_destroy;
    return true;
}

bool null_terminated_array_const_begin(sk_const_iterator *dest, void **source, UINT_64 size)
{
    if (!dest || !source)
    {
        return false;
    }

    dest->collection = source;
    dest->data = ALLOC(UINT_64, 1);
    *((int *)dest->data) = size;

    dest->has_next = null_terminated_array_const_has_next;
    dest->next = null_terminated_array_const_next;
    dest->destroy = null_terminated_array_const_destroy;
    return true;
}



bool argv_const_has_next(const sk_const_iterator *it)
{
    return it && *((int *)it->data);
}

const void *argv_const_next(sk_const_iterator *it)
{
    if (!it || !argv_const_has_next(it))
    {
        return NULL;
    }

    const void *ret = *((char **)it->collection);
    it->collection = ((char **)it->collection) + 1;
    *((int *)it->data) = *((int *)it->data) - 1;

    return ret;
}

void argv_const_destroy(void *p)
{
    if (!p)
    {
        return;
    }

    free(((sk_const_iterator *)p)->data);
}

bool null_terminated_array_const_has_next(const sk_const_iterator *it)
{
    return it && it->collection && *((char **)it->collection);
}

const void *null_terminated_array_const_next(sk_const_iterator *it)
{
    if (!it || !null_terminated_array_const_has_next(it))
    {
        return NULL;
    }

    const void *ret = it->collection;
    it->collection = (void *)(((UINT_64)it->collection) + (*(UINT_64 *)it->data));

    return ret;
}

void null_terminated_array_const_destroy(void *p)
{
    if (!p)
    {
        return;
    }

    free(((sk_const_iterator *)p)->data);
}
