#ifndef BLIST_H
#define BLIST_H

#include "BBase.h"

/**
 * Two basic types of doubly linked lists:
 *
 * 1) BList is a linked list of BNodes
 * 2) BListPri is a linked list of BNodePri nodes, sorted by Priority.
 */

/**
 * An element in a BList linked list .
 *
 * You will inherit from this to add useful member variables - a list of BNodes
 * isn't very interesting.
 */
class BNode : public BBase {
public:
  BNode();
  virtual ~BNode();

public:
  // make this node last on the list, if node is key
  void InsertBeforeNode(BNode *aNextNode) {
    BNode *pnode = aNextNode->mPrev;
    pnode->mNext = this;
    aNextNode->mPrev = this;
    mNext = aNextNode;
    mPrev = pnode;
  }
  // make this node first on the list, if node is key
  void InsertAfterNode(BNode *mPreviousNode) {
    BNode *nnode = mPreviousNode->mNext;
    mPreviousNode->mNext = this;
    nnode->mPrev = this;
    mNext = nnode;
    mPrev = mPreviousNode;
  }
  void Remove() {
    mNext->mPrev = mPrev;
    mPrev->mNext = mNext;
  }

public:
  BNode *mNext, *mPrev;
};

/**
 * An element in a BListPri linked list.
 */
class BNodePri : public BBase {
public:
  BNodePri(TInt aPri = 0);
  virtual ~BNodePri();

public:
  void InsertBeforeNode(BNodePri *aNode) {
    BNodePri *pnode = aNode->mPrev;
    pnode->mNext = this;
    aNode->mPrev = this;
    mNext = aNode;
    mPrev = pnode;
  }
  // make this node first on the list, if node is key
  void InsertAfterNode(BNodePri *pnode) {
    BNodePri *nnode = pnode->mNext;
    pnode->mNext = this;
    nnode->mPrev = this;
    mNext = nnode;
    mPrev = pnode;
  }
  void Remove() {
    mNext->mPrev = mPrev;
    mPrev->mNext = mNext;
  }

public:
  BNodePri *mNext, *mPrev;
  TInt pri;
};

/**
 * Double linked list of elements. Elements are simply added to the head or tail of the list.
 */
class BList : public BNode {
public:
  BList();
  virtual ~BList();

  /**
   * Remove all of the elements from the list.
   */
  virtual void Reset() {
    mNext = (BNode *)this;
    mPrev = (BNode *)this;
  }

  virtual void AddHead(BNode &aNode);
  virtual BNode *RemHead();
  virtual void AddTail(BNode &aNode);
  virtual BNode *RemTail();
  virtual void RemoveNode(BNode *aNode);

  /**
  * Get the element from the head of the list.
  * @return The head element.
  */
  virtual BNode *First() { return mNext; }

  /**
   * Get the element after the specified element.
   * @param curr The current element;
   * @return The next element in the list;
   */
  virtual BNode *Next(BNode *aNode) { return aNode->mNext; }

  /**
    * Get the element from the tail of the list.
    * @return The head element.
    */
  virtual BNode *Last() { return mPrev; }

  /**
   * Get element before the specified element.
   * @param curr The current element;
   * @return The previous element in the list;
   */
  virtual BNode *Prev(BNode *aNode) { return aNode->mPrev; }

  /**
   * Test if the specified element is the end of the list.
   * @param curr The element to test.
   * @return True if is is the end or faes if it is not the end.
   */

  virtual TBool End(BNode *aNode) { return aNode == (BNode *)this; }
};

/**
 * Double linked list of elements, sorted by descending priority.
 *
 * Elements with the highest priority value are near the head of the list.
 * Elements with the lowest priority value are near the tail of the list.
 */
class BListPri : public BNodePri {
public:
  BListPri();
  virtual ~BListPri();

public:
  void Dump(BNodePri *aStop = ENull);

public:
  /**
   * Remove all of the elements from the list.
   */
  virtual void Reset() {
    mNext = (BNodePri *)this;
    mPrev = (BNodePri *)this;
  }

  virtual void Add(BNodePri &aNode);
  virtual void RemoveNode(BNodePri *aNode);
  /**
   * Get the head element from the list.
   * @return The head element.
   */
  virtual BNodePri *First() { return mNext; }
  /**
   * Test if the specified element is the end of the list.
   * @param curr The element to test.
   * @return True if is is the end or faes if it is not the end.
   */
  virtual TBool End(BNodePri *curr) { return curr == (BNodePri *)this; }
  virtual BNodePri *RemHead();
  virtual BNodePri *RemTail();
  /**
   * Get the element after the specified element.
   * @param curr The current element;
   * @return The mNext element in the list;
   */
  virtual BNodePri *Next(BNodePri *curr) { return curr->mNext; }
  /**
    * Get the element from the tail of the list.
    * @return The head element.
    */
  virtual BNodePri *Last() { return mPrev; }
  /**
   * Get element before the specified element.
   * @param curr The current element;
   * @return The mPrevious element in the list;
   */
  virtual BNodePri *Prev(BNodePri *curr) { return curr->mPrev; }

private:
  virtual void AddHead(BNodePri &nodevirtual);
  virtual void AddTail(BNodePri &node);
};

#endif
