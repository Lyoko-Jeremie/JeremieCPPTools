#include "WinSocketPack.h"

#include "CharArray.h"

#include "RWLock.h"

// C++11 Pthread 库
//#include <thread>
//#include <mutex>


#include <iostream>

#include <stdexcept> // 抛出错误 runtime_error
#include <vector>
#include <string>
#include <sstream>
//using namespace std;

// 初始化API
WinSocketPack::WinSocketPack() throw (std::runtime_error, bad_exception)
        : RWLPoolSocket(true)   // 初始化锁参数
        , ATMStop(false)
        , IDmain(0)
{

    // 单例 保护
    {
        int static a;
        if (a==1)
        {
            throw std::runtime_error("Cannot ctor two WinSocketPack.");
        }
        else
        {
            a = 1;
        }
    }

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // 要启动的版本号
//    wVersionRequested = MAKEWORD( 1, 1 );
    wVersionRequested = MAKEWORD( 2, 2 );   // 2.2

    // 启动库
    // WSAStartup ???
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) // 检查启动错误
    {
//        cout << "WSAStartup error" << endl;
        throw std::runtime_error("WSAStartup error");
    }

}

// 清理句柄池 关闭未关闭的Socket句柄
WinSocketPack::~WinSocketPack()
{
    cout << "WinSocketPack::~WinSocketPack()" << endl;
    {   /// ATMWGuard
        ATMWGuard atm(this->RWLPoolSocket);
        // 从后往前关闭
        while ( !this->PoolSocket.empty() )
        {
            if ( (this->PoolSocket.end() - 1)->able )       // 之前没有关闭
            {
                int info = 0;
                do{
                    info = closesocket( (this->PoolSocket.end() - 1)->sock );
                }while( info == WSAEWOULDBLOCK );       // 非阻塞套接字关闭时需要等待重试
            }
            this->PoolSocket.erase(this->PoolSocket.end() - 1 );

//            {
////                ATMWTRGuard atmtr(this->RWLPoolSocket);
////                this->CloseSocket( this->GetPoolSize() -1);
//            }

//            if ( this->PoolSocket.at(this->PoolSocket.size() -1) != 0 )       // 之前没有关闭
//            {
//                int info = 0;
//                do{
//                    info = closesocket( this->PoolSocket.at(this->PoolSocket.size() -1) );
//                }while( info == WSAEWOULDBLOCK );       // 非阻塞套接字关闭时需要等待重试
//        //        this->PoolSocket.erase( this->PoolSocket.begin() + i );       为多线程着想  不移除【erase】
//        //        移除会导致定位用下标失效
//                this->PoolSocket[this->PoolSocket.size() -1] = 0;
//            }
//            this->PoolSocket.erase(this->PoolSocket.end() - 1 );
        }
    }   /// ATMWGuard
    WSACleanup();
}

// 未对池加锁的...不允许在不对池加锁的前提下调用
size_t WinSocketPack::GetSocketIndex( size_t id)
{
    if ( id > this->GetIDSize() )
    {
        return bad_index;
    }
//    ATMRGuard atm(this->RWLPoolSocket);
    size_t i = 0;
    for ( SocketObject const &so : this->PoolSocket )
    {
        if ( id == so.id )
        {
            return i;
        }
        ++i;
    }
    return bad_index;
}

// 使用编号获取句柄
SOCKET WinSocketPack::GetSocket(size_t id) throw( overflow_error, runtime_error, bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    if ( id > this->GetIDSize() )
    {
        throw std::overflow_error( "WSP::GetSocket:: id overflow" );
    }
    for ( SocketObject const &so : this->PoolSocket )
    {
        if ( id == so.id )
        {
            return so.sock;
        }
    }
    throw std::runtime_error( "WSP::GetSocket:: id not found" );
}


// 获取当前池大小
size_t WinSocketPack::GetPoolSize() throw( bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    return this->PoolSocket.size();
}


// 获取当前最大ID大小
// dead lock waring : 没有拿到池锁的前提下不要调用此函数
size_t WinSocketPack::GetIDSize() throw( bad_exception )
{
    std::lock_guard<std::mutex> mtx(MTXID);
    return this->IDmain;
}


