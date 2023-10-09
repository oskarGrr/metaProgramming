#include <iostream>
#include <cstdint>

typedef uint64_t index_t;

template <index_t index, typename T> struct IntegerTypePair
{ 
    using type = T;
    static constexpr index_t integer = index;
    static IntegerTypePair getPairFromIndex(std::integral_constant<index_t, integer>) {return {};}
    static IntegerTypePair getPairFromType(T) {return {};}
};

template <typename ...IDTypePairs> struct BiDirectionalIndexTypeMap : IDTypePairs...
{
    using IDTypePairs::getPairFromIndex...;
    using IDTypePairs::getPairFromType...;

    template <index_t ID>
    using type = typename decltype(
        getPairFromIndex(std::integral_constant<index_t, ID>{}))::type;

    template <typename T>
    static constexpr index_t index = decltype(
        getPairFromType(T{}))::integer;
};

template <typename ...Ts> struct TypeRegistry
{
    static constexpr size_t maxNumTypes{64};
    static constexpr size_t numTypes{sizeof...(Ts)};
    static_assert(sizeof...(Ts) < maxNumTypes, "Exceeded maximum number of types");

    template <index_t ...IDs> struct CombineWith 
    { 
        static_assert(sizeof...(Ts) == sizeof...(IDs), 
            "There are mismatching Types to IDs in the TypeRegistry");

        using Map = BiDirectionalIndexTypeMap<IntegerTypePair<IDs, Ts>...>;
    };
};

template <size_t requestedID> 
using GetType = 
TypeRegistry <int, char, bool, float> ::
CombineWith  <0,   1,    2,    3    > ::Map::type<requestedID>;

template <typename T>
static auto index = 
TypeRegistry <int, char, bool, float> ::
CombineWith  <0,   1,    2,    3    > ::Map::index<T>;

int main()
{
    std::cout << "1 is mapped to: " << typeid(GetType<1>).name() << '\n';
    std::cout << "char is mapped to: " << index<char> << "\n\n";

    std::cout << "3 is mapped to: " << typeid(GetType<3>).name() << '\n';
    std::cout << "bool is mapped to: " << index<bool> << '\n';

    return 0;
}