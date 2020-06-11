#ifndef EXEC_BSPARSRARRAY_H
#define EXEC_BSPARSRARRAY_H

#include <Exec/Types/BList.h>
#include <Exec/Memory.h>

const TInt MAX_SPARSEARRAY_BUCKETS = 4096;
const TInt SPARSEARRAY_BUCKETS = 256;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BSparseArray;

class BSparseArrayNode : public BNode {
  friend BSparseArray;

public:
  BSparseArrayNode(const char *aName, TInt64 mKey);
  ~BSparseArrayNode();

protected:
  TInt64 mSparseKey;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BSparseArray : public BBase {
public:
  BSparseArray(TInt aSparseArrayBuckets = SPARSEARRAY_BUCKETS);
  ~BSparseArray();

public:
  TInt64 Count() { return mCount; }
  BSparseArrayNode *Find(const TInt64 aKey);
  TBool Add(BSparseArrayNode &aNode);
  TBool Remove(TInt64 aKey);
  BSparseArrayNode *Replace(BSparseArrayNode &aNode); // returns old Node, or ENull

protected:
  TInt64 mCount;
  TInt64 mSparseArrayBuckets; // number of buckets
  BList *mSparseArrayLists;
};

#endif
