/*
 * bt_node.cpp
 *
 *  Created on: Feb 18, 2013
 *      Author: jasonbraley
 */

#include <unistd.h>
#include "bt_node.h"
#include "bt_errors.h"

BtreeNode::BtreeNode()
{
	parentPtr = NULL;
	keyCount = 0;
	type = LEAF;
}

BtreeNode::~BtreeNode()
{
}

int BtreeNode::get_keyCount()
{
	return keyCount;
}

void BtreeNode::set_keyCount( int count )
{
	keyCount = count;
}

void BtreeNode::set_type( int newType )
{
	type = newType;
}

int BtreeNode::get_type()
{
	return type;
}

BtreeNode* BtreeNode::get_parentPtr()
{
	return parentPtr;
}

void BtreeNode::set_parentPtr( BtreeNode* pointer )
{
	parentPtr = pointer;
}





