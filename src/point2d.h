#include <boost/container_hash/hash.hpp>

#include <compare>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace Gfx_2d {

struct Direction
{
    int64_t dx = 0, dy = 0;

    template<std::integral T>
    constexpr Direction operator*(const T& num) const noexcept
    {
        Direction d {dx, dy};
        d *= num;
        return d;
    }

    template<std::integral T>
    constexpr Direction& operator*=(const T& num) noexcept
    {
        dx *= num;
        dy *= num;
        return *this;
    }

    constexpr auto operator<=>(Direction const&) const = default;

    [[nodiscard]] constexpr Direction back() const noexcept { return Direction {dx * -1, dy * -1}; }

    [[nodiscard]] constexpr Direction cw90() const noexcept { return Direction {-dy, dx}; }
    [[nodiscard]] constexpr Direction ccw90() const noexcept { return Direction {dy, -dx}; }

    friend size_t hash_value(Gfx_2d::Direction const& d) noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, d.dx);
        boost::hash_combine(seed, d.dy);
        return seed;
    }
};

constinit const Direction North{0, -1};
constinit const Direction South{0,  1};
constinit const Direction West{-1, 0};
constinit const Direction East{ 1, 0};

constinit const Direction NW{-1, -1};
constinit const Direction NE{ 1, -1};
constinit const Direction SW{-1,  1};
constinit const Direction SE{ 1,  1};

constinit const Direction Up{0, -1};
constinit const Direction Down{0,  1};
constinit const Direction Left{-1, 0};
constinit const Direction Right{ 1, 0};

constinit const Direction TopLeft{-1, -1};
constinit const Direction TopRight{ 1, -1};
constinit const Direction BottomLeft{-1,  1};
constinit const Direction BottomRight{ 1,  1};

template<std::integral Coord>
struct Point
{
    Coord x {}, y {};

    constexpr Point() noexcept = default;
    constexpr Point(Coord x_, Coord y_) noexcept
        : x {x_}
        , y {y_}
    { }

    constexpr Point(Point const&) noexcept = default;
    constexpr Point(Point&&) noexcept = default;

    constexpr Point& operator=(Point const&) noexcept = default;
    constexpr Point& operator=(Point&&) noexcept = default;

    constexpr auto operator<=>(Point const&) const = default;

    constexpr Point& operator+=(Direction const& o) noexcept
    {
        x += o.dx;
        y += o.dy;
        return *this;
    }

    constexpr friend Point operator+(Point lhs, Direction const& o) noexcept
    {
        lhs += o;
        return lhs;
    }

    constexpr Point& operator-=(Direction const& o) noexcept
    {
        x -= o.dx;
        y -= o.dy;
        return *this;
    }

    constexpr friend Point operator-(Point lhs, Direction const& o) noexcept
    {
        lhs -= o;
        return lhs;
    }

    constexpr Direction operator-(Point const& o) const noexcept { return {x - o.x, y - o.y}; }

    constexpr Coord manhattan_dist(Point const& o) const noexcept
    {
        return std::abs(std::make_signed_t<Coord>(x) - std::make_signed_t<Coord>(o.x))
            + std::abs(std::make_signed_t<Coord>(y) - std::make_signed_t<Coord>(o.y));
    }

    friend size_t hash_value(Gfx_2d::Point<Coord> const& p) noexcept {
        size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};


}  // namespace Gfx_2d

template<>
struct std::hash<Gfx_2d::Direction>
{
    size_t operator()(Gfx_2d::Direction const& d) const noexcept
    {
        return hash_value(d);
    }
};

template<typename Coord>
struct std::hash<Gfx_2d::Point<Coord>>
{
    size_t operator()(Gfx_2d::Point<Coord> const& p) const noexcept
    {
        return hash_value(p);
    }
};
