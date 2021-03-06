/*!
    \file  FDCDriver.cpp
    \brief class Floppy Disk Controller for MultiTask
    \brief remove from kernel and add to file server

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2003/02/07 update:$Date$
*/

#include "FDCDriver.h"

using namespace MonAPI;

/*!
    \brief Constructer

    \author Higepon
    \date   create:2003/02/03 update:2004/12/27
*/
FDCDriver::FDCDriver() : motorCount(0), currentTrack(-1)
{
    initilize();
    return;
}

/*!
    \brief Destructer

    \author Higepon
    \date   create:2003/02/03 update:2004/12/27
*/
FDCDriver::~FDCDriver()
{
    motor(OFF);
    monapi_deallocate_dma_memory(dmabuff, 4096);
    return;
}

/*!
    \brief initilize controller

    \author Higepon
    \date   create:2003/02/03 update:2004/12/27
*/
void FDCDriver::initilize()
{
    uint8_t specifyCommand[] = {FDC_COMMAND_SPECIFY
                           , 0xC1 /* SRT = 4ms HUT = 16ms */
                           , 0x10 /* HLT = 16ms DMA       */
                            };

    /* allocate dma buffer */
    dmabuff = monapi_allocate_dma_memory(4096);

    /* do not over 64KB align */
    if (dmabuff == NULL || (uint32_t)dmabuff % 64 * 1024)
    {
        _printf("FDDriver:dma buffer allocate error\n");
        return;
    }

    /* setup DMAC */
    outp8(0xda, 0x00);
    delay(1);
    outp8(0x0d, 0x00);
    delay(1);
    outp8(0xd0, 0x00);
    delay(1);
    outp8(0x08, 0x00);
    delay(1);
    outp8(0xd6, 0xc0);
    delay(1);
    outp8(0x0b, 0x46);
    delay(1);
    outp8(0xd4, 0x00);
    delay(1);

    outp8(FDC_CCR_PRIMARY, 0);

    motor(ON);

    /* specify */
    if (!sendCommand(specifyCommand, sizeof(specifyCommand)))
    {
        logprintf("Specify command failed\n");
        motorAutoOff();
        return;
    }

    motorAutoOff();
    return;
}

int FDCDriver::open()
{
    return 0;
}

int FDCDriver::close()
{
    return 0;
}

int FDCDriver::read(uint32_t lba, void* buf, int size)
{
    return this->read(lba, (uint8_t*)buf) ? 0 : -1;
}

int FDCDriver::write(uint32_t lba, void* buf, int size)
{
    return this->write(lba, (uint8_t*)buf) ? 0 : -1;
}

/*!
    \brief wait interrupt

    \author Higepon
    \date   create:2003/02/10 update:2004/12/27
*/
bool FDCDriver::waitInterrupt()
{
    return waitInterrupt(2000, 6);
}


bool FDCDriver::waitInterrupt(uint32_t ms, uint8_t irq)
{
    MessageInfo msg;

    uint32_t timerId = set_timer(ms);

    for (int i = 0; ; i++)
    {
        int result = MonAPI::Message::peek(&msg, i);

        if (result != M_OK)
        {
            i--;
            syscall_mthread_yield_message();
        }
        else if (msg.header == MSG_TIMER)
        {
            if (msg.arg1 != timerId) continue;
            kill_timer(timerId);

            if (MonAPI::Message::peek(&msg, i, PEEK_REMOVE) != M_OK) {
                monapi_fatal("peek error %s:%d\n", __FILE__, __LINE__);
            }

            return false;
        }
        else if (msg.header == MSG_INTERRUPTED)
        {
            if (msg.arg1 != irq) continue;
            kill_timer(timerId);

            if (MonAPI::Message::peek(&msg, i, PEEK_REMOVE) != M_OK) {
                monapi_fatal("peek error %s:%d\n", __FILE__, __LINE__);
            }

            return true;
        }
    }
    return false;
}

/*!
    \brief print status of FDC

    \param  on ON/OFF
    \author Higepon
    \date   create:2003/02/10 update:2004/12/27
*/
void FDCDriver::motor(bool on)
{
    if (on)
    {
        motorCount++;
        outp8(FDC_DOR_PRIMARY, FDC_START_MOTOR);
        delay(4);
    }
    else
    {
        outp8(FDC_DOR_PRIMARY, FDC_STOP_MOTOR);
    }
    return;
}

/*!
    \brief print status of FDC

    \author Higepon
    \date   create:2004/02/10 update:2004/12/27
*/
void FDCDriver::motorAutoOff()
{
    motorCount--;
    if (motorCount <= 0)
    {
        motor(OFF);
    }
}

