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

class BAvlTree : public BBase {
public:
  BAvlTree();
public:
  BAvlNode *root;
  int mSize;

protected:
  BAvlNode *Insert(BAvlNode *newNode, BAvlNode *root);
public:
  BAvlNode *Insert(BAvlNode *aNewNode) {
    return Insert(newNode, root);
  }

  BAvlNode *getMinNode(BAvlNode *root);
protected:
  BAvlNode *Remove(TInt64 key, BAvlNode *root);
public:
  BAvlNode *Remove(TInt64 key) {
    return Remove(key, root);
  }
  BAvlNode *Update(TInt64 keyOld, TInt64 keyNew, BAvlNode *root);
  // Find returns the BAvlNode with given value, if found, or ENull
protected:
  BAvlNode *Find(TInt64 aValue, BAvlNode *aRoot);
public:
  BAvlNode *Find(TInt64 aValue) { return Find(aValue, root); }
};

#endif
