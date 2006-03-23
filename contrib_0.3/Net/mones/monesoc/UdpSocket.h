/*!
    \file  UdpSocket.h
    \brief UdpSocketクラス ヘッダ

    Copyright (c) 2005 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2005/08/09 update:$Date$
*/

#include <sys/types.h>
#include <sys/HashMap.h>
#include <monapi.h>
#include <monapi/cmemoryinfo.h>
#include <monalibc/stdio.h>

#include <monesoc/SocketContainer.h>

#ifndef _MONA_UDPSOCKET
#define _MONA_UDPSOCKET



/*!
    UdpSocket class
*/
class UdpSocket
{
    
  public:
    
    //コンストラクタ
    UdpSocket();
    ~UdpSocket();
    
    //bind
    int bind(word port);
    
    //recv
    int recv(char* buff ,dword len);

    //send
    int send(char* mesg, dword len, char* host, word port);

  private:
    word useport;       //暗黙的に使うポート

};

#endif
