#ifndef EXEC_BAVLTREE_H
#define EXEC_BAVLTREE_H

#include <Exec/BBase.h>

/**
 * BAvlTree 
 *
 * Implements a general purpose AVL (balanced binary tree) for fast lookups.  Is something of an alternative to 
 * BList.
 *
 * Inherit nodes from BAvlNode.  The mKey value must be unique and is the value you use to reference nodes in the tree.
 */

class BAvlNode : BBase {
public:
  BAvlNode(TInt64 aKey = 0);

public:
  BAvlNode *mLeft;
  BAvlNode *mRight;

  TInt64 mKey;
  int mHeight;

public:
  BAvlNode *RightRotate();
  BAvlNode *LeftRotate();
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BAvlTree : public BBase {
public:
  BAvlTree();

public:
  BAvlNode *mRoot;
  int mSize;

protected:
  BAvlNode *Insert(BAvlNode *aNewMode, BAvlNode *aRoot);

public:
  BAvlNode *Insert(BAvlNode *aNewNode) {
    return Insert(aNewNode, mRoot);
  }

protected:
  BAvlNode *getMinNode(BAvlNode *aRoot);

protected:
  BAvlNode *Remove(TInt64 aKey, BAvlNode *aRoot);

public:
  BAvlNode *Remove(TInt64 aKey) {
    return Remove(aKey, mRoot);
  }
  BAvlNode *Update(TInt64 keyOld, TInt64 keyNew, BAvlNode *aRoot);
  // Find returns the BAvlNode with given value, if found, or ENull
protected:
  BAvlNode *Find(TInt64 aKey, BAvlNode *aRoot);

public:
  BAvlNode *Find(TInt64 aKey) { return Find(aKey, mRoot); }
};

#endif
