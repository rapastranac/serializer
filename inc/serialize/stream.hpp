#ifndef STREAM_HPP
#define STREAM_HPP

#include <memory>
#include <cstring>

namespace serializer
{

    class stream
    {
        friend class oarchive;

    public:
        stream(/* args */)
        {
            this->Bytes = 0;
        }

        stream(int Bytes)
        {
            this->Bytes = Bytes;
            delete[] buffer;
            buffer = new char[Bytes];
        }

        void allocate(int Bytes)
        {
            this->Bytes = Bytes;
            if (buffer)
                delete[] buffer;

            buffer = new char[Bytes];
        }

        char &operator*()
        {
            return *buffer;
        }

        char &operator[](int idx)
        {
            return buffer[idx];
        }

        stream(const stream &rhs)
        {
            this->Bytes = rhs.Bytes;
            this->buffer = new char[Bytes];
            std::memcpy(this->buffer, rhs.buffer, Bytes);
        }

        stream(stream &&rhs)
        {
            this->Bytes = rhs.Bytes;
            rhs.Bytes = 0;
            this->buffer = std::move(rhs.buffer);
        }

        stream &operator=(const stream &rhs)
        {
            this->Bytes = rhs.Bytes;
            this->buffer = new char[Bytes];
            std::memcpy(this->buffer, rhs.buffer, Bytes);
            return *this;
        }

        stream &operator=(stream &&rhs)
        {
            this->Bytes = rhs.Bytes;
            rhs.Bytes = 0;
            this->buffer = std::move(rhs.buffer);
            return *this;
        }

        ~stream()
        {
            if (buffer)
                delete[] buffer;
        }

        // returns buffer size in Bytes
        auto size()
        {
            return Bytes;
        }

    private:
        int Bytes;
        char *buffer = nullptr;
    };
} // namespace serializer

#endif