// 获取一个新的ID
// dead lock waring : 没有拿到池锁的前提下不要调用此函数
size_t WinSocketPack::GetID() throw( overflow_error, bad_exception )
{
    std::lock_guard<std::mutex> mtx(MTXID);
    if ( this->bad_index == IDmain )
    {
        /// 已经运行到用完size_t这个大小的程度了
        /// 但是因为size_t实在是太大【long long unsigned int】
        /// 所以不是运行几年十几年并且是超高负债的系统就不可能达到这个大小
        /// 所以 我们直接抛异常..........嗯
        /// 嗯...如果是持续运行百年的程序我们还是不要这么做吧...
        throw std::overflow_error("Bad Bad...Design Bad....\n    the ID over the Max Large.");
    }
    ++IDmain;
    return IDmain;
}

// 池是否为空
bool WinSocketPack::EmptyPool() throw( bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    return this->PoolSocket.empty();
}

// 池清扫
// dead lock waring : 双锁..
void WinSocketPack::CleaningPool()
{
    ATMWGuard atm(this->RWLPoolSocket);
    auto it = this->PoolSocket.begin();
    while ( it != this->PoolSocket.end() )
    {
        if ( false == it->able )
        {
            it = this->PoolSocket.erase(it);
        }
        else
        {
            ++it;
        }
    }
    if ( this->PoolSocket.empty() )
    {
        std::lock_guard<std::mutex> mtx(MTXID);
        IDmain = 0;
    }
    return;
}

// 关闭并删除
void WinSocketPack::DeleteSocket( size_t i)
{
    /// TODO
    return;
}


// 关闭某个Socket
void WinSocketPack::CloseSocket(size_t i) throw( out_of_range, bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    size_t index = this->GetSocketIndex( i );
    if ( index != this->bad_index )
    {
        if ( this->PoolSocket.at(index).able )       // 之前没有关闭
        {
            int info = 0;
            do{
                info = closesocket( this->PoolSocket.at(index).sock );
            }while( info == WSAEWOULDBLOCK );       // 非阻塞套接字关闭时需要等待重试
            this->PoolSocket[index].able = false;
        }
    }
}


// Shutdown
bool WinSocketPack::Shutdown(size_t i, WSP_SHUTDOWN how) throw( out_of_range, bad_exception )
{
    size_t h = ~0;
    {
        ATMRGuard atm(this->RWLPoolSocket);
        size_t index = this->GetSocketIndex( i );
        if ( index != this->bad_index )
        {
            h = this->PoolSocket.at(index).sock;
        }
        else
        {
            return false;
        }
    }
    return (shutdown( h, how) != SOCKET_ERROR) ? true : false;
}


// 创建新Socket 返回池内句柄id
size_t WinSocketPack::NewSocket(WSP_AF af, WSP_TYPE type, WSP_PROTOCOL protocol) throw( runtime_error, overflow_error, bad_exception )
{
    SOCKET socks=socket( af, type, protocol);
    if ( INVALID_SOCKET == socks)
    {
        stringstream ss("socket NewSocket INVALID_SOCKET ");
        ss << WSAGetLastError();
        throw std::runtime_error( ss.str() );
    }
    ATMWGuard atm(this->RWLPoolSocket);
    this->PoolSocket.emplace_back( socks, this->GetID());
    return this->PoolSocket.at(this->PoolSocket.size() - 1).id;
}

// 创建新Socket 返回池内句柄编号
size_t WinSocketPack::NewSocketTCP() throw( runtime_error, overflow_error, bad_exception )
{
    return this->NewSocket( WSP_AF::WSP_AF_INET, WSP_TYPE::WSP_SOCK_STREAM, WSP_PROTOCOL::WSP_IPPROTO_TCP );
}



// Bind
bool WinSocketPack::Bind(size_t i, std::string addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception )
{
    sockaddr_in addrin;
    unsigned long S_addr = inet_addr(addr.c_str());   // inet_addr失败返回 INADDR_NONE
    if ( S_addr == INADDR_NONE )
    {
        return false;
    }
    addrin.sin_addr.S_un.S_addr = S_addr;
    addrin.sin_family = family;
    addrin.sin_port = htons(port);
    int iResult = ::bind(this->GetSocket(i), (SOCKADDR *) &addrin, sizeof (addrin));
    if (iResult == SOCKET_ERROR) {
//        stringstream ss("socket Bind SOCKET_ERROR ");
//        ss << WSAGetLastError();
//        throw std::runtime_error( ss );
        return false;
    }
    return true;
}

