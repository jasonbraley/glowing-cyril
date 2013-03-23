/*
 * bt_leaf.cpp
 *
 *  Created on: Feb 24, 2013
 *      Author: jasonbraley
 */

#include "bt_errors.h"
#include "bt_leaf.h"

Status BtreeLeaf::insertKey( KeyId newKey, int value )
{
	int numCurrentKeys = get_keyCount();
	KeyId currentKey = newKey;
	BtreeNode* currentValue = (BtreeNode*)value;
	Status retStatus = DONE;

	if( numCurrentKeys == MAX_NUM_KEYS )
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
				ptr[i] = (BtreeNode*)currentValue;

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
		key[numCurrentKeys] = currentKey;
		ptr[numCurrentKeys] = (BtreeNode*)currentValue;

		/*
		 * Increment the current key count.
		 */
		set_keyCount( numCurrentKeys + 1 );
	}


	return (retStatus);
}

Status BtreeLeaf::deleteKey( KeyId delKey, int value )
{
  int numCurrentKeys = get_keyCount();
  KeyId currentKey = delKey;
  BtreeNode* currentValue = (BtreeNode*) value;
  int i;

  for(i = 0; i < numCurrentKeys; ++i) {
    if(getKey(i) == currentKey && getPtr(i) == currentValue)
      break;
  }

  if(i == numCurrentKeys)
    return KEY_NOT_FOUND;
}

Status BtreeLeaf::searchKey( KeyId key, int value )
{
	return DONE;
}

