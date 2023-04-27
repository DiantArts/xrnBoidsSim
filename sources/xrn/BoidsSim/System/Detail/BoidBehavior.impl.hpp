#pragma once

namespace xrn::bsim::system::detail {

///////////////////////////////////////////////////////////////////////////
/// Partition index type
///
///////////////////////////////////////////////////////////////////////////
struct PartitionIndex {
    constexpr PartitionIndex()
        : x{ 0 }
        , y{ 0 }
        , z{ 0 }
    {}
    constexpr PartitionIndex(int x, int y, int z)
        : x{ static_cast<::std::uint16_t>(x) }
        , y{ static_cast<::std::uint16_t>(y) }
        , z{ static_cast<::std::uint16_t>(z) }
    {}
    constexpr PartitionIndex(const ::glm::vec3& vec)
        : x{ static_cast<::std::uint16_t>(vec.x) }
        , y{ static_cast<::std::uint16_t>(vec.y) }
        , z{ static_cast<::std::uint16_t>(vec.z) }
    {}
    ::std::uint16_t x;
    ::std::uint16_t y;
    ::std::uint16_t z;
};

///////////////////////////////////////////////////////////////////////////
consteval auto getNumberOfPartition(
    ::glm::vec3 mapSize
    , double perceptionRadius
) -> ::xrn::bsim::system::detail::PartitionIndex
{
    // https://stackoverflow.com/a/66146159
    auto ceil{
        [](double f) -> int {
            const double i{ static_cast<int>(f) };
            return static_cast<int>(f > i ? i + 1 : i);
        }
    };

    // testing the lambda works as intended
    static_assert(ceil(0.0) == 0);
    static_assert(ceil(0.5) == 1);
    static_assert(ceil(0.999999) == 1);
    static_assert(ceil(1.0) == 1);
    static_assert(ceil(123.0) == 123);
    static_assert(ceil(123.4) == 124);
    static_assert(ceil(-0.5) == 0);
    static_assert(ceil(-0.999999) == 0);
    static_assert(ceil(-1.0) == -1);
    static_assert(ceil(-123.0) == -123);
    static_assert(ceil(-123.4) == -123);

    return ::xrn::bsim::system::detail::PartitionIndex{
        ceil(static_cast<double>(mapSize.x) / perceptionRadius) + 1
        , ceil(static_cast<double>(mapSize.y) / perceptionRadius) + 1
        , ceil(static_cast<double>(mapSize.z) / perceptionRadius) + 1
    };
}

} // namespace xrn::bsim::system::detail

template <> struct fmt::formatter<::xrn::bsim::system::detail::PartitionIndex> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()){ return ctx.begin(); }
    template <typename FormatContext> auto format(const ::xrn::bsim::system::detail::PartitionIndex& vec, FormatContext& ctx) -> decltype(ctx.out()){
        return format_to( ctx.out(), "PartitionIndex{}{}, {}, {}{}", '{', static_cast<int>(vec.x), static_cast<int>(vec.y), static_cast<int>(vec.z), '}');
    }
};
