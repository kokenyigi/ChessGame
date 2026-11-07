#pragma once
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

template<typename T>
class RingBuffer
{
private:
    T* _buffer = nullptr;
    size_t _size = 0u;
    size_t _capacity = 0u;
    size_t _headIndex = 0u;
    size_t _tailIndex = 0u;
public:
    RingBuffer(){}
    ~RingBuffer()
    {
        if(_buffer != nullptr)
        {
            delete[] _buffer;
        }
    }
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    //should call only once
    void Init(size_t capacity)
    {
        if(_buffer != nullptr)
        {
            delete[] _buffer;
        }

        _buffer = new T[capacity];
        _capacity = capacity;
        _size = 0u;
        _tailIndex = 0u;
        _headIndex = 0u;
    }

    void Push(const T& element)
    {
        _buffer[_headIndex] = element;
        _headIndex = (_headIndex + 1u) % _capacity;
        ++_size;
    }

    T Pop()
    {
        size_t popIndex = _tailIndex;
        _tailIndex = (_tailIndex + 1u) % _capacity;
        --_size;
        return _buffer[popIndex];
    }

    size_t GetSize()
    {
        return _size;
    }

    bool IsEmpty(){return _size == 0u; }

    bool IsFull(){return _size == _capacity;}

    void Destroy()
    {
        if(_buffer != nullptr)
        {
            delete[] _buffer;
            _buffer = nullptr    
        }

        _capacity = 0u;
        _size = 0u;
        _tailIndex = 0u;
        _headIndex = 0u;
    }

};


#endif


