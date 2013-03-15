/*
 * bt_builder.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: jasonbraley
 */

#include <unistd.h>
#include <bt_builder.h>

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
	BtreeLeaf* leaf = node;
	retStatus = leaf->insertKey(newKey, NULL);

	/*
	 * Now check the status to see whether we need to do
	 * additional work.
	 */
	if (retStatus == LEAF_IS_FULL)
	{
		KeyId* parentId;
		BtreeNode* currentNode = searchNode;
		BtreeNode* leftPtr = NULL;
		BtreeNode* rightPtr = NULL;

		retStatus = splitNode(0, parentId, currentNode, leftPtr, rightPtr );
		if( retStatus == DONE )
		{
			BtreeIndex* parent = searchNode->get_parentPtr();
			if( parent == NULL )
			{
				parent = new BtreeIndex();
				root = parent;
				leftPtr->set_parentPtr(parent);
				rightPtr->set_parentPtr(parent);
			}

			retStatus = parent->insertKey(*parentId, 0, leftPtr, rightPtr);
			if( retStatus == INDEX_IS_FULL)
			{

			}

		}
		else
		{
			cout << "Something is borked" <<endl;
		}
	}

	return DONE;
}

Status BtreeBuilder::searchForLeafNode( KeyId key, BtreeNode* root, BtreeNode*& leaf )
{
	BtreeNode* searchNode = root;

	/* Search for the leaf node. */
	while (searchNode->get_type() != LEAF)
	{
		BtreeNode* temp = NULL;
		BtreeIndex* index = searchNode;
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

Status BtreeBuilder::splitNode( KeyId key, KeyId*& parentKey,
	BtreeNode* currentNode, BtreeNode*& leftPtr, BtreeNode*& rightPtr )
{
	BtreeNode* newNode = NULL;


	if( currentNode->get_type() == LEAF )
	{
		newNode = new BtreeLeaf();
		for( int i=0; i<MAX_NUM_KEYS/2; i++ )
		{
			int readKey = currentNode->getKey(i);
			BtreeNode* readPtr = currentNode->getPtr(i);


		}
	}
	else
	{
		newNode = new BtreeIndex();
	}

	newNode->set_parentPtr(currentNode->get_parentPtr());



	return OK;
}

BtreeScan* BtreeBuilder::openScan( KeyId lo_key, KeyId hi_key )
{
return OK;
}

Status BtreeBuilder::findStartPosition( BtreeScan* scanner, KeyId int1 )
{
return OK;
}

Status BtreeBuilder::destroyBtree()
{
return OK;
}
