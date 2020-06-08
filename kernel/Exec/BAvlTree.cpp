#include <Exec/BAvlTree.h>

static int getHeight(BAvlNode *root) {
  if (root == ENull) { //height of leaf = 0
    return 0;
  }
  return root->height;
}

static int balanceFactor(BAvlNode *root) {
  if (root == ENull) { //balanceFactor of leaf = 0
    return 0;
  }
  return (getHeight(root->left) - getHeight(root->right));
}

static void printGraphviz(BAvlNode *root, int size) {
  if (size > 1) {
    if (root) {
      if (root->left) {
        // cout << '"' << root->data << '"';
        // cout << " -> ";
        // cout << '"' << root->left->data << '"';
        // cout << endl;
        printGraphviz(root->left, size);
      }
      if (root->right) {
        // cout << '"' << root->data << '"';
        // cout << " -> ";
        // cout << '"' << root->right->data << '"';
        // cout << endl;
        printGraphviz(root->right, size);
      }
    }
  }
  else {
  // cout << '"' << root->data << '"';
  }
}

TBool searchNode(int key, BAvlNode *root) {
  if (root == ENull) {
    return EFalse;
  }
  if (root->mData == key) {
    return ETrue;
  }
  else {
    if (key < root->mData)
      return searchNode(key, root->left);
    else
      return searchNode(key, root->right);
  }
  return EFalse;
}

BAvlNode *BAvlTree::Find(TInt64 aValue, BAvlNode *aRoot) {
  if (aRoot == ENull) {
    return ENull;
  }
  if (aRoot->mData == aValue) {
    return aRoot;
  }
  else {
    if (aValue < aRoot->mData) {
      return Find(aValue, aRoot->left);
    }
    else {
      return Find(aValue, aRoot->right);
    }
  }
  return ENull;

}  

BAvlNode::BAvlNode(TInt64 aData) {
  this->mData = aData;
  this->left = ENull;
  this->right = ENull;
  this->height = 1;
}

BAvlNode *BAvlNode::rightRotate() {

  /*
           this                         x
           / \      right-rotate       / \
          x  (T3)   ----------->    (T1) this
         / \                              / \
      (T1) (T2)                        (T2) (T3)
*/

  BAvlNode *x = this->left;
  BAvlNode *T2 = x->right;
  x->right = this;
  this->left = T2;

  this->height = MAX(getHeight(this->left), getHeight(this->right)) + 1;
  x->height = MAX(getHeight(x->left), getHeight(x->right)) + 1;

  return x;
}

BAvlNode *BAvlNode::leftRotate() {

  /*
          this                          x
           / \       left-rotate       / \
        (T1)  x      ---------->    this (T3)
             / \                     / \
          (T2) (T3)               (T1) (T2)
*/

  BAvlNode *x = this->right;
  BAvlNode *T2 = x->left;
  this->right = T2;
  x->left = this;

  this->height = MAX(getHeight(this->left), getHeight(this->right)) + 1;
  x->height = MAX(getHeight(x->left), getHeight(x->right)) + 1;

  return x;
}

BAvlTree::BAvlTree() {
  this->root = ENull;
  size = 0;
}

BAvlNode *BAvlTree::Insert(BAvlNode *newNode, BAvlNode *root) {
  if (root == ENull)
    return newNode;

  if (newNode->mData < root->mData)
    root->left = Insert(newNode, root->left);
  else if (newNode->mData > root->mData)
    root->right = Insert(newNode, root->right);
  else {
    dlog("Duplicate value, cannot insert %d\n", newNode->mData);
    return root;
  }
  root->height = MAX(getHeight(root->left), getHeight(root->right)) + 1;

  int bFactor = balanceFactor(root);

  if (bFactor <= 1 && bFactor >= -1)
    return root;
  else {
    if (bFactor < -1) {
      if (newNode->mData > root->right->mData) //right - right inserting
        return root->leftRotate();
      else {
        //right - left inserting
        root->right = root->right->rightRotate();
        return root->leftRotate();
      }
    }
    else if (bFactor > 1) {
      if (newNode->mData < root->left->mData) //left - left inserting
        return root->rightRotate();
      else //left - right inserting
      {
        root->left = root->left->leftRotate();
        return root->rightRotate();
      }
    }
  }
  return root;
}

BAvlNode *BAvlTree::getMinNode(BAvlNode *root) {
  BAvlNode *temp = root;

  while (temp->left != ENull) {
    temp = temp->left;
  }
  return temp;
}

BAvlNode *BAvlTree::Remove(TInt64 key, BAvlNode *root) {
  if (root == ENull) {
    dlog("The value %d not found\n", key);
    return root;
  }

  if (key < root->mData) {
    root->left = Remove(key, root->left);
  }
  else if (key > root->mData) {
    root->right = Remove(key, root->right);
  }
  else {
    if (root->left == ENull || root->right == ENull) { // 0 or 1 child case
      if (root->left == ENull) {
	// left child not present
        root = root->right;
      }
      else {
	// right child not present
        root = root->left;
      }
    }
    else {
      // 2 children case
      BAvlNode *minRight = getMinNode(root->right);
      root->mData = minRight->mData;
      root->right = Remove(minRight->mData, root->right);
    }

    int bFactor = balanceFactor(root);

    if (bFactor <= 1 && bFactor >= -1) {
      return root;
    }
    else {
      if (bFactor < -1) {
        if (balanceFactor(root->right) > 0) {
          root->right = root->right->rightRotate();
          return root->leftRotate();
        }
        else {
          return root->leftRotate();
	}
      }
      else if (bFactor > 1) {
        if (balanceFactor(root->left) > 0) {
          return root->rightRotate();
	}
        else {
          root->left = root->left->leftRotate();
          return root->rightRotate();
        }
      }
    }
  }
  return root;
}

BAvlNode *BAvlTree::Update(TInt64 keyOld, TInt64 keyNew, BAvlNode *root) {
  BAvlNode *node = Find(keyOld);
  if (!node) {
    dlog("No node with value %d\b", keyOld);
    return root;
  }
  else {
    //cannot just change the value of given node to new value as BST property
    //may be violated
    root = Remove(keyOld, root);
    node->mData = keyNew;
    root = Insert(node, root);
  }
  return root;
}

