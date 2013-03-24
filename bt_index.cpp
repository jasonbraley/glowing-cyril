/*
 * bt_index.cpp
 *
 *  Created on: Feb 26, 2013
 *      Author: jasonbraley
 */

#include "bt_errors.h"
#include "bt_index.h"

Status BtreeIndex::insertKey( KeyId newKey, int keyCount, BtreeNode*& leftChild,
		BtreeNode*& rightChild )
{
	int numCurrentKeys = get_keyCount();
	KeyId currentKey = newKey;
	BtreeNode* currentValue = rightChild;
	Status retStatus = DONE;

	if (numCurrentKeys == MAX_NUM_KEYS)
	{
		retStatus = INDEX_IS_FULL;
	}
	else if (numCurrentKeys == 0)
	{
                setPtr(leftChild, 0);
                setPtr(rightChild, 1);
                setKey(0, newKey);

		/*
		 * Increment the current key count.
		 */
		set_keyCount(numCurrentKeys + 1);
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
		        if (getKey(i) > currentKey)
			{
			        KeyId tempKey = getKey(i);
				BtreeNode* tempPtr = getPtr(i+1);

			        setKey(i, currentKey);
				setPtr(currentValue, i+1);

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
		setKey(numCurrentKeys, currentKey);
		setPtr(currentValue, numCurrentKeys + 1);

		/*
		 * Increment the current key count.
		 */
		set_keyCount(numCurrentKeys + 1);
	}

	return (retStatus);
}

Status BtreeIndex::deleteKey( KeyId key, int int2 )
{
	return DONE;
}

Status BtreeIndex::searchKey( KeyId searchKey, int int2, BtreeNode*& childPtr )
{
	childPtr = NULL;

	for( int i=0; i<keyCount; i++ )
	{
		if( searchKey < key[i] )
		{
			childPtr = ptr[i];
			break;
		}
	}

	/*
	 * If we didn't take the less than path, return the
	 * far right ptr in the list.
	 */
	if( childPtr == NULL )
	{
		childPtr = ptr[keyCount];
	}

	return DONE;
}

