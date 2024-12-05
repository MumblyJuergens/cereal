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

    template <typename T, typename S>
    concept serializable = serializer<S> && requires(T t, S s) {
        { t.serialize(s) };
    };

    template <typename T, typename S>
    concept deserializable = deserializer<S> && requires(T t, S s) {
        { t.deserialize(s) };
    };

    template <typename Serializer>
    inline void serialize(Serializer &out, serializable<Serializer> auto &item)
    {
        item.serialize(out);
    }

    template <typename T, typename S>
    concept deserializable_during_construction = deserializer<S> && requires(T t, S s) {
        { T{s} };
    };

    template <typename T, typename Deserializer>
        requires deserializable<T, Deserializer>
    auto deserialize(Deserializer &in)
    {
        T item{};
        item.deserialize(in);
        return item;
    }

    template <typename T, typename Deserializer>
        requires deserializable_during_construction<T, Deserializer>
    auto deserialize(Deserializer &in)
    {
        return T{in};
    }

} // End namespace cereal.