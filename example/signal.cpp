#include "../include/signal.h"

 
class MySignalTest {
 public:
     void Log(const char* str) {
         log_event(str); // emit a signal
     }
 // 定义一个信号
 signals:
     apf::Signal<const char*> log_event;
 };
 //---------------------------------
 class MySlotTest {
 // 定义一个槽
 slots:
      void OnLog(const char* str) {printf(str);}
 };
 //----------------------------------
 void main() {
     MySignalTest signal_test = new MySignalTest();
     MySlotTest slot_test = new MySlotTest();
     // 连接信号与槽
     APFConnect(signal_test, log_event, slot_test, &MySlotTest::OnLog);
     // 执行函数发射信号
     signal_test->Log("signal test\n");
 }
