#ifndef OARCHIVE_HPP
#define OARCHIVE_HPP

#include "archive.hpp"

namespace archive
{
    class oarchive : public archive_parent
    {

    private:
        archive::stream *stream = nullptr;     // pointer to stream
        int NUM_ARGS;                          // Number of arguments attached to stream
        std::vector<std::pair<int, char *>> C; // temporary container to store buffer section while building stream
        int Bytes;                             // number of bytes to be contained in stream

    public:
        oarchive(archive::stream &stream)
        {
            this->NUM_ARGS = 0;
            this->Bytes = 0;
            this->stream = &stream;
        }

        ~oarchive()
        {
            for (int i = 0; i < C.size(); i++)
            {
                delete[] C[i].second;
            }
        }

        template <typename TYPE>
        oarchive &operator<<(TYPE &src)
        {
            serialize(src);
            finishBuffer();
            return *this;
        }

    private:
        void createBuffer()
        {
            int bytes = (NUM_ARGS + 1) * sizeof(int);
            Bytes += bytes;
            stream->allocate(Bytes);
            int position = 0;
            int count = sizeof(int);
            std::memcpy(&stream[position], &NUM_ARGS, count);
            position += count;

            for (int i = 0; i < C.size(); i++)
            {
                count = sizeof(int);
                std::memcpy(&stream[position], &C[i].first, count);
                position += count;
            }

            for (int i = 0; i < C.size(); i++)
            {
                count = C[i].first;
                std::memcpy(&stream[position], C[i].second, count);
                position += count;
            }
        }

        void finishBuffer()
        {
            int counter = 0;
            int count;

            int firstBytes = (NUM_ARGS + 1) * sizeof(int);
            stream->allocate(Bytes + firstBytes);
            char *buf = &stream->operator[](0); //pointer to buffer in stream

            std::memcpy(&buf[0], &NUM_ARGS, sizeof(int)); //NUM_ARGS copied into stream
            counter += sizeof(int);

            for (int i = 0; i < NUM_ARGS; i++)
            {
                count = sizeof(int);
                std::memcpy(&buf[counter], &C[i].first, count); //Bytes count copied
                counter += sizeof(int);
            }

            for (size_t i = 0; i < NUM_ARGS; i++)
            {
                count = C[i].first;
                std::memcpy(&buf[counter], C[i].second, count); //actual Bytes copied
                counter += C[i].first;
            }
        }

        /*
        integral types:         bool, char, char8_t, char16_t, char32_t, wchar_t, short, int, long, long long
        floating point types:   float, double, long double
        */
        template <typename _T,
                  std::enable_if_t<std::is_fundamental<_T>::value, bool> = true>
        void serialize(const _T &src)
        {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(_T);
            int count = disp_unit * 1;
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(C.back().second, &src, count);
        }

        template <typename TYPE>
        void serialize(const std::vector<TYPE> &src)
        {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(C.back().second, src.data(), count);
        }

        template <typename TYPE>
        void serialize(const std::set<TYPE> &src)
        {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));

            auto it = src.begin();
            int idx = 0;
            while (it != src.end())
            {
                std::memcpy(&C.back().second[idx], &*it, disp_unit);
                ++it;
                idx += disp_unit;
            }
        }

        template <typename TYPE>
        void serialize(const std::list<TYPE> &src)
        {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));

            auto it = src.begin();
            int idx = 0;
            while (it != src.end())
            {
                std::memcpy(&C.back().second[idx], &*it, disp_unit);
                ++it;
                idx += disp_unit;
            }
        }

        template <typename TYPE>
        void serialize(const std::queue<TYPE> &src)
        {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));

            auto srcCpy = src;
            int idx = 0;
            while (!srcCpy.empty())
            {
                std::memcpy(&C.back().second[idx], &srcCpy.front(), disp_unit);
                srcCpy.pop();
                idx += disp_unit;
            }
        }

        template <typename _Ty1, typename _Ty2>
        void serialize(const std::map<_Ty1, _Ty2> &src)
        {
            /*
                for maps, an element telling the size of the upcoming map is inserted
            */
            ++this->NUM_ARGS;
            int size = src.size();
            int disp_unit = sizeof(int);
            int count = disp_unit * 1;
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(&C.back().second[0], &size, disp_unit);

            for (auto const &[key, val] : src)
            {
                serialize(key);
                finishBuffer();
                serialize(val);
                finishBuffer();
            }

            printf("Hello line\n");
        }

        void serialize(std::string &src)
        {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(char);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(C.back().second, src.c_str(), count);
        }

        template <class TYPE,
                  std::enable_if_t<!is_stl_container<TYPE>::value &&
                                       !std::is_fundamental<TYPE>::value &&
                                       !std::is_same<TYPE, std::string>::value,
                                   bool> = true>
        void serialize(TYPE &src)
        {
            src.serialize(*this);
        }
    };

}; // namespace archive

#endif