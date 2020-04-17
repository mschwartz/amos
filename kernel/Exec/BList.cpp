#include <Exec/BList.h>
#include <Exec/Memory.h>

#ifdef KERNEL
#include <x86/bochs.h>
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BNode

BNode::BNode(const char *aNodeName) : BBase() {
  mNodeName = DuplicateString(aNodeName);
}

BNode::~BNode() {
  if (mNodeName) {
    delete mNodeName;
  }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BNodePri

BNodePri::BNodePri(const char *aNodeName, TInt aPri) : BNode(aNodeName), pri(aPri) {}

BNodePri::~BNodePri() {}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BList

/**
 * Create an empty list.
 */
BList::BList(const char *aNodeName) : BNode(aNodeName) { 
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

BNode *BList::Find(const char *aNodeName) {
  for (BNode *n = First(); !End(n); n = n->mNext) {
    if (CompareStrings(aNodeName, n->mNodeName) == 0) {
      return n;
    }
  }
  return ENull;
}

BNode *BList::Find(BNode& aNode) {
  for (BNode *n = First(); !End(n); n = n->mNext) {
    if (n == &aNode) {
      return n;
    }
  }
  return ENull;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/// BListPri

#if 0
/**
 * Create an empty priority list.
 */
BListPri::BListPri() : BNodePri(0) {
  Reset();
}

/**
 * Print a message if the list is not empty when it is deleted.
 */
BListPri::~BListPri() {
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
#endif

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
  BNodePri *n = mNext;
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
  BNodePri *n = mPrev;
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
  for (BNodePri *n = First(); !End(n); n = n->mNext) {
    if (aNode.pri < n->pri) {
      aNode.InsertBeforeNode(n);
      return;
    }
  }
  AddTail(aNode);
}

BNodePri *BListPri::Find(const char *aNodeName) {
  for (BNodePri *n = First(); !End(n); n = n->mNext) {
    if (CompareStrings(aNodeName, n->mNodeName) == 0) {
      return n;
    }
  }
  return ENull;
}

BNodePri *BListPri::Find(BNodePri& aNode) {
  for (BNodePri *n = First(); !End(n); n = n->mNext) {
    if (n == &aNode) {
      return n;
    }
  }
  return ENull;
}

