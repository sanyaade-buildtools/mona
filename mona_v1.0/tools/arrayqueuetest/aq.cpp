#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "types.h"

template <class T> class Array;

/*
    main()
*/
int main(int argc, char** argv)
{
    Array<int> array(10);

    for (int i = 0; i < array.getLength(); i++)
    {
        array[i] = i;
    }

    for (int i = 0; i < array.getLength(); i++)
    {
        printf("[%d]", array[i]);
    }
    return 0;
}

#if 1 // tino's for test
template <class T> struct Array
{
protected:
    T* pointer;
    int length;
    int* count;

private:
    inline void Initialize()
    {
        this->pointer = 0 /*NULL*/;
        this->length = 0;
        this->count  = 0 /*NULL*/;
    }

public:
    Array()
    {
        this->Initialize();
    }

    Array(int length)
    {
        this->Initialize();
        this->Set(new T[length], length);
    }

    Array(T* pointer, int length, bool isManaged = true)
    {
        this->Initialize();
        this->Set(pointer, length, isManaged);
    }

    Array(const Array<T>& pointer)
    {
        this->Initialize();
        this->Set(pointer);
    }

    virtual ~Array()
    {
        this->Unset();
    }

    void Set(T* pointer, int length, bool isManaged = true)
    {
        this->Unset();

        this->pointer = pointer;
        this->length = length;
        if (this->pointer != 0 /*NULL*/ && isManaged) this->count = new int(0);
    }

    void Set(const Array<T>& pointer)
    {
        this->Unset();

        this->pointer = pointer.pointer;
        this->length = pointer.length;
        this->count  = pointer.count;
        if (this->count != 0 /*NULL*/) (*this->count)++;
    }

    void Clear()
    {
        this->Initialize();
    }

    void Unset()
    {
        if (this->count != 0 /*NULL*/)
        {
            if (*this->count == 0)
            {
                delete this->count;
                this->count = 0 /*NULL*/;
                delete [] this->pointer;
            }
            else
            {
                (*this->count)--;
                this->count = 0 /*NULL*/;
            }
            this->pointer = 0 /*NULL*/;
            this->length = 0;
        }
    }

    inline T* get_Pointer() { return this->pointer; }
    inline int getLength() { return this->length; }
    inline int get_Count() const { return this->count; }

    inline bool operator ==(T* arg) const { return this->pointer == arg; }
    inline bool operator !=(T* arg) const { return this->pointer != arg; }
    inline bool operator ==(const Array<T>& arg) const { return this->pointer == arg.pointer; }
    inline bool operator !=(const Array<T>& arg) const { return this->pointer != arg.pointer; }

    inline Array<T>& operator =(const Array<T>& pointer)
    {
        this->Set(pointer);
        return *this;
    }

    inline T& operator [](int index)
    {
#ifdef DEBUG_MODE
        if (this->pointer == 0 /*NULL*/ || index < 0 || this->length - 1 < index)
        {
            // DIE...
        }
#endif
        return this->pointer[index];
    }
};
#endif
/*----------------------------------------------------------------------
    Array
----------------------------------------------------------------------*/
#if 0
#define FOREACH(type, iterator, array) \
    if ((array).getLength() > 0) \
        for ({int __i = 0; type iterator;} \
            __i < (array).getLength() && (&(iterator = (array)[__i]) || true); __##i++)
#define STACK_ARRAY(type, name, size) type __##name[size]; Array<type> name(__##name, size)

template <class T> class Array {

  public:
    Array(dword length) : length_(length), alloc_(true) {
        array_ = new T[length];
    }

    Array(T* array, dword length) : array_(array), length_(length), alloc_(false) {
    }

    virtual ~Array() {
        if (alloc_) {
            delete[] array_;
        }
    }

  public:
    inline T& operator [](dword index) {

        if (index < 0 || index > length_ - 1) {
            g_console->printf("array index outof range %d\n", index);
            for(;;);
        }
        return array_[index];
    }

    inline int getLength() const {
        return length_;
    }

  private:
    T* array_;
    dword length_;
    bool alloc_;
};
#endif

class Queue
{
public:
    static void initialize(Queue* queue);
    static void addToNext(Queue* p, Queue* q);
    static void addToPrev(Queue* p, Queue* q);
    static void remove(Queue* p);
    static bool isEmpty(Queue* p);
    static Queue* removeNext(Queue* p);
    static Queue* top(Queue* root);
public:
    Queue* next;
    Queue* prev;
};

void Queue::initialize(Queue* queue)
{
    queue->prev = queue;
    queue->next = queue;
}

void Queue::addToNext(Queue* p, Queue* q)
{
    q->next = p->next;
    q->prev = p;
    p->next->prev = q;
    p->next = q;
}

void Queue::addToPrev(Queue* p, Queue* q)
{
    q->prev = p->prev;
    q->next = p;
    p->prev->next = q;
    p->prev = q;
}

void Queue::remove(Queue* p)
{
    p->prev->next = p->next;
    p->next->prev = p->prev;
}

bool Queue::isEmpty(Queue* p)
{
    return (p->next == p);
}

Queue* Queue::removeNext(Queue* p)
{
    Queue* result = p->next;
    p->next = result->next;
    result->next->prev = p;
    return result;
}

Queue* Queue::top(Queue* root)
{
    return root->next;
}
