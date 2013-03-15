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
	int numCurrentKeys = get_keyCount();
	KeyId currentKey = key;
	int currentValue = value;
	Status retStatus = DONE;

	if (numCurrentKeys == 0)
	{
		key[0] = currentKey;
		ptr[0] = value;
	}
	else if( numCurrentKeys == MAX_NUM_KEYS )
	{
		retStatus = LEAF_IS_FULL;
	}
	else
	{
		/*
		 * Loop through the current list of keys. If the key at location i in the key array is bigger than the currentKey,
		 * we've found the correct location to place currentKey. Copy it into this location and make what was originally in
		 * location i the new currentKey. Repeat the loop until we hit the end. The LTE sign here allows us to place our
		 * last key in the last open entry. We will not exceed the array because we'll split the leaf node when we get too big.
		 */
		for (int i = 0; i < numCurrentKeys; i++)
		{
			if (key[i] > currentKey)
			{
				KeyId tempKey = key[i];
				BtreeNode* tempPtr = ptr[i];

				key[i] = currentKey;
				ptr[i] = currentValue;

				currentKey = tempKey;
				currentValue = tempPtr;
			}
		}

		/*
		 * Place the final key in the array. This may
		 * be the new one (if it's larger than everything currently
		 * in the node. Or it may be a leftover element that got shifted
		 * right by one.
		 */
		key[numCurrentKeys+1] = currentKey;
		ptr[numCurrentKeys+1] = currentKey;
	}

	return (retStatus);
}

Status BtreeLeaf::deleteKey( KeyId key, int value )
{
	return DONE;
}

Status BtreeLeaf::searchKey( KeyId key, int value )
{
	return DONE;
}

