#include <Exec/BList.h>
#ifdef KERNEL
#include <x86/bochs.h>
//#else
//#include <stdio.h>
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

BNode::BNode(const char *aName) {
#ifdef KERNEL
  dprint("BNode %s\n", aName);
  bochs
#endif
  mNodeName = DuplicateString(aName);
}

BNode::~BNode() {
  if (mNodeName) {
    delete[] mNodeName;
  }
  mNodeName = ENull;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BList

/**
 * Create an empty list.
 */
BList::BList(const char *aName) : BNode(aName) {
  Reset();
}

/**
 * Print a message if the list is not empty when it is deleted.
 */
BList::~BList() {
  //#ifndef PRODUCTION
  //#if (defined(__XTENSA__) && defined(DEBUGME)) || !defined(__XTENSA__)
  //#ifdef KERNEL
  //   if (mNext != this) {
  //     dprint("List not empty!");
  //   }
  //#endif
  //#endif
  //#endif
}

/**
 * Add an element to the tail of the list.
 * @param node The element to add.
 */
void BList::AddTail(BNode &aNode) {
  aNode.InsertBeforeNode(this);
}

/**
 * Add an element to the head of the list.
 * @param node The element to add.
 */
void BList::AddHead(BNode &aNode) {
  aNode.InsertAfterNode(this);
}

/**
 * Remove the element at the head of the list.
 * @return The removed element.
 */
BNode *BList::RemHead() {
  BNode *n = mNext;
  if (n == (BNode *)this)
    return ENull;
  n->Remove();
  return n;
}

/**
 * Remove the element at the tail of the list.
 * @return The removed element.
 */
BNode *BList::RemTail() {
  BNode *n = mPrev;
  if (n == (BNode *)this)
    return NULL;
  n->Remove();
  return n;
}

/**
 * Remove the specified element from the list.
 *  * @param node The element to remove.
 */
void BList::RemoveNode(BNode *aNode) {
  aNode->Remove();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BListPri

BNodePri::BNodePri(const char *aName, TInt64 aPri = 0) : BNode(aName), pri(aPri) {
  //
#ifdef KERNEL
  dprint("BNodePri %s\n", aName);
  bochs
#endif
}

BNodePri::~BNodePri() {
  //
}

/**
 * Create an empty priority list.
 */
BListPri::BListPri(const char *aName) : BNodePri(aName, 0) {
#ifdef KERNEL
  dprint("BListPri\n");
  bochs
#endif
  Reset();
}

/**
 * Print a message if the list is not empty when it is deleted.
 */
BListPri::~BListPri() {
  if (mNext != this) {
#ifdef KERNEL
    dprint("List not empty!\n");
//#else
//    printf("*** List not empty!\n");
#endif
  }
}

void BListPri::Dump(BNodePri *aStop) {
  //#ifdef KERNEL
  //  for (auto *s = First(); !End(s); s = Next(s)) {
  //    dprint("Node %p PRI(%d)\n", s, s->pri);
  //    if (aStop && s == aStop) {
  //      break;
  //    }
  //  }
  //#endif
}

void BListPri::AddTail(BNodePri &aNode) {
  aNode.InsertBeforeNode(this);
}

void BListPri::AddHead(BNodePri &aNode) {
  aNode.InsertAfterNode(this);
}

/**
 * Remove the prioritized element at the head of the list.
 * @return The removed element.
 */
BNodePri *BListPri::RemHead() {
  BNodePri *n = (BNodePri *)mNext;
  if (n == (BNodePri *)this) {
    return NULL;
  }
  n->Remove();
  return n;
}

/**
 * Remove the prioritized element at the tail of the list.
 * @return The removed element.
 */
BNodePri *BListPri::RemTail() {
  BNodePri *n = (BNodePri *)mPrev;
  if (n == (BNodePri *)this) {
    return NULL;
  }
  n->Remove();
  return n;
}

/**
 * Remove the specified element from the list.
 * @param node The element to remove.
 */
void BListPri::RemoveNode(BNodePri *aNode) {
  aNode->Remove();
}

void BListPri::Add(BNodePri &aNode) {
#ifdef KERNEL
  dprint("Add %x\n", aNode);
#endif
  for (BNodePri *n = First(); !End(n); n = (BNodePri *)n->mNext) {
    if (aNode.pri < n->pri) {
      aNode.InsertBeforeNode(n);
      return;
    }
  }
  AddTail(aNode);
}