/*!
    \brief send command to FDC

    \param  command array of command
    \param  length  length of command
    \author Higepon
    \date   create:2003/02/16 update:2004/12/27
*/
bool FDCDriver::sendCommand(const uint8_t* command, const uint8_t length)
{
    /* send command */
    for (int i = 0; i < length; i++)
    {
        waitStatus(0x80 | 0x40, 0x80);

        /* send command */
        outp8(FDC_DR_PRIMARY, command[i]);
    }

    return true;
}

/*!
    \brief recalibrate

    \return true OK/false command fail
    \author Higepon
    \date   create:2003/02/10 update:2004/12/27
*/
bool FDCDriver::recalibrate()
{
    uint8_t command[] = {0x07, 0x00}; /* recalibrate */

    if (!sendCommand(command, sizeof(command)))
    {
        logprintf("FDCDriver#recalibrate:command fail\n");
        return false;
    }

    while (true)
    {
        waitInterrupt();

        waitStatus(0x10, 0x00);

        if (senseInterrupt()) break;
    }
    return true;
}

/*!
    \brief wait for FDC status

    \param  expected wait until msr == expected
    \author Higepon
    \date   create:2003/09/19 update:2004/12/27
*/
void FDCDriver::waitStatus(uint8_t expected)
{
    uint8_t status;

    do
    {
        status = inp8(FDC_MSR_PRIMARY);

    } while (status != expected);
}

/*!
    \brief wait for FDC status

    \param  expected wait until (msr & mask) == expected
    \param  mask     wait until (msr & mask) == expected
    \author Higepon
    \date   create:2003/09/19 update:2004/12/27
*/
void FDCDriver::waitStatus(uint8_t mask, uint8_t expected)
{
    uint8_t status;

    do
    {
        status = inp8(FDC_MSR_PRIMARY);

    } while ((status & mask) != expected);
}

/*!
    \brief get result of result phase

    \return result
    \author Higepon
    \date   create:2003/09/19 update:2004/12/27
*/
uint8_t FDCDriver::getResult()
{
    waitStatus(0xd0, 0xd0);
    return inp8(FDC_DR_PRIMARY);
}

/*!
    \brief seek

    \param  track
    \return true OK/false time out
    \author Higepon
    \date   create:2003/02/11 update:2004/12/27
*/
bool FDCDriver::seek(uint8_t track)
{
    uint8_t command[] = {FDC_COMMAND_SEEK, 0, track};

    if (currentTrack == track)
    {
        return true;
    }

    if (!sendCommand(command, sizeof(command)))
    {
        logprintf("FDCDriver#:seek command fail\n");
        return false;
    }

    while (true)
    {
        waitInterrupt();
        waitStatus(0x10, 0x00);

        if (senseInterrupt()) break;
    }

    currentTrack = track;
    return true;
}

/*!
    \brief Sense Interrrupt Command

    \author Higepon
    \date   create:2003/02/13 update:2004/12/27
*/
bool FDCDriver::senseInterrupt()
{
    uint8_t command[] = {FDC_COMMAND_SENSE_INTERRUPT};

    if (!sendCommand(command, sizeof(command)))
    {
        logprintf("FDCDriver#senseInterrrupt:command fail\n");
        return false;
    }

    results[0] = getResult(); /* ST0 */
    results[1] = getResult(); /* PCN */

    if ((results[0] & 0xC0) != 0x00) return false;

    return true;
}

/*!
    \brief start dma

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::startDMA()
{
    /* mask channel2 */
    outp8(FDC_DMA_S_SMR, 0x02);
    return;
}

/*!
    \brief stop dma

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::stopDMA()
{
    /* unmask channel2 */
    outp8(FDC_DMA_S_SMR, 0x06);
    return;
}

/*!
    \brief setup dmac for read

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::setupDMARead(uint32_t size)
{
    size--; /* size should be */
    uint32_t p = (uint32_t)dmabuff;

    stopDMA();

    /* direction write */
    outp8(FDC_DMA_S_MR, 0x46);

    /* clear uint8_t pointer */
    outp8(FDC_DMA_S_CBP, 0);
    outp8(FDC_DMA_S_BASE,  uint8_t(p & 0xff));
    outp8(FDC_DMA_S_BASE,  uint8_t((p >> 8) & 0xff));
    outp8(FDC_DMA_S_COUNT, uint8_t(size & 0xff));
    outp8(FDC_DMA_S_COUNT, uint8_t(size >>8));
    outp8(FDC_DMA_PAGE2  , uint8_t((p >>16)&0xFF));

    startDMA();
    return;
}

