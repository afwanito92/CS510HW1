/*
 * sk_iterator_utils.h
 *
 *  Created on: Oct 2, 2015
 *      Author: solisknight
 */

#ifndef SK_ITERATOR_UTILS_H_
#define SK_ITERATOR_UTILS_H_

#include "sk_iterator/sk_const_iterator.h"

bool argv_const_begin(sk_const_iterator *dest, int argc, char **argv);

bool null_terminated_array_const_begin(sk_const_iterator *dest, void **source, UINT_64 size);

#endif /* SK_ITERATOR_UTILS_H_ */
