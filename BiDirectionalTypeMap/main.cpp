#include <iostream>
#include <vector>
#include <utility>//std::integer_sequence, std::make_index_sequence
#include <type_traits>//std::integral_constant

#include <thread>

//Below is a bi-directional compile time type to integer map.

//Use the map like this to automatically combine types with 
//indicies / integer sequence 0, 1, 2, 4 etc:
/*

//These types will be automatically paired up like this:
//int will be paired with 0, std::vector<int> will be paired with 1,
//bool will be paired with 2, and unsigned int will be paired with 3.
#define TYPE_REGISTRY TypeRegistry<int, std::vector<int>, bool, unsigned>

//Get the type mapped to the given index.
template <size_t idx>
using GetTypeFromIndex = TYPE_REGISTRY::IndexedMap::type<idx>;

//Get the index mapped to the given type.
template <typename T>
static auto GetIndexFromType = TYPE_REGISTRY::IndexedMap::index<T>;

*/

//Or use the map like this to combine types to specific integer IDs:
/*

#define TYPE_REGISTRY_COMBINE \
        TypeRegistry <int, char, bool, float > :: \    <- the types in the map
        CombineWith  <4,   12,   4,    13    >         <- the integers IDs they are paired with

//Get the type mapped to ID.
template <size_t ID> 
using GetType = TYPE_REGISTRY_COMBINE::Map::type<ID>

//Get the ID mapped to T.
template <typename T>
static auto getID = TYPE_REGISTRY_COMBINE::Map::index<T>

*/

template <size_t index, typename T> 
struct IntegerTypePair
{ 
    using type = T;
    static constexpr size_t integer = index;

    static IntegerTypePair getPairFromIndex(
        std::integral_constant<size_t, integer>) {return {};}

    static IntegerTypePair getPairFromType(T) {return {};}
};

template <typename ...IDTypePairs> 
struct BiDirectionalMap : IDTypePairs...
{
    using IDTypePairs::getPairFromIndex...;
    using IDTypePairs::getPairFromType...;

    template <size_t ID>
    using type = typename decltype(
        getPairFromIndex(std::integral_constant<size_t, ID>{}))::type;

    template <typename T>
    static constexpr size_t index = decltype(
        getPairFromType(T{}))::integer;
};

template <typename ...Ts>
struct TypeRegistry
{
    template <size_t ...IDs>
    struct CombineWith 
    { 
        static constexpr size_t maxNumTypes{64};
        static constexpr size_t numTypes{sizeof...(Ts)};

        static_assert(sizeof...(Ts) < maxNumTypes,
            "Exceeded maximum number of types");

        static_assert(sizeof...(Ts) == sizeof...(IDs), 
            "There are mismatching Types to IDs in the TypeRegistry");

        using Map = BiDirectionalMap<IntegerTypePair<IDs, Ts>...>;
    };

    template <typename T, T... indecies>
    static auto createIndexedMap(std::integer_sequence<T, indecies...>) 
        -> BiDirectionalMap<IntegerTypePair<indecies, Ts>...> {return {};}

    using IndexedMap = decltype(createIndexedMap(
        std::make_index_sequence<sizeof...(Ts)>{}));
};

//Below are the meta-functions that are described in the comment at the top of the file.

//The IndexedMap in the type registry will automatically pair 0 with int,  
//1 with std::vector<int>, 2 with bool, 3 with unsigned int, and 4 with std::thread.
#define TYPE_REGISTRY TypeRegistry<int, std::vector<int>, bool, unsigned, std::thread>

//Get the type mapped to the given index.
template <size_t idx>
using GetTypeFromIndex = TYPE_REGISTRY::IndexedMap::type<idx>;

//Get the index mapped to the given type.
template <typename T>
static auto GetIndexFromType = TYPE_REGISTRY::IndexedMap::index<T>;

int main()
{
    //The integer 2 will be mapped to the type bool at compile time.
    GetTypeFromIndex<2> a;
    std::cout << typeid(a).name();

    std::cout << " ";

    //The type std::vector<int> will be mapped to the integer 1 at compile time.
    std::cout << GetIndexFromType< std::vector<int> >;
}