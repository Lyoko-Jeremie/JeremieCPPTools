# JeremieCPPTools
Jeremie's C++ Tools Lib


全都是之前在做项目时自己写的各个工具类，放在一起备份一下，不然版本太多找不到

包含多线程、Socket、字符串、其他，几大类

所有工具均使用C++11标准，多线程库全部基于C++11的PThread模型，Socket、CmdAnalyzeTools、FileList库依赖windowsAPI


多线程类：
	RWLock：多读者多写者锁
	RWABLock：读写同步锁
	Barrier：可重入路障
	SimpleThreadPool：线程池实现的std::async/get异步任务托管


Socket：
	WinSocketPack：为了便于使用封装了ws2_32的基本API并实现可重入与多线程安全

工具类：
	CharArray：作为与C类库打交道的安全Buffer类
	CmdAnalyzeTools：简易命令行工具
	TimeGuard：时间度量
	FileList：目录列出

FreeTypeTextPrinter：
	基于FreeType的OpenCV字符绘制库，解决OpenCV无法在图像上绘制中文字符的问题
	仅仅依赖于FreeType、OpenCV、C++11，在FreeType2.6.2、OpenCV2.4.11、OpenCV3.1.0下测试通过




GNUv3授权
