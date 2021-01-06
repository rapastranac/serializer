#ifndef IARCHIVE_HPP
#define IARCHIVE_HPP

#include "archive.hpp"

namespace serializer {
    class iarchive : protected archive {
        friend class archive;

    private:
        std::vector<std::pair<int, int>> C; // temporary container to store argument location in buffer
        int arg_No;                         // index in stream

    public:
        //https://stackoverflow.com/a/23647515/5248548
        explicit iarchive(serializer::stream &stream) : archive(stream) {
            this->arg_No = 0;
        }

        ~iarchive() override {}

        template<typename TYPE>
        iarchive &operator>>(TYPE &target) {
            if (NUM_ARGS == 0) {
                std::memcpy(&NUM_ARGS, &strm[0], sizeof(int));
                int idx = sizeof(int);

                int arg_idx_begin = (NUM_ARGS + 1) * sizeof(int);

                for (int i = 0; i < NUM_ARGS; i++) {
                    int count = sizeof(int);
                    int argBytes;
                    std::memcpy(&argBytes, &strm[idx], count);
                    idx += count;
                    C.emplace_back(std::make_pair(argBytes, arg_idx_begin));
                    arg_idx_begin += argBytes;
                }
            }
            deserialize(target);

            return *this;
        }

    protected:
        /*
         * fundamental types
         * */
        template<typename _T,
                std::enable_if_t<std::is_fundamental<_T>::value, bool> = true>
        void deserialize(_T &target) {
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            std::memcpy(&target, &strm[start], count);
            ++arg_No;
        }

        template<typename TYPE>
        void deserialize(std::vector<TYPE> &target) {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            target.resize(count / disp_unit);
            std::memcpy(target.data(), &strm[start], count);
            ++arg_No;
        }

        template<typename TYPE>
        void deserialize(std::set<TYPE> &target) {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int SIZE = count / disp_unit;

            if (!target.empty()) {
                target.clear();
            }

            for (int i = 0; i < SIZE; i++) {
                TYPE item;
                std::memcpy(&item, &strm[start], disp_unit);
                target.insert(item);

                start += disp_unit;
            }
            ++arg_No;
        }

        template<typename TYPE>
        void deserialize(std::list<TYPE> &target) {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int SIZE = count / disp_unit;

            if (!target.empty()) {
                target.clear();
            }

            for (int i = 0; i < SIZE; i++) {
                TYPE item;
                std::memcpy(&item, &strm[start], disp_unit);
                target.push_back(item);

                start += disp_unit;
            }
            ++arg_No;
        }

        template<typename TYPE>
        void deserialize(std::queue<TYPE> &target) {
            int disp_unit = sizeof(TYPE);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            int SIZE = count / disp_unit;

            if (!target.empty()) {
                std::queue<TYPE> empty;
                std::swap(target, empty);
            }

            for (int i = 0; i < SIZE; i++) {
                TYPE item;
                std::memcpy(&item, &strm[start], disp_unit);
                target.push(item);

                start += disp_unit;
            }
            ++arg_No;
        }

        template<typename _Ty1, typename _Ty2>
        void deserialize(std::map<_Ty1, _Ty2> &target) {
            int disp_unit = sizeof(int);
            int start = C[arg_No].second;
            int mapSize;

            std::memcpy(&mapSize, &strm[start], disp_unit);

            ++arg_No;
            if (!target.empty()) {
                target.clear();
            }

            for (int i = 0; i < mapSize; i++) {
                _Ty1 key;
                deserialize(key);
                _Ty2 val;
                deserialize(val);
                target.insert(std::make_pair(key, val));
            }
        }

        void deserialize(std::string &target) {
            int disp_unit = sizeof(char);
            int count = C[arg_No].first;
            int start = C[arg_No].second;
            target.resize(count / disp_unit);
            std::memcpy(target.data(), &strm[start], count);
            ++arg_No;
        }

        template<class TYPE,
                std::enable_if_t<!is_stl_container<TYPE>::value &&
                                 !std::is_fundamental<TYPE>::value &&
                                 !std::is_same<TYPE, std::string>::value,
                        bool> = true>
        void deserialize(TYPE &src) {
            auto &tmp = dynamic_cast<archive &>(*this);
            tmp.template pass(*this, src, 1);
        }
    };

    template<class TYPE>
    void archive::operator()(TYPE &&data) {
        if (this->cas == 0) {
            auto &tmp = static_cast<oarchive &>(*this); //down casting
            tmp.serialize(data);
        } else if (this->cas == 1) {
            auto &tmp = static_cast<iarchive &>(*this); //down casting
            tmp.deserialize(data);
        }

    }
}; // namespace serializer

#endif