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
  BNode() : BBase(){}
  virtual ~BNode() {}

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
  BNodePri(TInt aPri = 0) : BBase(), pri(aPri) {}
  virtual ~BNodePri() {}

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
  void Reset() {
    mNext = (BNode *)this;
    mPrev = (BNode *)this;
  }

  void AddHead(BNode &aNode);
  BNode *RemHead();
  void AddTail(BNode &aNode);
  BNode *RemTail();
  void RemoveNode(BNode *aNode);

  /**
  * Get the element from the head of the list.
  * @return The head element.
  */
  BNode *First() { return mNext; }

  /**
   * Get the element after the specified element.
   * @param curr The current element;
   * @return The next element in the list;
   */
  BNode *Next(BNode *aNode) { return aNode->mNext; }

  /**
    * Get the element from the tail of the list.
    * @return The head element.
    */
  BNode *Last() { return mPrev; }

  /**
   * Get element before the specified element.
   * @param curr The current element;
   * @return The previous element in the list;
   */
  BNode *Prev(BNode *aNode) { return aNode->mPrev; }

  /**
   * Test if the specified element is the end of the list.
   * @param curr The element to test.
   * @return True if is is the end or faes if it is not the end.
   */

  TBool End(BNode *aNode) { return aNode == (BNode *)this; }
};

/**
 * Double linked list of elements, sorted by descending priority.
 *
 * Elements with the highest priority value are near the head of the list.
 * Elements with the lowest priority value are near the tail of the list.
 */
class BListPri : public BNodePri {
public:
  BListPri() : BNodePri(0) {
    Reset();
  }
  virtual ~BListPri() {}

public:
  void Dump(BNodePri *aStop = ENull);

public:
  /**
   * Remove all of the elements from the list.
   */
  void Reset() {
    mNext = (BNodePri *)this;
    mPrev = (BNodePri *)this;
  }

  void Add(BNodePri &aNode);
  void RemoveNode(BNodePri *aNode);
  /**
   * Get the head element from the list.
   * @return The head element.
   */
  BNodePri *First() { return mNext; }
  /**
   * Test if the specified element is the end of the list.
   * @param curr The element to test.
   * @return True if is is the end or faes if it is not the end.
   */
  TBool End(BNodePri *curr) { return curr == (BNodePri *)this; }
  BNodePri *RemHead();
  BNodePri *RemTail();
  /**
   * Get the element after the specified element.
   * @param curr The current element;
   * @return The mNext element in the list;
   */
  BNodePri *Next(BNodePri *curr) { return curr->mNext; }
  /**
    * Get the element from the tail of the list.
    * @return The head element.
    */
  BNodePri *Last() { return mPrev; }
  /**
   * Get element before the specified element.
   * @param curr The current element;
   * @return The mPrevious element in the list;
   */
  BNodePri *Prev(BNodePri *curr) { return curr->mPrev; }

//private:
  void AddHead(BNodePri &nodevirtual);
  void AddTail(BNodePri &node);
};

#endif
