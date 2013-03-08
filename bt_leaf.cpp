/*
 * bt_leaf.cpp
 *
 *  Created on: Feb 24, 2013
 *      Author: jasonbraley
 */

#include "bt_errors.h"

BtreeLeaf::~BtreeLeaf()
{
}

Status BtreeLeaf::insertKey( KeyId key, int value )
{
	int maxKey = get_keyCount();
	KeyId currentKey = key;
	int currentValue = value;

	if (maxKey == 0)
	{
		key[0] = currentKey;
		ptr[0] = value;
	}
	else
	{
		/*
		 * Loop through the current list of keys. If the key at location i in the key array is bigger than the currentKey,
		 * we've found the correct location to place currentKey. Copy it into this location and make what was originally in
		 * location i the new currentKey. Repeat the loop until we hit the end. The LTE sign here allows us to place our
		 * last key in the last open entry. We will not exceed the array because we'll split the leaf node when we get too big.
		 */
		for (int i = 0; i <= maxKey; i++)
		{
			if (key[i] > currentKey)
			{
				KeyId tempKey = key[i];
				BtreeNode* tempPtr = ptr[i];

				tempKey[i] = currentKey;
				tempPtr[i] = currentValue;

				currentKey = tempKey;
				currentValue = tempPtr;
			}
		}
	}

	set_keyCount(maxKey + 1);
	if( get_keyCount() >= MAX_NUM_KEYS )
	{
		return LEAF_IS_FULL;
	}
	else
	{
		return DONE;
	}
}

Status BtreeLeaf::deleteKey( KeyId key, int value )
{
	return DONE;
}

Status BtreeLeaf::searchKey( KeyId key, int value )
{
	return DONE;
}

