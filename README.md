# JeremieCPPTools
Jeremie's C++ Tools Lib


全都是之前在做项目时自己写的各个工具类，放在一起备份一下，不然版本太多找不到

包含多线程、Socket、字符串、其他，几大类

所有工具均使用C++11标准，多线程库全部基于C++11的PThread模型，Socket、CmdAnalyzeTools、FileList库依赖windowsAPI


多线程类：多读者多写者锁RWLock、读写同步锁RWABLock、可重入路障Barrier

Socket：WinSocketPack 为了便于使用封装了ws2_32的基本API并实现可重入与多线程安全

工具类：作为与C类库打交道的安全Buffer类CharArray、简易命令行工具CmdAnalyzeTools、时间度量TimeGuard、目录列出FileList





GNUv3授权
