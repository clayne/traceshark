/*
 * Traceshark - a visualizer for visualizing ftrace traces
 * Copyright (C) 2015  Viktor Rosendahl <viktor.rosendahl@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STRINGPOOL_H
#define STRINGPOOL_H

#include <cstdint>
#include "mempool.h"
#include "src/traceshark.h"
#include "src/tstring.h"

using namespace TraceShark;

enum StringPoolColor { SP_RED, SP_BLACK };

class StringPoolEntry {
public:
	StringPoolEntry *small;
	StringPoolEntry *large;
	StringPoolEntry *parent;
	TString *str;
	int height;
	__always_inline void stealParent(StringPoolEntry *newChild,
					 StringPoolEntry **rootptr);
	__always_inline void setHeightFromChildren();
};

class StringPool
{
public:
	StringPool(unsigned int nr_pages = 256*10, unsigned int hSizeP = 256);
	~StringPool();
	__always_inline TString* allocString(const TString *str, uint32_t hval);
	void clear();
	void reset();
	__always_inline void SwapEntries(StringPoolEntry *a,
					 StringPoolEntry *b);
private:
	MemPool *strPool;
	MemPool *charPool;
	MemPool *entryPool;
	StringPoolEntry **hashTable;
	unsigned int *usageTable;
	unsigned int hSize;
	char *curPtr;
	void clearTable();
};

__always_inline TString* StringPool::allocString(const TString *str,
						 uint32_t hval)
{
	TString *newstr;
	StringPoolEntry **aentry;
	StringPoolEntry **rootptr;
	StringPoolEntry *entry;
	StringPoolEntry *parent = NULL;
	StringPoolEntry *sibling;
	StringPoolEntry *grandParent;
	StringPoolEntry *smallChild;
	StringPoolEntry *largeChild;
	int cmp;
	int diff;
	int smallH;
	int largeH;

	hval = hval % hSize;
	if (usageTable[hval] > 50) {
		newstr = (TString*) strPool->allocObj();
		if (newstr == NULL)
			return NULL;
		newstr->len = str->len;
		newstr->ptr = (char*) charPool->allocChars(str->len + 1);
		if (newstr->ptr == NULL)
			return NULL;
		strncpy(newstr->ptr, str->ptr, str->len + 1);
		return newstr;
	}

	aentry = hashTable + hval;
	rootptr = aentry;
iterate:
	entry = *aentry;
	if (entry == NULL) {
		usageTable[hval]++;
		newstr = (TString*) strPool->allocObj();
		if (newstr == NULL)
			return NULL;
		newstr->len = str->len;
		newstr->ptr = (char*) charPool->allocChars(str->len + 1);
		if (newstr->ptr == NULL)
			return NULL;
		strncpy(newstr->ptr, str->ptr, str->len + 1);
		entry = (StringPoolEntry*) entryPool->allocObj();
		if (entry == NULL)
			return NULL;
		bzero(entry, sizeof(StringPoolEntry));
		entry->str = newstr;
		*aentry = entry; /* aentry is equal to &parent->[large|small] */

		entry->parent = parent;
		entry->height = 0;
		if (parent == NULL)
			return newstr; /* Ok, this is the root node */
		if (parent->height > 0)
			return newstr; /* parent already has another node */
		parent->height = 1;
		grandParent = parent->parent;
		if (grandParent == NULL)
			return newstr; /* Ok, too shallow to have a violation */
		/* update heights and find offending node */
		do {
			smallH = grandParent->small == NULL ?
				-1 : grandParent->small->height;
			largeH = grandParent->large == NULL ?
				-1 : grandParent->large->height;
			diff = smallH - largeH;
			if (diff == 0)
				break;
			if (diff > 1)
				goto rebalanceSmall;
			if (diff < -1)
				goto rebalanceLarge;
			grandParent->height = parent->height + 1;
			entry = parent;
			parent = grandParent;
			grandParent = grandParent->parent;
		} while(grandParent != NULL);
		return newstr;
	rebalanceSmall:
		/* Do small rebalance here (case 1 and case 2) */
		if (entry == parent->small) {
			/* Case 1 */
			sibling = parent->large;

			grandParent->stealParent(parent, rootptr);
			parent->large = grandParent;
			grandParent->parent = parent;
			grandParent->small = sibling;
			grandParent->height--;
			if (sibling != NULL)
				sibling->parent = grandParent;
		} else {
			/* Case 2 */
			smallChild = entry->small;
			largeChild = entry->large;

			grandParent->stealParent(entry, rootptr);
			entry->small = parent;
			entry->large = grandParent;
			entry->height = grandParent->height;

			grandParent->parent = entry;
			grandParent->small = largeChild;
			grandParent->setHeightFromChildren(); // Fixme: faster

			parent->parent = entry;
			parent->large = smallChild;
			parent->setHeightFromChildren();

			if (largeChild != NULL)
				largeChild->parent = grandParent;
			if (smallChild != NULL)
				smallChild->parent = parent;
		}
		return newstr;
	rebalanceLarge:
		/* Do large rebalance here */
		if (entry == parent->small) {
			/* Case 3 */
			smallChild = entry->small;
			largeChild = entry->large;

			grandParent->stealParent(entry, rootptr);
			entry->small = grandParent;
			entry->large = parent;
			entry->height = grandParent->height;

			grandParent->parent = entry;
			grandParent->large = smallChild;
			grandParent->setHeightFromChildren(); // Fixme: faster

			parent->parent = entry;
			parent->small = largeChild;
			parent->setHeightFromChildren();

			if (largeChild != NULL)
				largeChild->parent = parent;
			if (smallChild != NULL)
				smallChild->parent = grandParent;
		} else {
			/* Case 4 */
			sibling = parent->small;

			grandParent->stealParent(parent, rootptr);
			parent->small = grandParent;
			grandParent->parent = parent;
			grandParent->large = sibling;
			grandParent->height--;
			if (sibling != NULL)
				sibling->parent = grandParent;
		}
		return newstr;
	}
	/* Using strncmp here would lose performance and we know that the
	 * strings are null terminated */
	cmp = strcmp(str->ptr, entry->str->ptr);
	if (cmp == 0)
		return entry->str;
	parent = entry;
	if (cmp < 0) {
		aentry = &entry->small;
		goto iterate;
	}
	/*  cmp must be > 0, since not 0 and not < 0 */
	aentry = &entry->large;
	goto iterate;
}

__always_inline void StringPool::SwapEntries(StringPoolEntry *a,
					     StringPoolEntry *b)
{
	TString *tmp;
	int height;
	tmp = a->str;
	height = a->height;
	a->str = b->str;
	a->height = b->height;
	b->str = tmp;
	b->height = height;
}

__always_inline void StringPoolEntry::stealParent(StringPoolEntry *newChild,
						  StringPoolEntry **rootptr)
{
	newChild->parent = parent;
	if (parent == NULL) {
		/* Oops this is a root node */
		*rootptr = newChild;
		return;
	}
	if (parent->small == this)
		parent->small = newChild;
	else
		parent->large = newChild;
}

__always_inline void StringPoolEntry::setHeightFromChildren()
{
	int lh;
	int rh;

	lh = (small != NULL) ? (small->height) : -1;
	rh = (large != NULL) ? (large->height) : -1;
	height = TSMAX(lh, rh) + 1;
}


#endif /* STRINGPOOL_H */
