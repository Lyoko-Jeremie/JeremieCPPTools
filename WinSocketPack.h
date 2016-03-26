#ifndef WINSOCKETPACK_H
#define WINSOCKETPACK_H

// Winsock2 TCP 客户端代码
#include <Winsock2.h>       // 记得包含库 ws2_32
#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")
#endif

#include <stdexcept> // 抛出错误 runtime_error

// C++11 Pthread 库
//#include <thread>
#include <mutex>
#include <atomic>

#include "CharArray.h"

#include "RWLock.h"

//#include <vector>
#include <deque>
#include <string>


/// 已经解决了无用的句柄占用池的问题
/// 虽然没有实现句柄重用 但是实现了CleaningPool函数将已经CloseSocket的句柄从池中清除


/// 声明了会抛出的异常 但也请注意要使用set_unexpected

/// WinSocket 包装类
class WinSocketPack
{
    public:

        enum WSP_AF{

            WSP_AF_UNSPEC = 0,
            WSP_AF_UNIX = 1,
            WSP_AF_INET = 2,
            WSP_AF_IMPLINK = 3,
            WSP_AF_PUP = 4,
            WSP_AF_CHAOS = 5,
            WSP_AF_NS = 6,
            WSP_AF_IPX = AF_NS,
            WSP_AF_ISO = 7,
            WSP_AF_OSI = AF_ISO,
            WSP_AF_ECMA = 8,
            WSP_AF_DATAKIT = 9,
            WSP_AF_CCITT = 10,
            WSP_AF_SNA = 11,
            WSP_AF_DECnet = 12,
            WSP_AF_DLI = 13,
            WSP_AF_LAT = 14,
            WSP_AF_HYLINK = 15,
            WSP_AF_APPLETALK = 16,
            WSP_AF_NETBIOS = 17,
            WSP_AF_VOICEVIEW = 18,
            WSP_AF_FIREFOX = 19,
            WSP_AF_UNKNOWN1 = 20,
            WSP_AF_BAN = 21,
            WSP_AF_ATM = 22,
            WSP_AF_INET6 = 23,
            WSP_AF_CLUSTER = 24,
            WSP_AF_12844 = 25,
            WSP_AF_IRDA = 26,
            WSP_AF_NETDES = 28,
            WSP_AF_TCNPROCESS = 29,
            WSP_AF_TCNMESSAGE = 30,
            WSP_AF_ICLFXBM = 31,
            WSP_AF_BTH = 32,
            WSP_AF_MAX = 33

        };

        enum class WSP_TYPE {

            WSP_SOCK_STREAM = 1,
            WSP_SOCK_DGRAM = 2,
            WSP_SOCK_RAW = 3,
            WSP_SOCK_RDM = 4,
            WSP_SOCK_SEQPACKET = 5

        };

        enum class WSP_PROTOCOL {

            WSP_IPPROTO_IP = 0,
            WSP_IPPROTO_HOPOPTS = 0,
            WSP_IPPROTO_ICMP = 1,
            WSP_IPPROTO_IGMP = 2,
            WSP_IPPROTO_GGP = 3,
            WSP_IPPROTO_IPV4 = 4,
            WSP_IPPROTO_TCP = 6,
            WSP_IPPROTO_PUP = 12,
            WSP_IPPROTO_UDP = 17,
            WSP_IPPROTO_IDP = 22,
            WSP_IPPROTO_IPV6 = 41,
            WSP_IPPROTO_ROUTING = 43,
            WSP_IPPROTO_FRAGMENT = 44,
            WSP_IPPROTO_ESP = 50,
            WSP_IPPROTO_AH = 51,
            WSP_IPPROTO_ICMPV6 = 58,
            WSP_IPPROTO_NONE = 59,
            WSP_IPPROTO_DSTOPTS = 60,
            WSP_IPPROTO_ND = 77,
            WSP_IPPROTO_ICLFXBM = 78,
            WSP_IPPROTO_RAW = 255,
            WSP_IPPROTO_MAX = 256

        };

        enum WSP_INADDR {
            WSP_INADDR_ANY = INADDR_ANY,
            WSP_INADDR_LOOPBACK = INADDR_LOOPBACK,
            WSP_INADDR_BROADCAST = INADDR_BROADCAST,
            WSP_INADDR_NONE = INADDR_NONE

        };

        enum WSP_SHUTDOWN {
            WSP_SD_RECEIVE = SD_RECEIVE,
            WSP_SD_SEND = SD_SEND,
            WSP_SD_BOTH = SD_BOTH
        };

        int static const WSP_SOMAXCONN = 0x7fffffff;
        size_t static const WSP_ACCEPT_ERROR = ~0;  // 也就是取最大值

