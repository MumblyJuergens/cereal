#include <list>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cereal/cereal.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Integer (de)serialization", "[fundamental,integer]")
{
    std::uint8_t itemui8 = 200;
    std::uint16_t itemui16 = 40258;
    std::uint32_t itemui32 = 123456;
    std::uint64_t itemui64 = 12345645;
    std::int8_t itemi8 = 100;
    std::int16_t itemi16 = 20258;
    std::int32_t itemi32 = 123456;
    std::int64_t itemi64 = 12345645;
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(itemui8);
    out.serialize(itemui16);
    out.serialize(itemui32);
    out.serialize(itemui64);
    out.serialize(itemi8);
    out.serialize(itemi16);
    out.serialize(itemi32);
    out.serialize(itemi64);
    auto resultui8 = in.deserialize<std::uint8_t>();
    auto resultui16 = in.deserialize<std::uint16_t>();
    auto resultui32 = in.deserialize<std::uint32_t>();
    auto resultui64 = in.deserialize<std::uint64_t>();
    auto resulti8 = in.deserialize<std::int8_t>();
    auto resulti16 = in.deserialize<std::int16_t>();
    auto resulti32 = in.deserialize<std::int32_t>();
    auto resulti64 = in.deserialize<std::int64_t>();

    REQUIRE(stream.tellp() == 30);
    REQUIRE(resultui8 == itemui8);
    REQUIRE(resultui16 == itemui16);
    REQUIRE(resultui32 == itemui32);
    REQUIRE(resultui64 == itemui64);
    REQUIRE(resulti8 == itemi8);
    REQUIRE(resulti16 == itemi16);
    REQUIRE(resulti32 == itemi32);
    REQUIRE(resulti64 == itemi64);
}

TEST_CASE("Floating point (de)serialization", "[fundamental,real]")
{
    float itemf32 = 123.456f;
    double itemf64 = 456.789;
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(itemf32);
    out.serialize(itemf64);
    auto resultf32 = in.deserialize<float>();
    auto resultf64 = in.deserialize<double>();

    REQUIRE(stream.tellp() == 12);
    REQUIRE(resultf32 == itemf32);
    REQUIRE(resultf64 == itemf64);
}

struct tester_after
{
    int i{123};
    float f{456.789f};

    void serialize(cereal::Serializer &out)
    {
        out.serialize(i);
        out.serialize(f);
    }

    void deserialize(cereal::Deserializer &in)
    {
        i = in.deserialize<int>();
        f = in.deserialize<float>();
    }
};

struct tester_during
{
    int i;
    float f;

    void serialize(cereal::Serializer &out)
    {
        out.serialize(i);
        out.serialize(f);
    }

    tester_during(cereal::Deserializer &in)
    {
        i = in.deserialize<int>();
        f = in.deserialize<float>();
    }

    tester_during(int vi, float vf) : i{vi}, f{vf} {}
};

TEST_CASE("Deserializable after", "[struct,after]")
{
    tester_after item;
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    item.serialize(out);
    tester_after result;
    result.deserialize(in);

    REQUIRE(result.i == item.i);
    REQUIRE(result.f == item.f);
}

TEST_CASE("Deserializable during", "[struct,during]")
{
    tester_during item(123, 456.789f);
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    item.serialize(out);
    tester_during result{in};

    REQUIRE(result.i == item.i);
    REQUIRE(result.f == item.f);
}

TEST_CASE("Deserialize vector of int", "[container, vector, fundamental]")
{
    std::vector<int> item{234, 6575, 123, 6868, 1211};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::vector<int> result;
    in.deserialize<int>(std::back_inserter(result));

    REQUIRE(stream.tellp() == (5 * 4) + 8);
    REQUIRE(item == result);
}

TEST_CASE("Deserialize vector of int returned", "[container, vector, fundamental]")
{
    std::vector<int> item{234, 6575, 123, 6868, 1211};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::vector<int> result = in.deserialize<std::vector<int>>();

    REQUIRE(stream.tellp() == (5 * 4) + 8);
    REQUIRE(item == result);
}

TEST_CASE("Deserialize list of int", "[container, list, fundamental]")
{
    std::list<int> item{234, 6575, 123, 6868, 1211};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::list<int> result;
    in.deserialize<int>(std::back_inserter(result));

    REQUIRE(stream.tellp() == (5 * 4) + 8);
    REQUIRE(item == result);
}

TEST_CASE("Deserialize tuple of int", "[container, tuple, fundamental]")
{
    std::tuple<int, int> item{1, 234};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    auto result = in.deserialize<std::tuple<int, int>>();

    REQUIRE(stream.tellp() == 8);
    REQUIRE(std::get<0>(item) == std::get<0>(result));
    REQUIRE(std::get<1>(item) == std::get<1>(result));
}

TEST_CASE("Deserialize map of int", "[container, map, fundamental]")
{
    std::map<int, int> item{{1, 234}, {2, 6575}, {3, 123}, {4, 6868}, {5, 1211}};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::map<int, int> result;
    in.deserialize<std::pair<int, int>>(std::insert_iterator(result, result.end()));

    REQUIRE(stream.tellp() == (5 * 8) + 8);
    REQUIRE(item == result);
}

TEST_CASE("Deserialize map of int returned", "[container, map, fundamental]")
{
    std::map<int, int> item{{1, 234}, {2, 6575}, {3, 123}, {4, 6868}, {5, 1211}};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::map<int, int> result = in.deserialize<std::map<int, int>>();

    REQUIRE(stream.tellp() == (5 * 8) + 8);
    REQUIRE(item == result);
}

TEST_CASE("Deserialize unordered_map of int", "[container, unordered_map, fundamental]")
{
    std::unordered_map<int, int> item{{1, 234}, {2, 6575}, {3, 123}, {4, 6868}, {5, 1211}};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::unordered_map<int, int> result;
    in.deserialize<std::pair<int, int>>(std::insert_iterator(result, result.end()));

    REQUIRE(stream.tellp() == (5 * 8) + 8);
    REQUIRE(item == result);
}

TEST_CASE("Deserialize unordered_map of int returned", "[container, unordered_map, fundamental]")
{
    std::unordered_map<int, int> item{{1, 234}, {2, 6575}, {3, 123}, {4, 6868}, {5, 1211}};
    std::stringstream stream;
    cereal::Serializer out{stream};
    cereal::Deserializer in{stream};

    out.serialize(item);
    std::unordered_map<int, int> result = in.deserialize<std::unordered_map<int, int>>();

    REQUIRE(stream.tellp() == (5 * 8) + 8);
    REQUIRE(item == result);
}