#include <Types/BSparseArray.h>

// constructor
BSparseArrayNode::BSparseArrayNode(const char *aName, TInt64 aKey) : BNode(aName) {
  mSparseKey = aKey;
}

BSparseArrayNode::~BSparseArrayNode() {
  //
}

// constructor
BSparseArray::BSparseArray(const TInt aSparseArrayBuckets) {
  mCount = 0;
  mSparseArrayBuckets = aSparseArrayBuckets;
  // mDiskCache = new BSparseArray(mSparseArrayBuckets);
  dlog("construct BSparseArray(%d)\n", mSparseArrayBuckets);
  // mSparseArrayLists = new BList[aSparseArrayBuckets];
}

BSparseArray::~BSparseArray() {
  for (TInt i = 0; i < mSparseArrayBuckets; i++) {
    BList *list = (BList *)&mSparseArrayLists[i];
    while (BSparseArrayNode *node = (BSparseArrayNode *)list->RemHead()) {
      delete node;
    }
  }
  // delete[] mSparseArrayLists;
}

BSparseArrayNode *BSparseArray::Find(const TInt64 aKey) {
  // dlog("BSParsaeArray::Find(%d) buckets(%d)\n", aKey, mSparseArrayBuckets);
  TInt64 index = aKey % mSparseArrayBuckets;
  // dlog("index(%d)\n", index);
  BList *list = (BList *)&mSparseArrayLists[index];
  // dlog("list(%x)\n", list);

  for (BSparseArrayNode *node = (BSparseArrayNode *)list->First();
       !list->End(node);
       node = (BSparseArrayNode *)node->mNext) {

    // dlog("node(%x)\n", node);
    if (node->mSparseKey == aKey) {
      return node;
    }
  }

  return ENull;
}

TBool BSparseArray::Add(BSparseArrayNode &aNode) {
  if (Find(aNode.mSparseKey)) {
    return EFalse;
  }
  TInt64 index = aNode.mSparseKey % mSparseArrayBuckets;
  BList *list = (BList *)&mSparseArrayLists[index];
  list->AddHead(aNode);
  mCount++;
  return ETrue;
}

TBool BSparseArray::Remove(TInt64 aKey) {
  BSparseArrayNode *node = Find(aKey);
  if (!node) {
    return EFalse;
  }
  node->Remove();
  mCount--;
  return ETrue;
}

BSparseArrayNode *BSparseArray::Replace(BSparseArrayNode &aNode) {
  BSparseArrayNode *node = Find(aNode.mSparseKey);
  if (!node) {
    node->Remove();
  }
  TInt64 index = aNode.mSparseKey % mSparseArrayBuckets;
  BList *list = (BList *)&mSparseArrayLists[index];
  list->AddHead(aNode);
  return node;
}