        // 单例 禁止重复构造
        WinSocketPack()  throw (std::runtime_error, bad_exception);
        // 禁用拷贝与移动函数
        WinSocketPack( WinSocketPack const & ) = delete;
        WinSocketPack & operator= ( WinSocketPack const &) = delete;
        WinSocketPack( WinSocketPack &&) = delete;
        WinSocketPack & operator= ( WinSocketPack && ) = delete;

        virtual ~WinSocketPack();

        SOCKET GetSocket( size_t i) throw( overflow_error, runtime_error, bad_exception );
        size_t GetPoolSize( ) throw( bad_exception );
        size_t GetIDSize( ) throw( bad_exception );
        bool EmptyPool( ) throw( bad_exception );
        void CleaningPool( );

        void CloseSocket( size_t i) throw( out_of_range, bad_exception );
        void DeleteSocket( size_t i);
        bool Shutdown( size_t i, WSP_SHUTDOWN how) throw( out_of_range, bad_exception );

        size_t NewSocket( WSP_AF af, WSP_TYPE type, WSP_PROTOCOL protocol ) throw( runtime_error, overflow_error, bad_exception );
        size_t NewSocketTCP() throw( runtime_error, overflow_error, bad_exception );

        bool Bind( size_t i, std::string addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception );
        bool Bind( size_t i, WSP_INADDR addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception );

        bool Listen( size_t i, int MaximumQueue) throw( overflow_error, runtime_error, bad_exception );

        // blocking 是否阻塞
        bool SetBlockingMode( size_t i, bool blocking) throw( overflow_error, runtime_error, bad_exception );

        // Note: Accept 失败返回 WSP_ACCEPT_ERROR
        size_t Accept( size_t i) throw( overflow_error, runtime_error, bad_exception );

        bool Connect(  size_t i, std::string addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception );

        bool Send( size_t i, std::string data) throw( overflow_error, runtime_error, bad_exception );
        int Recv( size_t i, std::string &data, size_t maxlength = 100) throw( overflow_error, runtime_error, bad_exception );     // -1 SOCKET_ERROR    0 连接被正常关闭  1 成功
        int Recv( size_t i, CharArray &data) throw( overflow_error, runtime_error, bad_exception );     // -1 SOCKET_ERROR    0 连接被正常关闭  1 成功
    protected:
    private:

        struct SocketObject
        {
            SocketObject() : sock(0), id(0), able(false) {}
            SocketObject( SOCKET so, size_t id, bool Enable = true) : sock(so), id(id), able(Enable) {}
            SOCKET sock;
            size_t id;
            bool able;
        };

        std::deque<SocketObject> PoolSocket;  // SOCKET 句柄池      TODOED 已经增加多线程支持
        /// 尚未实现重用功能

//        // deque 添加元素不移动位置 vector有可能 效能相近
//        // 因为 SOCKET 实为句柄 【unsigned __int64 or unsigned __int】
//        // 并且没有删除操作

//        // 所有访问都加锁  反正除了send & recv & Accept 不会出现频繁访问

//        std::mutex MTXPoolSocket;   // 对 PoolSocket 的互斥锁
//        //std::lock_guard<std::mutex> mtxp(MTXPoolSocket);

        /// PoolSocket 读写保护原子锁
        RWLock RWLPoolSocket;

        // 终止锁
        std::atomic_bool ATMStop;

        // 递增id互斥体
        std::mutex MTXID;
        // 递增的id
        size_t IDmain;

        // 获取一个新的ID
        size_t GetID() throw( overflow_error, bad_exception );
        // 从id获取池index
        // 不加锁 需要在外部对池加锁
        size_t GetSocketIndex( size_t id);
        // 无效的池下标
        size_t static const bad_index = ~0;



        /// Note : Dead-Lock Waring
        /// 在需要对 RWLPoolSocket 和 MTXID 同时获取的场景下
        /// 必须要先获取 RWLPoolSocket 再获取 MTXID
        /// 否则必然出现死锁


};


class WSPSocketGuard
{
    public:
        inline
        WSPSocketGuard( WinSocketPack &wspack, size_t wsphandle ):wsp(wspack),wsph(wsphandle)
        {
            return;
        }

        inline
        ~WSPSocketGuard()
        {
            wsp.Shutdown( wsph, WinSocketPack::WSP_SHUTDOWN::WSP_SD_BOTH);
            return;
        }

        WSPSocketGuard( WSPSocketGuard const & other) = delete;
        WSPSocketGuard &operator=( WSPSocketGuard const & other) = delete;
        WSPSocketGuard( WSPSocketGuard && other) = delete;
        WSPSocketGuard &operator=( WSPSocketGuard && other) = delete;

    private:
        WinSocketPack &wsp;

    public:
        size_t wsph;

    protected:
};







#endif // WINSOCKETPACK_H
