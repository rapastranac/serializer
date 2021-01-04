#ifndef IARCHIVE_HPP
#define IARCHIVE_HPP

#include "archive.hpp"

namespace archive
{
    class iarchive : public archive_parent
    {

    private:
        char *buffer;
        int NUM_ARGS;                       // number of arguments attached to stream
        std::vector<std::pair<int, int>> C; // temporary container to store buffer section while building stream
        int Bytes;                          // number of bytes to be contained in stream
        int arg_No;                         // index in stream

    public:
        iarchive(archive::stream &stream)
        {
            this->NUM_ARGS = 0;
            this->Bytes = 0;
            this->arg_No = 0;
            this->buffer = &stream[0];
        }

        ~iarchive() {}

        template <typename TYPE>
        iarchive &operator>>(TYPE &target)
        {
            if (NUM_ARGS == 0)
            {
                std::memcpy(&NUM_ARGS, &buffer[0], sizeof(int));
                int idx = sizeof(int);

                int arg_indx_begin = (NUM_ARGS + 1) * sizeof(int);

                for (int i = 0; i < NUM_ARGS; i++)
                {
                    int count = sizeof(int);
                    int argBytes;
                    std::memcpy(&argBytes, &buffer[idx], count);
                    idx += count;
                    C.emplace_back(std::make_pair(argBytes, arg_indx_begin));
                    arg_indx_begin += argBytes;
                }
            }
            deserialize(target);

            return *this;
        }

    private:
        /*
        integral types:         bool, char, char8_t, char16_t, char32_t, wchar_t, short, int, long, long long
        floating point types:   float, double, long double
        */
        template <typename _T,
                  std::enable_if_t<std::is_fundamental<_T>::value, bool> = true>
        void deserialize(_T &target)
        {
            int disp_unit = sizeof(_T);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            std::memcpy(&target, &buffer[start], count);
            ++arg_No;
        }

        template <typename TYPE>
        void deserialize(std::vector<TYPE> &target)
        {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            target.resize(count / disp_unit);
            std::memcpy(target.data(), &buffer[start], count);
            ++arg_No;
        }

        template <typename TYPE>
        void deserialize(std::set<TYPE> &target)
        {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int SIZE = count / disp_unit;

            for (int i = 0; i < SIZE; i++)
            {
                TYPE item;
                std::memcpy(&item, &buffer[start], disp_unit);
                target.insert(item);

                start += disp_unit;
            }
            ++arg_No;
        }

        template <typename TYPE>
        void deserialize(std::list<TYPE> &target)
        {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int SIZE = count / disp_unit;

            for (int i = 0; i < SIZE; i++)
            {
                TYPE item;
                std::memcpy(&item, &buffer[start], disp_unit);
                target.push_back(item);

                start += disp_unit;
            }
            ++arg_No;
        }

        template <typename TYPE>
        void deserialize(std::queue<TYPE> &target)
        {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int SIZE = count / disp_unit;

            for (int i = 0; i < SIZE; i++)
            {
                TYPE item;
                std::memcpy(&item, &buffer[start], disp_unit);
                target.push(item);

                start += disp_unit;
            }
            ++arg_No;
        }

        template <typename _Ty1, typename _Ty2>
        void deserialize(std::map<_Ty1, _Ty2> &target)
        {
            int disp_unit = sizeof(int);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int mapSize;

            std::memcpy(&mapSize, &buffer[start], disp_unit);

            ++arg_No;

            for (int i = 0; i < mapSize; i++)
            {
                _Ty1 key;
                deserialize(key);
                _Ty2 val;
                deserialize(val);
                target.insert(std::make_pair(key, val));
            }
        }

        void deserialize(std::string &target)
        {
            int disp_unit = sizeof(char);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            target.resize(count / disp_unit);
            std::memcpy(target.data(), &buffer[start], count);
            ++arg_No;
        }

        template <class TYPE,
                  std::enable_if_t<!is_stl_container<TYPE>::value &&
                                       !std::is_fundamental<TYPE>::value &&
                                       !std::is_same<TYPE, std::string>::value,
                                   bool> = true>
        void deserialize(TYPE &src)
        {
            src.deserialize(*this);
        }
    };
}; // namespace archive

#endif