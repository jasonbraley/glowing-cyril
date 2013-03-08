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
	BtreeIndex* parent = NULL;
	BtreeLeaf* child = NULL;
	Status retStatus;

	child = root;
	while (child->get_type() != LEAF)
	{
		BtreeNode* temp = NULL;
		parent = child;
		parent->searchKey(newKey, 0, temp);
		child = temp;
	}

	retStatus = child->insertKey(newKey, 0);
	if (retStatus == LEAF_IS_FULL)
	{
		if (parent == NULL)
		{
			parent = new BtreeIndex();
		}
		parent->insertKey();
	}
	return OK;
}

Status BtreeBuilder::deleteBuilderKey( KeyId delKey )
{
	return OK;
}

Status BtreeBuilder::splitNode( KeyId key, KeyId*& parentKey,
		BtreeNode* currentNode, BtreeNode*& leftPtr, BtreeNode*& rightPtr )
{
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
