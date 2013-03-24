/*
 * bt_builder.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: jasonbraley
 */

#include <assert.h>
#include <unistd.h>
#include "bt_builder.h"

static int check_tree( BtreeNode* node, KeyId low, KeyId high )
{
	int i;
	KeyId previous;

	/* Check keys to see if they meet the bounds set above */
	previous = low;
	for (i = 0; i < node->get_keyCount(); ++i)
	{
		if (node->getKey(i) < previous || node->getKey(i) >= high)
			assert(0);

		previous = node->getKey(i);
	}

	/* If this isn't a leaf, arrange to check children. */
	if (node->get_type() == INDEX)
	{
		/* Check each child pointer's parent pointer to ensure they point to us. */
		for (i = 0; i < node->get_keyCount() + 1; ++i)
			assert(node->getPtr(i)->get_parentPtr() == node);

		/* Recursively check children. */
		previous = low;
		for (i = 0; i < node->get_keyCount(); ++i)
		{
			check_tree(node->getPtr(i), previous, node->getKey(i));
			previous = node->getKey(i);
		}

		check_tree(node->getPtr(i), previous, high);
	}

	return 0;
}

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
	searchForLeafNode(newKey, root, node);

	/* We found the right leaf node. Insert the key. */
	BtreeLeaf* leaf = (BtreeLeaf*) node;
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
		retStatus = splitNode(newKey, parentId, currentNode, leftPtr, rightPtr);
		if (retStatus != OK)
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
		while (done == false)
		{
			BtreeIndex* indexPtr = (BtreeIndex*) currentNode;

			/* Create a new index if we've reached the top. */
			if (indexPtr == NULL)
			{
				indexPtr = new BtreeIndex();
				root = (BtreeNode*) indexPtr;
				leftPtr->set_parentPtr(indexPtr);
				rightPtr->set_parentPtr(indexPtr);
			}

			/* We've got the currentParent. Try inserting it. If we succeed, we're done. */
			retStatus = indexPtr->insertKey(newKey, 0, leftPtr, rightPtr);
			if (retStatus == INDEX_IS_FULL)
			{
				retStatus = splitNode(newKey, parentId, indexPtr, leftPtr,
						rightPtr);
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
Status BtreeBuilder::searchForLeafNode( KeyId key, BtreeNode* root,
		BtreeNode*& leaf )
{
	BtreeNode* searchNode = root;

	/* Search for the leaf node. */
	while (searchNode->get_type() != LEAF)
	{
		BtreeNode* temp = NULL;
		BtreeIndex* index = (BtreeIndex*) searchNode;
		index->searchKey(key, 0, temp);
		searchNode = temp;
	}

	leaf = searchNode;

	return (OK);
}

Status BtreeBuilder::deleteBuilderKey( KeyId delKey )
{
	BtreeNode* node = NULL;

	searchForLeafNode(delKey, root, node);

	return node->deleteKey(delKey, 0);
}

/*
 * FUNCTION: 	BtreeBuilder::splitNode
 * DESCRIPTION: This function splits a node into two nodes
 * ARGS: 		key <- KeyId: this is the key that we were trying to insert when we split the node.
 * 				parentKey <- KeyId: key that should be inserted into the parent
 * 				currentNode <- BtreeNode*: the node to split
 * 				leftPtr <- the left node we were trying to insert when we split the node.
 * 				rightPtr <- the right node we were trying to insert when we split the node.
 * RETURN: 		parentKey is returned by reference
 * 				leftPtr is updated with the address of the original node
 * 				rightPtr is updated with the address of the new node created.
 */
Status BtreeBuilder::splitNode( KeyId newKey, KeyId& parentKey,
		BtreeNode* currentNode, BtreeNode*& leftPtr, BtreeNode*& rightPtr )
{
	BtreeNode* newNode = NULL;
	KeyId tempKeyData[MAX_NUM_KEYS + 1]; //holds all of the key data that is in current node.
	BtreeNode* tempPtrData[MAX_NUM_PTRS + 1]; // holds all of the ptr data that's in current node.

	// copy out all of the key data; there's one more ptr than there
	// is keys. Make sure we don't overrun the buffer.
	int tempIndex = 0;
	int i;
	int has_inserted = 0;

	for (i = 0; i < MAX_NUM_KEYS; i++, tempIndex++)
	{
		/*
		 * For the first index, grab the first pointer
		 */
		if (i == 0)
		{
			tempPtrData[i] = currentNode->getPtr(i);
			currentNode->setPtr(0, i);
		}

		/*
		 * Check to see whether we should insert here. If so,
		 * we copy in the key and advance our array entry counter.
		 */
		if (newKey < currentNode->getKey(i) && !has_inserted)
		{
			tempKeyData[tempIndex] = newKey;
			tempPtrData[tempIndex + 1] = rightPtr;
			tempIndex += 1;
			has_inserted = 1;
		}

		/*
		 * Finally, copy over the actual things that are at this
		 * location.
		 */
		tempKeyData[tempIndex] = currentNode->getKey(i);
		tempPtrData[tempIndex + 1] = currentNode->getPtr(i + 1);

		currentNode->setKey(i, 0);
		currentNode->setPtr(0, i+1);
	}

	/*
	 * If we didn't insert, do it now.
	 */
	if (i == tempIndex)
	{
		tempKeyData[MAX_NUM_KEYS] = newKey;
		tempPtrData[MAX_NUM_PTRS] = rightPtr;
	}

	if (currentNode->get_type() == LEAF)
	{
		// create the new node
		newNode = new BtreeLeaf();
		newNode->set_parentPtr(currentNode->get_parentPtr());
		newNode->setPtr(currentNode->getPtr(MAX_NUM_PTRS - 1),
				MAX_NUM_PTRS - 1);

		// wipe the old node back to zero keys and chain it with the new.
		currentNode->set_keyCount(0);
		currentNode->setPtr(newNode, MAX_NUM_PTRS - 1);

		// figure out which one is the middle key.
		int middleKeyIndex = (MAX_NUM_KEYS + 1) / 2;

		/* Distribute the data amidst the two nodes */
		for (int i = 0; i < MAX_NUM_KEYS + 1; i++)
		{
			BtreeLeaf* leafNode = NULL;
			if (i <= middleKeyIndex)
			{
				leafNode = (BtreeLeaf*) currentNode;
			}
			else if (i > middleKeyIndex)
			{
				leafNode = (BtreeLeaf*) newNode;
			}

			if (leafNode != NULL)
			{
				Status sts = leafNode->insertKey(tempKeyData[i], 0);
				if (sts != DONE)
				{
					cout << "Something went horribly awry" << endl;
					exit(2);
				}
			}
		}

		parentKey = newNode->getKey(0);
		leftPtr = (BtreeNode*) currentNode;
		rightPtr = (BtreeNode*) newNode;
	}
	else
	{
		// this is the logic for the index
		newNode = new BtreeIndex();
		newNode->set_parentPtr(currentNode->get_parentPtr());

		// wipe the old node back to zero keys
		currentNode->set_keyCount(0);

		// figure out which one is the middle ptr.
		int middlePtrIndex = (MAX_NUM_PTRS + 1) / 2;
		tempIndex = 0;
		/* Distribute the data amidst the two nodes */
		for (int i = 0; i < MAX_NUM_PTRS + 1; i++, tempIndex++)
		{
			BtreeIndex* indexNode = NULL;
			if (i < middlePtrIndex)
			{
				indexNode = (BtreeIndex*) currentNode;
				indexNode->setPtr(tempPtrData[i], tempIndex);
				indexNode->setKey(tempIndex, tempKeyData[i]);
				indexNode->set_keyCount(indexNode->get_keyCount() + 1);
			}
			else if (i == middlePtrIndex)
			{
				indexNode = (BtreeIndex*) currentNode;
				indexNode->setPtr(tempPtrData[i], tempIndex);
				parentKey = tempKeyData[i];
				tempIndex = -1;
			}
			else if (i > middlePtrIndex)
			{
				indexNode = (BtreeIndex*) newNode;
				indexNode->setPtr(tempPtrData[i], tempIndex);

				if (i < MAX_NUM_KEYS + 1)
				{
					indexNode->setKey(tempIndex, tempKeyData[i]);
					indexNode->set_keyCount(indexNode->get_keyCount() + 1);
				}

				indexNode->getPtr(tempIndex)->set_parentPtr(indexNode);
			}
		}

		leftPtr = currentNode;
		rightPtr = newNode;
	}

	return OK;
}

BtreeScan* BtreeBuilder::openScan( KeyId lo_key, KeyId hi_key )
{
	BtreeScan *scan = new BtreeScan();
	BtreeNode* leaf = NULL;

	searchForLeafNode( lo_key, root, leaf );

	scan->set_leaf(leaf);
	scan->set_endKey(hi_key);

	return (scan);
}

Status BtreeBuilder::findStartPosition( BtreeScan* scanner, KeyId int1 )
{
	return OK;
}

Status BtreeBuilder::destroyBtree()
{
	return OK;
}
