/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <drivers/virtio/VirtioDevice.h>
#include <drivers/virtio/VirtQueue.h>

VirtQueue::VirtQueue(VirtioDevice& dev) :
    freeNum_(dev.inp16(VIRTIO_PCI_QUEUE_NUM)),
    dev_(dev)
{
    ASSERT(freeNum_ > 0);
    mem_ = ContigousMemory::allocate(vring_size(freeNum_, MAP_PAGE_SIZE));
    ASSERT(mem_);

    activate(mem_->getPhysicalAddress());
    vring_init(&vring_, freeNum_, mem_->get(), MAP_PAGE_SIZE);
}

void VirtQueue::activate(uintptr_t paddr)
{
    dev_.outp32(VIRTIO_PCI_QUEUE_PFN, paddr >> VIRTIO_PCI_QUEUE_ADDR_SHIFT);
}

void VirtQueue::deactivate()
{
    dev_.outp32(VIRTIO_PCI_QUEUE_PFN, NULL);
}

VirtQueue* VirtQueue::findVirtQueue(int queueIndex, VirtioDevice& dev)
{
    // Select the queue to use.
    dev.outp16(VIRTIO_PCI_QUEUE_SEL, queueIndex);

    // How many descriptors do the queue have?
    const int numberOfDesc = dev.inp16(VIRTIO_PCI_QUEUE_NUM);
    if (numberOfDesc == 0) {
        return NULL;
    }

    // already activated?
    if (dev.inp32(VIRTIO_PCI_QUEUE_PFN)) {
        return NULL;
    }
    return new VirtQueue(dev);
}
