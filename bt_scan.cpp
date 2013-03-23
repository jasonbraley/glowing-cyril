#include <unistd.h>
#include "bt_errors.h"
#include "bt_node.h"
#include "bt_scan.h"

BtreeScan::BtreeScan() {}

BtreeScan::~BtreeScan() {}

BtreeNode* BtreeScan::get_leaf() {
  return leaf;
}

Status BtreeScan::set_leaf(BtreeNode* l) {
  leaf = l;
  return OK;
}

int BtreeScan::get_pos() {
  return pos;
}

Status BtreeScan::set_pos(int p) {
  pos = p;
  return OK;
}

KeyId BtreeScan::get_endKey() {
  return endKey;
}

Status BtreeScan::set_endKey(KeyId e) {
  endKey = e;
  return OK;
}

Status BtreeScan::getNext(int *i) {
  return OK;
}
