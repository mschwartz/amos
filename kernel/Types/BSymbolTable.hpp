#ifndef RCOMP_BSYMBOLTABLE_H
#define RCOMP_BSYMBOLTABLE_H

#include <Types/BList.hpp>
// #include <string.h>

const TInt HASH_SIZE = 256;

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

struct BSymbol : public BNode {
  BSymbol(const char *aName, TUint32 aValue, void *aPtr = ENull) : BNode(aName){
    mValue = aValue;
    mPtr = aPtr;
  }

  ~BSymbol() {
//    delete[] name;
  }

  void *mPtr; // ptr to anything you want to be able to lookup
  TUint32 mValue;
};

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

struct BSymbolList : public BList {
public:
  BSymbolList() :  BList("BSymbolList") {}
  ~BSymbolList() {
    while (BSymbol *s = RemHead()) {
      delete s;
    }
  }

public:
  BSymbol *RemHead() { return (BSymbol *) BList::RemHead(); }

  BSymbol *First() { return (BSymbol *) mNext; }

  BSymbol *Next(BSymbol *curr) { return (BSymbol *) curr->mNext; }

  BSymbol *Last() { return (BSymbol *) mPrev; }

  BSymbol *Prev(BSymbol *curr) { return (BSymbol *) curr->mPrev; }

  TBool End(BSymbol *curr) { return curr == (BSymbol *) this; }
};

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

class BSymbolTable : public BBase {
public:
  BSymbolTable();

  ~BSymbolTable();

public:
  BSymbol *LookupSymbol(const char *name);

  TBool AddSymbol(const char *aName, TUint32 aValue, TAny *aPtr = ENull);

protected:
  BSymbolList mBuckets[HASH_SIZE];
};


#endif //RCOMP_BSYMBOLTABLE_H
