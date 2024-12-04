#pragma once

#include <bit>
#include <concepts>
#include <istream>
#include <ostream>

namespace cereal
{
    template <typename T>
    concept fundamental = std::is_arithmetic_v<T>;

    void serialize(std::ostream &os, fundamental auto item)
    {
        if constexpr (std::endian::native != std::endian::little)
            item = std::byteswap(item);
        os.write(reinterpret_cast<const char *>(&item), sizeof(item));
    }

    template <fundamental T>
    auto deserialize(std::istream &is)
    {
        T temp{};
        is.read(reinterpret_cast<char *>(&temp), sizeof(T));
        return temp;
    }

    struct serializable
    {
        virtual ~serializable() = default;
        virtual void serialize(std::ostream &out) = 0;
    };

    struct deserializable
    {
        virtual ~deserializable() = default;
        virtual void deserialize(std::istream &in) = 0;
    };

    inline void serialize(std::ostream &out, serializable &item)
    {
        item.serialize(out);
    }

    template <typename T>
    concept deserializable_after_construction = std::is_base_of_v<deserializable, T> && std::is_default_constructible_v<T>;

    template <typename T>
    concept deserializable_during_construction = std::is_base_of_v<deserializable, T> && std::is_constructible_v<T, std::istream &>;

    template <deserializable_after_construction T>
    auto deserialize(std::istream &in)
    {
        T item{};
        item.deserialize(in);
        return item;
    }

    template <deserializable_during_construction T>
    auto deserialize(std::istream &in)
    {
        return T{in};
    }

} // End namespace cereal.