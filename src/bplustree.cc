#include "bplustree.h"

bplustree::bplustree(BlockView &block) : block(block), rootOffset(0) {}

// 1: insert the key-offset pair to the tree
void bplustree::insert(float key, unsigned offset) {
  bplustreeNode node;        // temporary node
  block.loadAt(rootOffset);  // loads the root node

  while (!node.isLeaf) {
    unsigned i = 0;
    // continues as long as i is less than the total number of keys and
    // key is greater than or equal to the current key
    // increasing i by 1 (next key) after checking the current
    while (i < node.numKeys && key >= node.keys[i]) i++;
    block.loadAt(node.children[i]);
  }

  // when reaching a leaf node,
  // add the new key
  node.keys.push_back(key);
  node.children.push_back(offset);
  // num of keys +1
  node.numKeys++;
  block.saveAt(rootOffset);

  if (node.isFull()) {
    splitNode(node, rootOffset);
  }
}

// search for a key
bool bplustree::search(float key, unsigned &offset) {
  bplustreeNode node;
  block.loadAt(rootOffset);

  // Traverse to the leaf node
  while (!node.isLeaf) {
    unsigned i = 0;
    // if the search key is greater than or equal to the current key, moves to
    // the next key
    while (i < node.numKeys && key >= node.keys[i]) i++;
    block.loadAt(node.children[i]);
  }

  // iterates through all keys in the leaf node
  for (unsigned i = 0; i < node.numKeys; i++) {
    if (node.keys[i] == key) {
      offset = node.children[i];
      return true;
    }
  }

  return false;
}

void bplustree::splitNode(bplustreeNode &node, unsigned offset) {
  bplustreeNode newnode(node.isLeaf);  // create the new node
  unsigned mid = node.numKeys / 2;     // find the mid

  // move the second half of keys and children to the new node
  newnode.numKeys = node.numKeys - mid;  // number of keys in the second half
  for (unsigned i = 0; i < newnode.numKeys; i++) {
    // moves the second half to the new node (both keys and children)
    newnode.keys[i] = node.keys[mid + i];
    newnode.children[i] = node.children[mid + i];
  }
  newnode.children[newnode.numKeys] =
      node.children[node.numKeys];  // update the child's pointer

  node.numKeys = mid;  // update the key count of the original node (first half)

  unsigned newOffset = offset + 1;  // determine the offset fot the new node
  // writes the new node and the changed original node to the file (disk state)
  block.saveAt(newOffset);
  block.saveAt(offset);

  // to be continue: The root split case...
}

void bplustree::printTree(unsigned offset, int level) {
  // print tree???
}
