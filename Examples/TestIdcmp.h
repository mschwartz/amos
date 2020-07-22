#ifndef EXAMPLES_TEST_IDCMP_H
#define EXAMPLES_TEST_IDCMP_H

#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>

class TestIdcmpTask : public BTask {
 public:
  TestIdcmpTask();
  ~TestIdcmpTask();

 public:
  TInt64 Run();
};

#endif
