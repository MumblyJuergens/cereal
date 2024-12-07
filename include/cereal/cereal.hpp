#pragma once

#include <algorithm>
#include <bit>
#include <istream>
#include <ostream>
#include <ranges>
#include <tuple>
#include <utility>
#include <gsl/narrow>

namespace cereal
{
    template <typename T>
    concept fundamental = std::is_arithmetic_v<T>;

    template <typename Type, template <typename...> typename Template>
    constexpr bool is_specialization_v = false; // true if and only if Type is a specialization of Template
    template <template <typename...> typename Template, typename... Types>
    constexpr bool is_specialization_v<Template<Types...>, Template> = true;

    template <typename Type, template <typename...> typename Template>
    struct is_specialization : std::bool_constant<is_specialization_v<Type, Template>>
    {
    };

    template <typename T>
    constexpr bool tuple_like_impl =
        is_specialization_v<T, std::tuple> || is_specialization_v<T, std::pair>;

    template <typename T>
    concept tuple_like = tuple_like_impl<std::remove_cvref_t<T>>;

    template <tuple_like T>
    struct remove_cv_tuple
    {
    };

    template <tuple_like T>
    struct remove_cv_tuple<T const> : public remove_cv_tuple<T>
    {
    };

    template <template <typename...> typename T, typename... A>
    struct remove_cv_tuple<T<A...>>
    {
        using type = T<std::remove_cv_t<A>...>;
    };

    template <typename T>
    concept container = requires(T t) {
        typename T::value_type;
        { t.end() };
    };

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

        template <std::ranges::input_range Range, class Proj = std::identity>
        void serialize(Range &&range, Proj proj = {})
        {
            serialize(gsl::narrow<std::ptrdiff_t>(std::distance(range.begin(), range.end())));
            std::ranges::for_each(range, [&](const auto &x)
                                  { serialize(x); }, proj);
        }

        template <tuple_like T, std::size_t I = 0>
        void serialize(const T &item)
        {
            if constexpr (I < std::tuple_size_v<T>)
            {
                serialize(std::get<I>(item));
                serialize<T, I + 1>(item);
            }
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
            std::remove_cv_t<T> temp{};
            is.read(reinterpret_cast<char *>(&temp), sizeof(T));
            return temp;
        }

        template <typename T, std::input_or_output_iterator O>
        void deserialize(O result)
        {
            const auto n = deserialize<std::ptrdiff_t>();
            std::ranges::generate_n(result, n, [this]()
                                    { return deserialize<T>(); });
        }

        template <container T>
        auto deserialize()
        {
            T t;
            deserialize<typename T::value_type>(std::inserter(t, t.end()));
            return t;
        }

        template <tuple_like T, std::size_t I = 0>
        void deserialize(T &t)
        {
            if constexpr (I < std::tuple_size_v<T>)
            {
                std::get<I>(t) = deserialize<std::tuple_element_t<I, T>>();
                deserialize<T, I + 1>(t);
            }
        }

        template <tuple_like T>
        auto deserialize()
        {
            typename remove_cv_tuple<T>::type t;
            deserialize(t);
            return t;
        }
    };

} // End namespace cereal.