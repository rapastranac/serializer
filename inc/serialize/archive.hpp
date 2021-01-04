#ifndef ARCHIVE_PARENT_HPP
#define ARCHIVE_PARENT_HPP

#include "stream.hpp"

#include <charconv>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

//STL containers
#include <array>
#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <vector>

/*
* Created by Andres Pastrana on 2020
* andres.pastrana@usherbrooke.ca
* rapastranac@gmail.com
*/

/*
* it contains serialized data in bytes
|NUM_ARGS  |
|  ARG1    |
|  ARG2    |
|   ...    |
|  ARGN,   |
|ARG1_BYTES|
|ARG1_BYTES|
|   ...    |
|ARGN_BYTES|

*/

//specialize a type for all of the STL containers.
//https://stackoverflow.com/a/31105859/5248548
namespace is_stl_container_impl
{
    template <typename T>
    struct is_stl_container : std::false_type
    {
    };
    template <typename T, std::size_t N>
    struct is_stl_container<std::array<T, N>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::vector<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::deque<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::list<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::set<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::unordered_set<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::multiset<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::map<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::multimap<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::unordered_map<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::unordered_multimap<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::queue<Args...>> : std::true_type
    {
    };
    template <typename... Args>
    struct is_stl_container<std::priority_queue<Args...>> : std::true_type
    {
    };
} // namespace is_stl_container_impl

//type trait to utilize the implementation type traits as well as decay the type
template <typename T>
struct is_stl_container
{
    static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

namespace archive
{
    class archive_parent
    {
    private:
        archive::stream *stream = nullptr;     // pointer to stream
        int NUM_ARGS;                          // Number of arguments attached to stream
        std::vector<std::pair<int, char *>> C; // temporary container to store buffer section while building stream
        int Bytes;                             // number of bytes to be contained in stream

    public:
        archive_parent() {}
        archive_parent(archive::stream &stream)
        {
            this->NUM_ARGS = 0;
            this->Bytes = 0;
            this->stream = &stream;
        }

        virtual ~archive_parent()
        {
        }
    };
}; // namespace archive

#endif