/*!
    \brief setup dmac for write

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::setupDMAWrite(uint32_t size)
{
    size--;
    uint32_t p = (uint32_t)dmabuff;

    stopDMA();

    /* direction read */
    outp8(FDC_DMA_S_MR, 0x4a);

    /* clear uint8_t pointer */
    outp8(FDC_DMA_S_CBP, 0);
    outp8(FDC_DMA_S_BASE,  uint8_t(p & 0xff));
    outp8(FDC_DMA_S_BASE,  uint8_t((p >> 8) & 0xff));
    outp8(FDC_DMA_S_COUNT, uint8_t(size & 0xff));
    outp8(FDC_DMA_S_COUNT, uint8_t(size >>8));
    outp8(FDC_DMA_PAGE2  , uint8_t((p >>16)&0xFF));

    startDMA();
    return;
}

/*!
    \brief disk read

    \param track  track
    \param head   head
    \param sector sector

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::read(uint8_t track, uint8_t head, uint8_t sector)
{
    uint8_t command[] = {FDC_COMMAND_READ
                   , (head & 1) << 2
                   , track
                   , head
                   , sector
                   , 0x02
                   , 0x12 // EOT osask(0x7e, 0x01, 0xff)
                   , 0x1B // GSL
                   , 0xFF // DTL vmware hate 0x00
                   };
    if (!seek(track))
    {
        logprintf("read#seek:error");
        return false;
    }

    setupDMARead(512);

    if (!sendCommand(command, sizeof(command)))
    {
        logprintf("read#send command:error\n");
        return false;
    }

    if (!waitInterrupt()) {
        stopDMA();
        return false;
    }
    for (int i = 0; i < 7; i++)
    {
        results[i] = getResult();
    }
    stopDMA();
    if ((results[0] & 0xC0) != 0x00) return false;
    return true;
}

/*!
    \brief disk write

    \param track  track
    \param head   head
    \param sector sector

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::write(uint8_t track, uint8_t head, uint8_t sector)
{
    uint8_t command[] = {0xC5//FDC_COMMAND_WRITE
                   , (head & 1) << 2
                   , track
                   , head
                   , sector
                   , 0x02
                   , 0x12
                   , 0x1b
                   , 0x00
                   };
    setupDMAWrite(512);

    seek(track);

    sendCommand(command, sizeof(command));
    waitInterrupt();

    stopDMA();

    for (int i = 0; i < 7; i++)
    {
        results[i] = getResult();
    }

    if ((results[0] & 0xC0) != 0x00) return false;

    return true;
}

/*!
    \brief disk read

    \param lba    logical block address
    \param buf    read result buffer 512uint8_t

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::read(uint32_t lba, uint8_t* buf)
{
    uint8_t track, head, sector;

    lbaToTHS(lba, track, head, sector);

    /* read. if error, retry 10 times */
    for (int i = 0; i < 10; i++)
    {
        if (read(track, head, sector))
        {
            memcpy(buf, dmabuff, 512);
            return true;
        }
    }

    return false;
}

/*!
    \brief disk write

    \param lba    logical block address
    \param buf    write result buffer 512uint8_t

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
bool FDCDriver::write(uint32_t lba, uint8_t* buf)
{
    uint8_t track, head, sector;

    lbaToTHS(lba, track, head, sector);

    memcpy(dmabuff, buf,  512);

    /* write. if error, retry 10 times */
    for (int i = 0; i < 10; i++)
    {
        if (write(track, head, sector)) return true;
    }

    return false;
}

/*!
    \brief disk read

    \param lba    logical block address
    \param track  track
    \param head   head
    \param sector sector

    \author Higepon
    \date   create:2003/02/15 update:2004/12/27
*/
void FDCDriver::lbaToTHS(int lba, uint8_t& track, uint8_t& head, uint8_t& sector)
{
    track = lba / (2 * 18);
    head = (lba / 18) % 2;
    sector = 1 + lba % 18;
    return;
}

int FDCDriver::ioctl(void* p)
{
    return checkDiskChange() ? 1: 0;
}

bool FDCDriver::checkDiskChange()
{
    currentTrack = -1;
    motor(true);
    recalibrate();
    bool changed = (inp8(0x3f7) & 0x80);
    motorAutoOff();
    return changed;
}

bool FDCDriver::isInserted(int drive)
{
    // now we don't drive parameters
    bool result = read((uint8_t)0, (uint8_t)0, (uint8_t)2);
    return result;
}
