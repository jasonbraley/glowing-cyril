/*
 * bt_builder.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: jasonbraley
 */

#include <unistd.h>
#include "bt_builder.h"

BtreeBuilder::~BtreeBuilder()
{
	destroyBtree();
	delete root;
}

Status BtreeBuilder::insertBuilderKey( KeyId argKey )
{
	BtreeNode* node = NULL;
	Status retStatus;
	KeyId newKey = argKey;

	/* Given the root, look for the appropriate leaf node. */
	searchForLeafNode( newKey, root, node );

	/* We found the right leaf node. Insert the key. */
	BtreeLeaf* leaf = (BtreeLeaf*)node;
	retStatus = leaf->insertKey(newKey, 0);

	/*
	 * Now check the status to see whether we need to do
	 * additional work.
	 */
	if (retStatus == LEAF_IS_FULL)
	{
		KeyId parentId;
		BtreeNode* currentNode = leaf;
		BtreeNode* leftPtr = NULL;
		BtreeNode* rightPtr = NULL;
		bool done = false;

		/* Leaf was full. Split that node. */
		retStatus = splitNode( newKey, parentId, currentNode, leftPtr, rightPtr );
		if( retStatus != OK )
		{
			cout << "Something is borked" << endl;
			exit(4);
		}

		/* The new key was inserted when we split the node */
		newKey = parentId;
		currentNode = leftPtr->get_parentPtr();

		/*
		 * We need to add the new node to the tree. To that end
		 * we will insert it into the parent. If that makes the parent split
		 * we'll go through again.
		 */
		while( done == false )
		{
			BtreeIndex* indexPtr = (BtreeIndex*)currentNode;

			/* Create a new index if we've reached the top. */
			if (indexPtr == NULL)
			{
				indexPtr = new BtreeIndex();
				leftPtr->set_parentPtr(indexPtr);
				rightPtr->set_parentPtr(indexPtr);
			}

			/* We've got the currentParent. Try inserting it. If we succeed, we're done. */
			retStatus = indexPtr->insertKey(newKey, 0, leftPtr, rightPtr);
			if (retStatus == INDEX_IS_FULL)
			{
				retStatus = splitNode(newKey, parentId, indexPtr, leftPtr, rightPtr);
				if (retStatus != OK)
				{
					cout << "Something is borked" << endl;
					exit(3);
				}
				else
				{
					currentNode = indexPtr->get_parentPtr();
					newKey = parentId;
				}
			}
			else
			{
				done = true;
			}

		}
	}

	return OK;
}

/*
 * FUNCTION: 		BtreeBuilder::searchForLeafNode
 * DESCRIPTION: 	given a node, it searches for and returns the correct leaf
 */
Status BtreeBuilder::searchForLeafNode( KeyId key, BtreeNode* root, BtreeNode*& leaf )
{
	BtreeNode* searchNode = root;

	/* Search for the leaf node. */
	while (searchNode->get_type() != LEAF)
	{
		BtreeNode* temp = NULL;
		BtreeIndex* index = (BtreeIndex*)searchNode;
		index->searchKey(key, 0, temp);
		searchNode = temp;
	}

	leaf = searchNode;

	return (OK);
}

Status BtreeBuilder::deleteBuilderKey( KeyId delKey )
{
	return OK;
}

Status BtreeBuilder::splitNode( KeyId key, KeyId& parentKey, BtreeNode* currentNode, BtreeNode*& leftPtr, BtreeNode*& rightPtr )
{
	BtreeNode* newNode = NULL;
	const int maxTempKeySize = MAX_NUM_KEYS + 1;	// this will be big enough to contain all the key data in the original node plus the new one.
	const int maxTempPtrsSize = MAX_NUM_PTRS + 1; 	// this will be big enough to contain all the ptr data in the original node plus the new one.
	KeyId tempKeyData[maxTempKeySize]; 			//holds all of the key data that is in current node.
	BtreeNode* tempPtrData[maxTempPtrsSize]; // holds all of the ptr data that's in current node.

	// copy out all of the key data; there's one more ptr than there
	// is keys. Make sure we don't overrun the buffer.
	int tempIndex = 0;
	for (int i = 0; i < MAX_NUM_PTRS; i++, tempIndex++ )
	{
		// insert the new key in its correct position in the mix.
		if ( i < MAX_NUM_KEYS && key < currentNode->getKey(i))
		{
			tempKeyData[tempIndex] = key;
			tempPtrData[tempIndex] = rightPtr;
			tempIndex += 1;
		}

		if( i < MAX_NUM_KEYS )
		{
			tempKeyData[tempIndex] = currentNode->getKey(i);
		}

		tempPtrData[tempIndex] = currentNode->getPtr(i);
	}

	if( currentNode->get_type() == LEAF )
	{
		// create the new node
		newNode = new BtreeLeaf();
		newNode->set_parentPtr( currentNode->get_parentPtr() );
		newNode->setPtr( currentNode->getPtr(MAX_NUM_PTRS-1), MAX_NUM_PTRS-1);

		// wipe the old node back to zero keys and chain it with the new.
		currentNode->set_keyCount(0);
		currentNode->setPtr(newNode, MAX_NUM_PTRS-1);

		// figure out which one is the middle key.
		int middleKeyIndex = (MAX_NUM_KEYS+1)/2;
		KeyId middleKeyValue = currentNode->getKey(middleKeyIndex);

		/* Distribute the data amidst the two nodes */
		for( int i=0; i<MAX_NUM_KEYS+1; i++ )
		{
			BtreeLeaf* leafNode = NULL;
			if( i <= middleKeyIndex )
			{
				leafNode = (BtreeLeaf*)currentNode;
			}
			else if ( i > middleKeyIndex )
			{
				leafNode = (BtreeLeaf*)newNode;
			}

			if( leafNode != NULL )
			{
				Status sts = leafNode->insertKey(tempKeyData[i], 0 );
				if( sts != OK )
				{
					cout << "Something went horribly awry" << endl;
					exit(2);
				}
			}
		}

		parentKey = newNode->getKey(0);
		leftPtr = (BtreeNode*)currentNode;
		rightPtr = (BtreeNode*)newNode;
	}
	else
	{
		// this is the logic for the index
		newNode = new BtreeIndex();
		newNode->set_parentPtr( currentNode->get_parentPtr() );

		// wipe the old node back to zero keys
		currentNode->set_keyCount(0);

		// figure out which one is the middle key.
		int middlePtrIndex = maxTempPtrsSize / 2;

		/* Distribute the data amidst the two nodes */
		for (int i = 0; i < MAX_NUM_KEYS + 1; i++)
		{
			BtreeIndex* indexNode = NULL;
			if (i <= middlePtrIndex)
			{
				indexNode = (BtreeIndex*) currentNode;
			}
			else if (i > middlePtrIndex)
			{
				indexNode = (BtreeLeaf*) newNode;
			}

			if (indexNode != NULL)
			{
				Status sts = indexNode->insertKey(tempKeyData[i], 0);
				if (sts != OK)
				{
					cout << "Something went horribly awry" << endl;
					exit(2);
				}
			}



	}

	return OK;
}

BtreeScan* BtreeBuilder::openScan( KeyId lo_key, KeyId hi_key )
{
	return (new BtreeScan());
}

Status BtreeBuilder::findStartPosition( BtreeScan* scanner, KeyId int1 )
{
return OK;
}

Status BtreeBuilder::destroyBtree()
{
return OK;
}
