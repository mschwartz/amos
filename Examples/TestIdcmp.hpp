#ifndef EXAMPLES_TEST_IDCMP_H
#define EXAMPLES_TEST_IDCMP_H

#include <Exec/BTask.hpp>
#include <Inspiration/InspirationBase.hpp>

class TestIdcmpTask : public BTask {
 public:
  TestIdcmpTask();
  ~TestIdcmpTask();

 public:
  TInt64 Run();
};

#endif
