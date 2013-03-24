#include <unistd.h>
#include "bt_errors.h"
#include "bt_node.h"
#include "bt_scan.h"

BtreeScan::BtreeScan()
{
	leaf = NULL;
	endKey = 0;
	pos = 0;
}

BtreeScan::~BtreeScan()
{
}

BtreeNode* BtreeScan::get_leaf()
{
	return leaf;
}

Status BtreeScan::set_leaf( BtreeNode* l )
{
	leaf = l;
	return OK;
}

int BtreeScan::get_pos()
{
	return pos;
}

Status BtreeScan::set_pos( int p )
{
	pos = p;
	return OK;
}

KeyId BtreeScan::get_endKey()
{
	return endKey;
}

Status BtreeScan::set_endKey( KeyId e )
{
	endKey = e;
	return OK;
}

Status BtreeScan::getNext(KeyId* key) {
  // If no more keys on this leaf, move right.
  if(get_pos() >= get_leaf()->get_keyCount()) {
    set_leaf(get_leaf()->getPtr(MAX_NUM_PTRS - 1));
    set_pos(0);
  }

  // If we ran out of keys...
  if(get_leaf() == NULL ||
     get_pos() > get_leaf()->get_keyCount() ||
     (get_endKey() && get_endKey() < (get_leaf()->getKey(get_pos()))))
    return NO_MORE_KEYS; // XXX is this the right code?

  *key = get_leaf()->getKey(get_pos());
  set_pos(get_pos()+1);
  return OK;
}
