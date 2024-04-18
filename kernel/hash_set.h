/*
Copyright (C) 2016-2019 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#ifndef HASH_SET_H
#define HASH_SET_H

#include "kernel/types.h"

struct hash_set {
	unsigned total_buckets;
	unsigned num_entries;
	struct hash_set_node **head;
};

struct hash_set_node {
	unsigned key;
	void *data;
	struct hash_set_node *next;
};

struct hash_set *hash_set_create(unsigned max_expected_keys);
void hash_set_delete(struct hash_set *set);

unsigned   hash_string(const char *string, unsigned range_min, unsigned range_max);

unsigned   hash_set_add(struct hash_set *set, unsigned key, void *data);
void *     hash_set_lookup(struct hash_set *set, unsigned key);
unsigned   hash_set_remove(struct hash_set *set, unsigned key);
unsigned   hash_set_entries(struct hash_set *set);
void       hash_set_print(struct hash_set *set);

#endif
