#pragma once

#include <bit>
#include <istream>
#include <ostream>

namespace cereal
{
    template <typename T>
    concept fundamental = std::is_arithmetic_v<T>;

    class Serializer
    {
        std::ostream &os;

    public:
        Serializer(std::ostream &os) : os{os} {}

        void serialize(fundamental auto item)
        {
            if constexpr (std::endian::native != std::endian::little)
                item = std::byteswap(item);
            os.write(reinterpret_cast<const char *>(&item), sizeof(item));
        }
    };

    class Deserializer
    {
        std::istream &is;

    public:
        Deserializer(std::istream &is) : is{is} {}

        template <fundamental T>
        auto deserialize()
        {
            T temp{};
            is.read(reinterpret_cast<char *>(&temp), sizeof(T));
            return temp;
        }
    };

} // End namespace cereal.