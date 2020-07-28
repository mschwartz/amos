#include <Types/BHashTable.hpp>

TInt64 BHashNode::Hash(const char *aKey) {
  TInt64 sum = 0;
  const char *pc = aKey;
  while (*pc) {
    sum += *pc++;
  }
  return sum;
}

BHashNode::BHashNode(const char *aKey) : BNode(aKey), mHash(Hash(aKey)) {
  // mHash = Hash(aKey); // calculate this once to save time during lookups, etc.
}

BHashNode::~BHashNode() {
  //
}

TBool BHashNode::Equals(TInt64 aHash, const char *aString) {
  return aHash == mHash && (CompareStrings(aString, mNodeName) == 0);
}

BHashTable::BHashTable(const TInt aHashBuckets) {
  mCount = 0;
  mHashBuckets = aHashBuckets;
  mHashArray = new BList[aHashBuckets];
  //
}

BHashTable::~BHashTable() {
  //
  for (TInt i = 0; i < HASH_BUCKETS; i++) {
    BList *list = (BList *)&mHashArray[i];
    while (BHashNode *node = (BHashNode *)list->RemHead()) {
      delete node;
    }
  }
}

BHashNode *BHashTable::Find(const char *aKey) {
  TInt64 hash = BHashNode::Hash(aKey);
  TInt64 index = hash % HASH_BUCKETS;
  BList *list = (BList *)&mHashArray[index];
  for (BHashNode *node = (BHashNode *)list->First(); !list->End(node); node = (BHashNode *)node->mNext) {
    if (node->Equals(hash, aKey)) {
      return node;
    }
  }
  return ENull;
}

TBool BHashTable::Add(BHashNode &aNode) {
  if (Find(aNode.mNodeName)) {
    return EFalse;
  }
  TInt64 index = aNode.mHash % HASH_BUCKETS;
  BList *list = (BList *)&mHashArray[index];
  list->AddHead(aNode);
  mCount++;
  return ETrue;
}

TBool BHashTable::Remove(const char *aKey) {
  BHashNode *node = Find(aKey);
  if (!node) {
    return EFalse;
  }
  node->Remove();
  mCount--;
  return ETrue;
}

BHashNode *BHashTable::Replace(BHashNode& aNode) {
  BHashNode *node = Find(aNode.mNodeName);
  if (!node) {
    node->Remove();
  }
  TInt64 index = aNode.mHash % HASH_BUCKETS;
  BList *list = (BList *)&mHashArray[index];
  list->AddHead(aNode);
  return node;
}
