//$Id$
#include "IPStack.h"
using namespace mones;
using namespace MonAPI;

IPStack::IPStack() : 
    next_port(0),timerid(0),
    started(false), loopExit(false)
{
    syscall_get_io();  
    this->myID = System::getThreadID();
    timerid=set_timer(5000);    
    pDP= new Dispatch();
    if(!pDP->initialize()){
        printf("initalize failed\n");
        delete pDP;
        exit(1);
    }
}

IPStack::~IPStack()
{
    if( pDP!=NULL )
        delete pDP;
    if( timerid != 0)
        kill_timer(timerid);
}

void IPStack::messageLoop()
{
    this->started = true;
    for (MessageInfo msg; !loopExit;){
        if (Message::receive(&msg)) continue;
        switch (msg.header)
        {
        case MSG_INTERRUPTED:
            pDP->interrupt();
            break;
        case MSG_NET_GETFREEPORT:
            this->getfreeport(&msg);
            break;
        case MSG_NET_WRITE:
            this->write(&msg);
            break;
        case MSG_NET_READ:
            this->read(&msg);
            break;
        case MSG_NET_STATUS:
            this->status(&msg);
            break;
        case MSG_NET_OPEN:
            this->open(&msg);
            break;
        case MSG_NET_CLOSE:
            this->close(&msg);
            break;
        case MSG_NET_CONFIG:
            this->config(&msg);
            break;
        case MSG_TIMER:    
            pDP->PeriodicUpdate();
            break;
        default:
            printf("IPStack::MessageLoop default MSG=%x\n", msg.header);
            break;
        }
    }
    printf("IPStack exit\n");
}

//////////// MESSAGE HANDLERS ////////////////////////
void IPStack::config(MessageInfo* msg)
{
    printf("re-configure the net server.\n");
    Message::reply(msg);
}

void IPStack::getfreeport(MessageInfo* msg)
{
    next_port++;
    if( next_port <= 0x400)
        next_port=0x401;
    Message::reply(msg,next_port);
}

void IPStack::open(MessageInfo* msg)
{
    ConnectionInfo* c=NULL;
    switch(msg->arg3)
    {
    case TYPEICMP:
        ICMPCoInfo* pI=new ICMPCoInfo(pDP);
        pI->type=ECHOREQUEST;
        pI->seqnum=0;
        pI->idnum=0;
        pDP->AddConnection(pI);
        //cinfolist.add(pI);
        c=pI;
        break;
    case TYPEUDP:
        UDPCoInfo* pU = new UDPCoInfo(pDP);
        pDP->AddConnection(pU);
        //cinfolist.add(pU);
        c=pU;
        break;
    case TYPETCP:
        TCPCoInfo* pT=new TCPCoInfo(pDP);
        pDP->AddConnection(pT);
        //cinfolist.add(pT);
        c=pT;
        break;
    default:
        printf("orz\n");
    }
    c->remoteip   = msg->arg1; 
    c->remoteport = (word)(msg->arg2&0x0000FFFF);
    c->localport  = (word)(msg->arg2>>16);
    c->clientid   = msg->from;    
    c->netdsc     = pDP->ConnectionNum();  
    Message::reply(msg, c->netdsc);
}

void IPStack::close(MessageInfo* msg)
{
    dword ret=1;
    for(int i=0; i< pDP->ConnectionNum(); i++){
        ConnectionInfo* c  = pDP->GetConnection(i);
        if( c->netdsc == msg->arg1 ){
            delete pDP->RemoveConnection(i);
            i--;
            ret=0;
        }
    }
    Message::reply(msg,ret);
}

void IPStack::status(MessageInfo* msg)
{
    NetStatus stat;
    pDP->readStatus(&stat);
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
    if (mi != NULL){
        monapi_cmemoryinfo_create(mi, sizeof(NetStatus)/*+sizeof(arpcache)*N*/, true);        
        if( mi != NULL ){
            memcpy(mi->Data,&stat,mi->Size);
            Message::reply(msg, mi->Handle, mi->Size);
        }
        monapi_cmemoryinfo_delete(mi);
    }else{
        Message::reply(msg);
    }
}

void IPStack::read(MessageInfo* msg)
{
    //Register msg to waiting client list.
    for(int i=0; i<pDP->ConnectionNum(); i++){
        ConnectionInfo* c  = pDP->GetConnection(i);
        if( c->netdsc == msg->arg1 ){
            memcpy(&(c->msg),(byte*)msg,sizeof(MessageInfo));
            break;
        }
    }
    pDP->DoDispatch();
}

void IPStack::write(MessageInfo* msg)
{ 
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = msg->arg2;
        ret->Owner  = msg->from;
        ret->Size   = msg->arg3;
        monapi_cmemoryinfo_map(ret);
        for(int i=0; i<pDP->ConnectionNum(); i++){
            ConnectionInfo* cinfo  = pDP->GetConnection(i);
            if( cinfo->netdsc == msg->arg1 ){    
                pDP->Send(ret->Data,ret->Size,cinfo);
                break;
            }
        }
        monapi_cmemoryinfo_delete(ret);
    }
    Message::reply(msg);  
}