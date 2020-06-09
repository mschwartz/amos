#ifndef EXEC_BHASHTABLE_H
#define EXEC_BHASHTABLE_H

#include <Exec/BList.h>
#include <Exec/Memory.h>

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
  BHashTable();
  ~BHashTable();

public:
  TInt64 Count() { return mCount; }
  BHashNode *Find(const char *aKey);
  TBool Add(BHashNode& aNode);
  TBool Remove(const char *aKey);
  BHashNode *Replace(BHashNode& aNode); // returns old Node, or ENull

protected:
  TInt64 mCount;
  BList mHashArray[HASH_BUCKETS];
};

#endif
