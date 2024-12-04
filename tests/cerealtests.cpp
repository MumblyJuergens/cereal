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

    cereal::serialize(stream, itemui8);
    cereal::serialize(stream, itemui16);
    cereal::serialize(stream, itemui32);
    cereal::serialize(stream, itemui64);
    cereal::serialize(stream, itemi8);
    cereal::serialize(stream, itemi16);
    cereal::serialize(stream, itemi32);
    cereal::serialize(stream, itemi64);
    auto resultui8 = cereal::deserialize<std::uint8_t>(stream);
    auto resultui16 = cereal::deserialize<std::uint16_t>(stream);
    auto resultui32 = cereal::deserialize<std::uint32_t>(stream);
    auto resultui64 = cereal::deserialize<std::uint64_t>(stream);
    auto resulti8 = cereal::deserialize<std::int8_t>(stream);
    auto resulti16 = cereal::deserialize<std::int16_t>(stream);
    auto resulti32 = cereal::deserialize<std::int32_t>(stream);
    auto resulti64 = cereal::deserialize<std::int64_t>(stream);

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

    cereal::serialize(stream, itemf32);
    cereal::serialize(stream, itemf64);
    auto resultf32 = cereal::deserialize<float>(stream);
    auto resultf64 = cereal::deserialize<double>(stream);

    REQUIRE(stream.tellp() == 12);
    REQUIRE(resultf32 == itemf32);
    REQUIRE(resultf64 == itemf64);
}

struct tester_after : public cereal::serializable, public cereal::deserializable
{
    int i{123};
    float f{456.789f};

    void serialize(std::ostream &out) override
    {
        cereal::serialize(out, i);
        cereal::serialize(out, f);
    }

    void deserialize(std::istream &in) override
    {
        i = cereal::deserialize<int>(in);
        f = cereal::deserialize<float>(in);
    }
};

struct tester_during : public cereal::serializable, public cereal::deserializable
{
    int i;
    float f;

    void serialize(std::ostream &out) override
    {
        cereal::serialize(out, i);
        cereal::serialize(out, f);
    }

    void deserialize(std::istream &in) override
    {
        i = cereal::deserialize<int>(in);
        f = cereal::deserialize<float>(in);
    }

    tester_during(std::istream &in)
    {
        deserialize(in);
    }

    tester_during(int vi, float vf) : i{vi}, f{vf} {}
};

TEST_CASE("Deserializable after", "[struct,after]")
{
    tester_after item;
    std::stringstream stream;

    cereal::serialize(stream, item);
    tester_after result = cereal::deserialize<tester_after>(stream);

    REQUIRE(result.i == item.i);
    REQUIRE(result.f == item.f);
}

TEST_CASE("Deserializable during", "[struct,during]")
{
    tester_during item(123, 456.789f);
    std::stringstream stream;

    cereal::serialize(stream, item);
    tester_during result = cereal::deserialize<tester_during>(stream);

    REQUIRE(result.i == item.i);
    REQUIRE(result.f == item.f);
}