#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.h>
#include <Exec/BList.h>
#include <Exec/BTask.h>
#include <Exec/BDevice.h>

class ExecBase : public BBase {
public:
  ExecBase();
  ~ExecBase();
  void Init();

public:
  BTask *GetCurrentTask() { 
    return mCurrentTask; 
  }

  void AddDevice(BDevice *aDevice);
  BDevice *FindDevice(const char *aName);

  void SetCurrentTask(BTask *aTask);
  void AddTask(BTask *aTask);
  static void DumpCurrentTask();
  static void DumpCurrentTaskRegisters();

protected:
  BDeviceList *mDeviceList;
  BTaskList *mTasksReady;
  BTaskList *mTasksWait;
  BTask *mCurrentTask;

protected:
  volatile TInt32 mDisableNestCount;

public:
  void Disable();
  void Enable();

public:
  static ExecBase &GetExecBase();

};

#endif
