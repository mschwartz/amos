#ifndef EXEC_BAVLTREE_H
#define EXEC_BAVLTREE_H

#include <Exec/BBase.h>

class BAvlNode : BBase {
public:
  BAvlNode(TInt64 aData = 0);

public:
  BAvlNode *left;
  BAvlNode *right;

  TInt64 mData;
  int height;

public:
  BAvlNode *rightRotate();
  BAvlNode *leftRotate();
};

class BAvlTree : BBase {
public:
  BAvlTree();
public:
  BAvlNode *root;
  int size;

  BAvlNode *Insert(BAvlNode *newNode, BAvlNode *root);
  BAvlNode *getMinNode(BAvlNode *root);
  BAvlNode *Remove(TInt64 key, BAvlNode *root);
  BAvlNode *Update(TInt64 keyOld, TInt64 keyNew, BAvlNode *root);
  // Find returns the BAvlNode with given value, if found, or ENull
  BAvlNode *Find(TInt64 aValue, BAvlNode *aRoot);
  BAvlNode *Find(TInt64 aValue) { return Find(aValue, root); }
};

#endif