// Bind
bool WinSocketPack::Bind(size_t i, WSP_INADDR addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception )
{
    sockaddr_in addrin;
    addrin.sin_addr.S_un.S_addr = htonl(addr);
    addrin.sin_family = family;
    addrin.sin_port = htons(port);
    int iResult = ::bind(this->GetSocket(i), (SOCKADDR *) &addrin, sizeof (addrin));
    if (iResult == SOCKET_ERROR) {
//        stringstream ss("socket Bind SOCKET_ERROR ");
//        ss << WSAGetLastError();
//        throw std::runtime_error( ss );
        return false;
    }
    return true;
}

// Listen
bool WinSocketPack::Listen(size_t i, int MaximumQueue) throw( overflow_error, runtime_error, bad_exception )
{
    if ( listen(this->GetSocket(i), MaximumQueue) == SOCKET_ERROR )
    {
        return false;
    }
    return true;
}

// SetBlockingMode
bool WinSocketPack::SetBlockingMode( size_t i, bool blocking) throw( overflow_error, runtime_error, bad_exception )
{
    // int ioctlsocket(SOCKET s,__LONG32 cmd,u_long *argp);
    // 设置 socket I/O 模式

    // 第一个参数是需要设置的 socket 句柄
    // 当第二个参数是 FIONBIO 时    第三个参数决定是否开启阻塞模式     0 阻塞开启      非0 不阻塞
    // Note: 创建的新 socket 默认是阻塞的
    u_long mode = blocking ? 0 : 1;
    if ( ioctlsocket( this->GetSocket(i), FIONBIO, &mode) == SOCKET_ERROR )
    {
        return false;
    }
    return true;
}

// Accept
size_t WinSocketPack::Accept(size_t i) throw( overflow_error, runtime_error, bad_exception )
{
    sockaddr_in addrin;
    int len=sizeof(sockaddr_in);
    SOCKET socketin = accept( this->GetSocket(i), (SOCKADDR*)&addrin, &len);
    if ( socketin == INVALID_SOCKET )
    {
        return WSP_ACCEPT_ERROR;
    }
    ATMWGuard atm(this->RWLPoolSocket);
    this->PoolSocket.emplace_back(socketin,this->GetID());
    return this->PoolSocket.at(this->PoolSocket.size() - 1).id;
}


// Connect
bool WinSocketPack::Connect(size_t i, std::string addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception )
{
    sockaddr_in addrin;
    unsigned long S_addr = inet_addr(addr.c_str());   // inet_addr失败返回 INADDR_NONE
    if ( S_addr == INADDR_NONE )
    {
        return false;
    }
    addrin.sin_addr.S_un.S_addr = S_addr;
    addrin.sin_family = family;
    addrin.sin_port = htons(port);
    int iResult = connect(this->GetSocket(i), (SOCKADDR *) &addrin, sizeof (addrin));
    if (iResult == SOCKET_ERROR) {
//        stringstream ss("socket Connect SOCKET_ERROR ");
//        ss << WSAGetLastError();
//        throw std::runtime_error( ss );
        return false;
    }
    return true;
}


// Send
bool WinSocketPack::Send(size_t i, std::string data) throw( overflow_error, runtime_error, bad_exception )
{
    if ( send( this->GetSocket(i), data.c_str(), data.size(), 0) == SOCKET_ERROR )
    {
        return false;
    }
    return true;
}

// Recv
int WinSocketPack::Recv(size_t i, std::string& data, size_t maxlength) throw( overflow_error, runtime_error, bad_exception )
{
    CharArray ca(maxlength);
    if ( this->Recv( i, ca) < 1 )
    {
        return 0;
    }
    data = ca.GetString();
    return ca.dlength;
}


// Recv
int WinSocketPack::Recv(size_t i, CharArray& data) throw( overflow_error, runtime_error, bad_exception )
{
    int r = recv( this->GetSocket(i), data.GetPtr(), data.GetSize(), 0);
    if ( r == SOCKET_ERROR )      // 失败或正常中断
    {
        return -1;
    }
//    if ( r == 0)
//    {
//        return 0;
//    }
    data.dlength = r;   // 设置接收到的数据长度
    return data.dlength;
}


