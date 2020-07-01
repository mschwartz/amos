#include <Types/BAvlTree.h>

static int getHeight(BAvlNode *aRoot) {
  if (aRoot == ENull) { //height of leaf = 0
    return 0;
  }
  return aRoot->mHeight;
}

static int balanceFactor(BAvlNode *aRoot) {
  if (aRoot == ENull) { //balanceFactor of leaf = 0
    return 0;
  }
  return (getHeight(aRoot->mLeft) - getHeight(aRoot->mRight));
}

static void printGraphviz(BAvlNode *aRoot, int aSize) {
  if (aSize > 1) {
    if (aRoot) {
      if (aRoot->mLeft) {
        // cout << '"' << root->data << '"';
        // cout << " -> ";
        // cout << '"' << root->left->data << '"';
        // cout << endl;
        printGraphviz(aRoot->mLeft, aSize);
      }
      if (aRoot->mRight) {
        // cout << '"' << root->data << '"';
        // cout << " -> ";
        // cout << '"' << root->right->data << '"';
        // cout << endl;
        printGraphviz(aRoot->mRight, aSize);
      }
    }
  }
  else {
  // cout << '"' << root->data << '"';
  }
}

#if 0  
TBool searchNode(int key, BAvlNode *aRoot) {
  if (aRoot == ENull) {
    return EFalse;
  }
  if (aRoot->mKey == key) {
    return ETrue;
  }
  else {
    if (key < aRoot->mKey) {
      return searchNode(key, aRoot->mLeft);
    }
    else {
      return searchNode(key, aRoot->right);
    }
  }

  return EFalse;
}
#endif  

BAvlNode *BAvlTree::Find(TInt64 aKey, BAvlNode *aRoot) {
  if (aRoot == ENull) {
    return ENull;
  }
  if (aRoot->mKey == aKey) {
    return aRoot;
  }
  else {
    if (aKey < aRoot->mKey) {
      return Find(aKey, aRoot->mLeft);
    }
    else {
      return Find(aKey, aRoot->mRight);
    }
  }
  return ENull;

}  

BAvlNode::BAvlNode(TInt64 aKey) {
  this->mKey = aKey;
  this->mLeft = ENull;
  this->mRight = ENull;
  this->mHeight = 1;
}

BAvlNode *BAvlNode::RightRotate() {

  /*
           this                         x
           / \      right-rotate       / \
          x  (T3)   ----------->    (T1) this
         / \                              / \
      (T1) (T2)                        (T2) (T3)
*/

  BAvlNode *x = this->mLeft;
  BAvlNode *T2 = x->mRight;
  x->mRight = this;
  this->mLeft = T2;

  this->mHeight = MAX(getHeight(this->mLeft), getHeight(this->mRight)) + 1;
  x->mHeight = MAX(getHeight(x->mLeft), getHeight(x->mRight)) + 1;

  return x;
}

BAvlNode *BAvlNode::LeftRotate() {

  /*
          this                          x
           / \       mLeft-rotate       / \
        (T1)  x      ---------->    this (T3)
             / \                     / \
          (T2) (T3)               (T1) (T2)
*/

  BAvlNode *x = this->mRight;
  BAvlNode *T2 = x->mLeft;
  this->mRight = T2;
  x->mLeft = this;

  this->mHeight = MAX(getHeight(this->mLeft), getHeight(this->mRight)) + 1;
  x->mHeight = MAX(getHeight(x->mLeft), getHeight(x->mRight)) + 1;

  return x;
}

BAvlTree::BAvlTree() {
  this->mRoot = ENull;
  mSize = 0;
}

BAvlNode *BAvlTree::Insert(BAvlNode *aNewNode, BAvlNode *aRoot) {
  if (aRoot == ENull) {
    return aNewNode;
  }

  if (aNewNode->mKey < aRoot->mKey) {
    aRoot->mLeft = Insert(aNewNode, aRoot->mLeft);
  }
  else if (aNewNode->mKey > aRoot->mKey) {
    aRoot->mRight = Insert(aNewNode, aRoot->mRight);
  }
  else {
    dlog("Duplicate value, cannot insert %d\n", aNewNode->mKey);
    return aRoot;
  }
  aRoot->mHeight = MAX(getHeight(aRoot->mLeft), getHeight(aRoot->mRight)) + 1;

  int bFactor = balanceFactor(aRoot);

  if (bFactor <= 1 && bFactor >= -1) {
    return aRoot;
  }
  else {
    if (bFactor < -1) {
      if (aNewNode->mKey > aRoot->mRight->mKey) { //mRight - mRight inserting
        return aRoot->LeftRotate();
      }
      else {//mRight - mLeft inserting
        aRoot->mRight = aRoot->mRight->RightRotate();
        return aRoot->LeftRotate();
      }
    }
    else if (bFactor > 1) {
      if (aNewNode->mKey < aRoot->mLeft->mKey) { //left - left inserting
        return aRoot->RightRotate();
      }
      else {//mLeft - mRight inserting
        aRoot->mLeft = aRoot->mLeft->LeftRotate();
        return aRoot->RightRotate();
      }
    }
  }
  return aRoot;
}

BAvlNode *BAvlTree::getMinNode(BAvlNode *aRoot) {
  BAvlNode *temp = aRoot;

  while (temp->mLeft != ENull) {
    temp = temp->mLeft;
  }
  return temp;
}

BAvlNode *BAvlTree::Remove(TInt64 aKey, BAvlNode *aRoot) {
  if (aRoot == ENull) {
    dlog("The value %d not found\n", aKey);
    return aRoot;
  }

  if (aKey < aRoot->mKey) {
    aRoot->mLeft = Remove(aKey, aRoot->mLeft);
  }
  else if (aKey > aRoot->mKey) {
    aRoot->mRight = Remove(aKey, aRoot->mRight);
  }
  else {
    if (aRoot->mLeft == ENull || aRoot->mRight == ENull) { // 0 or 1 child case
      if (aRoot->mLeft == ENull) {
	// mLeft child not present
        aRoot = aRoot->mRight;
      }
      else {
	// mRight child not present
        aRoot = aRoot->mLeft;
      }
    }
    else {
      // 2 children case
      BAvlNode *minMRight = getMinNode(aRoot->mRight);
      aRoot->mKey = minMRight->mKey;
      aRoot->mRight = Remove(minMRight->mKey, aRoot->mRight);
    }

    int bFactor = balanceFactor(aRoot);

    if (bFactor <= 1 && bFactor >= -1) {
      return aRoot;
    }
    else {
      if (bFactor < -1) {
        if (balanceFactor(aRoot->mRight) > 0) {
          aRoot->mRight = aRoot->mRight->RightRotate();
          return aRoot->LeftRotate();
        }
        else {
          return aRoot->LeftRotate();
	}
      }
      else if (bFactor > 1) {
        if (balanceFactor(aRoot->mLeft) > 0) {
          return aRoot->RightRotate();
	}
        else {
          aRoot->mLeft = aRoot->mLeft->LeftRotate();
          return aRoot->RightRotate();
        }
      }
    }
  }
  return aRoot;
}

BAvlNode *BAvlTree::Update(TInt64 keyOld, TInt64 keyNew, BAvlNode *aRoot) {
  BAvlNode *node = Find(keyOld);
  if (!node) {
    dlog("No node with value %d\b", keyOld);
    return aRoot;
  }
  else {
    //cannot just change the value of given node to new value as BST property
    //may be violated
    aRoot = Remove(keyOld, aRoot);
    node->mKey = keyNew;
    aRoot = Insert(node, aRoot);
  }
  return aRoot;
}

