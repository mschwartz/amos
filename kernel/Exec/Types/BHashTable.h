#ifndef EXEC_BHASHTABLE_H
#define EXEC_BHASHTABLE_H

#include <Exec/Types/BList.h>
#include <Exec/Memory.h>

const TInt MAX_HASH_BUCKETS = 4096;
const TInt HASH_BUCKETS = 256;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BHashTable;

class BHashNode : public BNode {
  friend BHashTable;
public:
  BHashNode(const char *aKey);
  ~BHashNode();

public:
  static TInt64 Hash(const char *aString);
  TBool Equals(TInt64 aHash, const char *aKey);
  TBool Equals(BHashNode *aOther) {
    return Equals(aOther->mHash, aOther->mNodeName);
  }

protected:
  const TInt64 mHash;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BHashTable : public BBase {
public:
  BHashTable(TInt aHashBuckets = HASH_BUCKETS);
  ~BHashTable();

public:
  TInt64 Count() { return mCount; }
  BHashNode *Find(const char *aKey);
  TBool Add(BHashNode& aNode);
  TBool Remove(const char *aKey);
  BHashNode *Replace(BHashNode& aNode); // returns old Node, or ENull

protected:
  TInt64 mCount;
  TInt64 mHashBuckets; // number of buckets
  BList *mHashArray;
};

#endif
