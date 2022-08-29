# 为什么创建这个项目？
- 不满足于一般的生产消费者模型和观察者模型；
- 希望能充分解耦程序来应对大型工程；
- 总结一下业余时间的想法。

# Why create this project?
- Not satisfied with the general production-consumer model and observer model;
- It is hoped that the program can be fully decoupled to deal with large-scale projects;
- Summarize the idea of spare time.

# 这个项目能做什么？
- 对程序进行解耦
- 控制数据分发与流向

# What can this project do?
- Decouple the program
- Control data distribution and flow

# 计划使用的重载运算符
- 管道之间，加号（+）用于并行连接多个管道，被链接的管道将分别收到前置管道流出的数据流；
- 管道之间，减号（-）用于解除两个管道之间的链接，若被解除管道没有下级管道，则被解除管道称为末端管道，任何输出将被抛弃；
- 管道与经纪人之间，乘号（*）用于将管道链接到经纪人；
- 管道与经纪人之间，除号（/）用于解除管道到经纪人之间的链接；
- 经纪人与管道之间，乘号（*）用于将经纪人链接到管道；
- 经纪人与管道之间，除号（/）用于解除经纪人与管道之间的链接；

# Planned overloaded operators
- Between pipes, the plus sign (+) is used to connect multiple pipes in parallel, and the linked pipes will receive the data stream from the preceding pipe respectively;
- Between pipes, the minus sign (-) is used to unlink the two pipes. If the unblocked pipe has no subordinate pipes, the unblocked pipe is called the end pipe, and any output will be discarded;
- Between the pipeline and the broker, the multiplication sign (*) is used to link pipeline to broker;
- Between the pipeline and the broker, the division sign (/) is used to unlink the pipeline to the broker;
- between the broker and the pipeline, the multiplication sign (*) is used to link the broker to the pipeline;
- Between the broker and the pipeline, the division sign (/) is used to unlink the broker and the pipeline;

# 数据处理&流向处理
- 所有管道流入流出数据均为字节；
- 流入管道数据触发处理，处理函数抛入worker线程池执行；

# Data processing & flow processing
- All pipe inflow and outflow data are bytes;
- The incoming pipeline data triggers processing, and the processing function is thrown into the worker thread pool for execution;

# 为什么没有优先级？
数据流就像水流一样，遇窄则慢，人为增加优先级只会对其它数据流增加延迟，正确的做法是数据处理中管控时间。

# Why no priority?
A data flow is like a water flow. When it is narrow, it is slow. Artificially increasing the priority will only increase the delay of other data flows. The correct way is to control the time during data processing.
