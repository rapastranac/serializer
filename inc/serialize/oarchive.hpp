#ifndef OARCHIVE_HPP
#define OARCHIVE_HPP

#include "archive.hpp"

namespace serializer {
    class oarchive : public archive {

    public:
        oarchive(serializer::stream &stream) : archive(stream) {
        }

        ~oarchive() {
            for (int i = 0; i < C.size(); i++) {
                delete[] C[i].second;
            }
        }

        template<typename TYPE>
        oarchive &operator<<(TYPE &src) {
            serialize(src);
            fillBuffer();
            return *this;
        }

    private:
        void fillBuffer() {
            int counter = 0;
            int count;

            int firstBytes = (NUM_ARGS + 1) * sizeof(int);
            strm.allocate(Bytes + firstBytes);

            std::memcpy(&strm[0], &NUM_ARGS, sizeof(int)); //NUM_ARGS copied into stream
            counter += sizeof(int);

            for (int i = 0; i < NUM_ARGS; i++) {
                count = sizeof(int);
                std::memcpy(&strm[counter], &C[i].first, count); //Bytes count copied
                counter += sizeof(int);
            }

            for (size_t i = 0; i < NUM_ARGS; i++) {
                count = C[i].first;
                std::memcpy(&strm[counter], C[i].second, count); //actual Bytes copied
                counter += C[i].first;
            }
        }

        /*
         * fundamental types
         * */
        template<typename _T,
                std::enable_if_t<std::is_fundamental<_T>::value, bool> = true>
        void serialize(const _T &src) {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(_T);
            int count = disp_unit * 1;
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(C.back().second, &src, count);
        }

        template<typename TYPE>
        void serialize(const std::vector<TYPE> &src) {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(C.back().second, src.data(), count);
        }

        template<typename TYPE>
        void serialize(const std::set<TYPE> &src) {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));

            auto it = src.begin();
            int idx = 0;
            while (it != src.end()) {
                std::memcpy(&C.back().second[idx], &*it, disp_unit);
                ++it;
                idx += disp_unit;
            }
        }

        template<typename TYPE>
        void serialize(const std::list<TYPE> &src) {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));

            auto it = src.begin();
            int idx = 0;
            while (it != src.end()) {
                std::memcpy(&C.back().second[idx], &*it, disp_unit);
                ++it;
                idx += disp_unit;
            }
        }

        template<typename TYPE>
        void serialize(const std::queue<TYPE> &src) {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(TYPE);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));

            auto srcCpy = src;
            int idx = 0;
            while (!srcCpy.empty()) {
                std::memcpy(&C.back().second[idx], &srcCpy.front(), disp_unit);
                srcCpy.pop();
                idx += disp_unit;
            }
        }

        template<typename _Ty1, typename _Ty2>
        void serialize(const std::map<_Ty1, _Ty2> &src) {
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

            for (auto const &[key, val] : src) {
                serialize(key);
                serialize(val);
            }
        }

        void serialize(std::string &src) {
            ++this->NUM_ARGS;

            int disp_unit = sizeof(char);
            int count = disp_unit * src.size();
            this->Bytes += count;

            C.emplace_back(std::make_pair(count, new char[count]));
            std::memcpy(C.back().second, src.c_str(), count);
        }

        template<class TYPE,
                std::enable_if_t<!is_stl_container<TYPE>::value &&
                                 !std::is_fundamental<TYPE>::value &&
                                 !std::is_same<TYPE, std::string>::value,
                        bool> = true>
        void serialize(TYPE &src) {
            src.serialize(*this);
        }
    };

}; // namespace serializer

#endif