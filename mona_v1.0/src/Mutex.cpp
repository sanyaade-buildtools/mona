/*!
    \file  Mutex.cpp
    \brief class kMutex

    Copyright (c) 2002, 2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/

#include <Mutex.h>
#include <io.h>
#include <syscalls.h>
#include <HList.h>
#include <tester.h>

/*----------------------------------------------------------------------
    KMutex
----------------------------------------------------------------------*/
KMutex::KMutex(Process* process) : process_(process)
{
    waitList_ = new HList<Thread*>();
}

KMutex::~KMutex()
{
    if (waitList_->size() != 0)
    {
        g_console->printf("KMutex has waiting threads!!\n");
    }
    delete waitList_;
}

int KMutex::lock(Thread* thread)
{
    enter_kernel_lock_mode();

    /* lock OK */
    if (!isLocked())
    {
        owner_ = thread;

    /* lock NG, so wait */
    }
    else
    {
        waitList_->add(thread);
        KEvent::wait(thread, KEvent::MUTEX_LOCKED);
    }
    exit_kernel_lock_mode();
    return NORMAL;
}

int KMutex::tryLock(Thread* thread)
{
    int result;

    /* not locked */
    if (!isLocked())
    {
        return NORMAL;
    }

    enter_kernel_lock_mode();

    /* lock OK */
    if (!isLocked())
    {
        owner_ = thread;
        result = NORMAL;
    }
    else
    {
        result = -1;
    }

    exit_kernel_lock_mode();
    return result;
}

int KMutex::unlock()
{
    /* not locked */
    if (!isLocked())
    {
        return NORMAL;
    }

    enter_kernel_lock_mode();

    if (waitList_ ->size() == 0)
    {
        owner_ = NULL;
    }
    else
    {
        owner_ = waitList_->removeAt(0);
        KEvent::set(owner_, KEvent::MUTEX_LOCKED);
    }

    exit_kernel_lock_mode();
    return NORMAL;
}

int KMutex::checkSecurity(Thread* thread)
{
    if (thread->tinfo->process != process_)
    {
        return -1;
    }

    return 0;
}
