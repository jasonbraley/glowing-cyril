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

Status BtreeBuilder::insertBuilderKey( KeyId newKey )
{
	BtreeNode* node = NULL;
	Status retStatus;

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

		retStatus = splitNode( newKey, parentId, currentNode, leftPtr, rightPtr );
		if( retStatus == OK )
		{
			BtreeIndex* indexPtr = (BtreeIndex*)leaf->get_parentPtr();
			if (indexPtr == NULL)
			{
				indexPtr = new BtreeIndex();
				leftPtr->set_parentPtr(indexPtr);
				rightPtr->set_parentPtr(indexPtr);
			}
			else
			{
				retStatus = indexPtr->insertKey(parentId,)
			}

		}
		else
		{
			cout << "Something is borked" <<endl;
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

	if( currentNode->get_type() == LEAF )
	{
		KeyId tempKeyData[MAX_NUM_KEYS+1]; 		//holds all of the key data that is in current node.
		BtreeNode* tempPtrData[MAX_NUM_PTRS+1]; 	// holds all of the ptr data that's in current node.

		// copy out all of the key data
		for(int i=0; i<MAX_NUM_KEYS; i++)
		{
			// insert the new key in its correct position in the mix.
			if( key < currentNode->getKey(i) )
			{
				tempKeyData[i] = key;
				tempPtrData[i] = NULL;
				i += 1;
			}

			tempKeyData[i] = currentNode->getKey(i);
			tempPtrData[i] = currentNode->getPtr(i);
		}

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
