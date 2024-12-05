#pragma once

#include <bit>
#include <concepts>
#include <istream>
#include <ostream>

namespace cereal
{
    template <typename T>
    concept serializer = requires(T t, int i) {
        { t.serialize(i) };
    };

    template <typename T>
    concept deserializer = requires(T t) {
        { t.template deserialize<int>() } -> std::convertible_to<int>;
        { t.template deserialize<float>() } -> std::convertible_to<float>;
    };

    template <typename T>
    concept fundamental = std::is_arithmetic_v<T>;

    class stream_serializer
    {
        std::ostream &os;

    public:
        stream_serializer(std::ostream &os) : os{os} {}

        void serialize(fundamental auto item)
        {
            if constexpr (std::endian::native != std::endian::little)
                item = std::byteswap(item);
            os.write(reinterpret_cast<const char *>(&item), sizeof(item));
        }
    };

    static_assert(serializer<stream_serializer>);

    class stream_deserializer
    {
        std::istream &is;

    public:
        stream_deserializer(std::istream &is) : is{is} {}

        template <fundamental T>
        auto deserialize()
        {
            T temp{};
            is.read(reinterpret_cast<char *>(&temp), sizeof(T));
            return temp;
        }
    };

    static_assert(deserializer<stream_deserializer>);

} // End namespace cereal.