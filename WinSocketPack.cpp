#include "WinSocketPack.h"

#include "CharArray.h"

#include "RWLock.h"

// C++11 Pthread ��
//#include <thread>
//#include <mutex>


#include <iostream>

#include <stdexcept> // �׳����� runtime_error
#include <vector>
#include <string>
#include <sstream>
//using namespace std;

// ��ʼ��API
WinSocketPack::WinSocketPack() throw (std::runtime_error, bad_exception)
        : RWLPoolSocket(true)   // ��ʼ��������
        , ATMStop(false)
        , IDmain(0)
{

    // ���� ����
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

    // Ҫ�����İ汾��
//    wVersionRequested = MAKEWORD( 1, 1 );
    wVersionRequested = MAKEWORD( 2, 2 );   // 2.2

    // ������
    // WSAStartup ???
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) // �����������
    {
//        cout << "WSAStartup error" << endl;
        throw std::runtime_error("WSAStartup error");
    }

}

// �������� �ر�δ�رյ�Socket���
WinSocketPack::~WinSocketPack()
{
    cout << "WinSocketPack::~WinSocketPack()" << endl;
    {   /// ATMWGuard
        ATMWGuard atm(this->RWLPoolSocket);
        // �Ӻ���ǰ�ر�
        while ( !this->PoolSocket.empty() )
        {
            if ( (this->PoolSocket.end() - 1)->able )       // ֮ǰû�йر�
            {
                int info = 0;
                do{
                    info = closesocket( (this->PoolSocket.end() - 1)->sock );
                }while( info == WSAEWOULDBLOCK );       // �������׽��ֹر�ʱ��Ҫ�ȴ�����
            }
            this->PoolSocket.erase(this->PoolSocket.end() - 1 );

//            {
////                ATMWTRGuard atmtr(this->RWLPoolSocket);
////                this->CloseSocket( this->GetPoolSize() -1);
//            }

//            if ( this->PoolSocket.at(this->PoolSocket.size() -1) != 0 )       // ֮ǰû�йر�
//            {
//                int info = 0;
//                do{
//                    info = closesocket( this->PoolSocket.at(this->PoolSocket.size() -1) );
//                }while( info == WSAEWOULDBLOCK );       // �������׽��ֹر�ʱ��Ҫ�ȴ�����
//        //        this->PoolSocket.erase( this->PoolSocket.begin() + i );       Ϊ���߳�����  ���Ƴ���erase��
//        //        �Ƴ��ᵼ�¶�λ���±�ʧЧ
//                this->PoolSocket[this->PoolSocket.size() -1] = 0;
//            }
//            this->PoolSocket.erase(this->PoolSocket.end() - 1 );
        }
    }   /// ATMWGuard
    WSACleanup();
}

// δ�Գؼ�����...�������ڲ��Գؼ�����ǰ���µ���
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

// ʹ�ñ�Ż�ȡ���
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


// ��ȡ��ǰ�ش�С
size_t WinSocketPack::GetPoolSize() throw( bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    return this->PoolSocket.size();
}


// ��ȡ��ǰ���ID��С
// dead lock waring : û���õ�������ǰ���²�Ҫ���ô˺���
size_t WinSocketPack::GetIDSize() throw( bad_exception )
{
    std::lock_guard<std::mutex> mtx(MTXID);
    return this->IDmain;
}


// ��ȡһ���µ�ID
// dead lock waring : û���õ�������ǰ���²�Ҫ���ô˺���
size_t WinSocketPack::GetID() throw( overflow_error, bad_exception )
{
    std::lock_guard<std::mutex> mtx(MTXID);
    if ( this->bad_index == IDmain )
    {
        /// �Ѿ����е�����size_t�����С�ĳ̶���
        /// ������Ϊsize_tʵ����̫��long long unsigned int��
        /// ���Բ������м���ʮ���겢���ǳ��߸�ծ��ϵͳ�Ͳ����ܴﵽ�����С
        /// ���� ����ֱ�����쳣..........��
        /// ��...����ǳ������а���ĳ������ǻ��ǲ�Ҫ��ô����...
        throw std::overflow_error("Bad Bad...Design Bad....\n    the ID over the Max Large.");
    }
    ++IDmain;
    return IDmain;
}

// ���Ƿ�Ϊ��
bool WinSocketPack::EmptyPool() throw( bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    return this->PoolSocket.empty();
}

// ����ɨ
// dead lock waring : ˫��..
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

// �رղ�ɾ��
void WinSocketPack::DeleteSocket( size_t i)
{
    /// TODO
    return;
}


// �ر�ĳ��Socket
void WinSocketPack::CloseSocket(size_t i) throw( out_of_range, bad_exception )
{
    ATMRGuard atm(this->RWLPoolSocket);
    size_t index = this->GetSocketIndex( i );
    if ( index != this->bad_index )
    {
        if ( this->PoolSocket.at(index).able )       // ֮ǰû�йر�
        {
            int info = 0;
            do{
                info = closesocket( this->PoolSocket.at(index).sock );
            }while( info == WSAEWOULDBLOCK );       // �������׽��ֹر�ʱ��Ҫ�ȴ�����
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


// ������Socket ���س��ھ��id
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

// ������Socket ���س��ھ�����
size_t WinSocketPack::NewSocketTCP() throw( runtime_error, overflow_error, bad_exception )
{
    return this->NewSocket( WSP_AF::WSP_AF_INET, WSP_TYPE::WSP_SOCK_STREAM, WSP_PROTOCOL::WSP_IPPROTO_TCP );
}



// Bind
bool WinSocketPack::Bind(size_t i, std::string addr, int port, WSP_AF family) throw( overflow_error, runtime_error, bad_exception )
{
    sockaddr_in addrin;
    unsigned long S_addr = inet_addr(addr.c_str());   // inet_addrʧ�ܷ��� INADDR_NONE
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
    // ���� socket I/O ģʽ

    // ��һ����������Ҫ���õ� socket ���
    // ���ڶ��������� FIONBIO ʱ    ���������������Ƿ�������ģʽ     0 ��������      ��0 ������
    // Note: �������� socket Ĭ����������
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
    unsigned long S_addr = inet_addr(addr.c_str());   // inet_addrʧ�ܷ��� INADDR_NONE
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
    if ( r == SOCKET_ERROR )      // ʧ�ܻ������ж�
    {
        return -1;
    }
//    if ( r == 0)
//    {
//        return 0;
//    }
    data.dlength = r;   // ���ý��յ������ݳ���
    return data.dlength;
}


