#include <unistd.h>
#include "bt_errors.h"
#include "bt_scan.h"

BtreeScan::BtreeScan() {}

BtreeScan::~BtreeScan() {}

Status BtreeScan::getNext(int *i) {
  return OK;
}
