#include "customer_slip.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("calculateTotalPrice applies discount", "[core]") {
    REQUIRE(calculateTotalPrice(100.0, 0.0) == Catch::Approx(100.0));
    REQUIRE(calculateTotalPrice(100.0, 10.0) == Catch::Approx(90.0));
    REQUIRE(calculateTotalPrice(250.0, 20.0) == Catch::Approx(200.0));
}

TEST_CASE("parseDouble validates numeric input", "[core]") {
    double value = 0.0;
    REQUIRE(parseDouble("123", value));
    REQUIRE(value == Catch::Approx(123.0));

    REQUIRE(parseDouble("  45.67 ", value));
    REQUIRE(value == Catch::Approx(45.67));

    REQUIRE_FALSE(parseDouble("", value));
    REQUIRE_FALSE(parseDouble("abc", value));
    REQUIRE_FALSE(parseDouble("12abc", value));
}

TEST_CASE("formatCurrency uses fixed precision", "[core]") {
    REQUIRE(formatCurrency(0.0) == "$0.00");
    REQUIRE(formatCurrency(12.5) == "$12.50");
    REQUIRE(formatCurrency(999.999) == "$1000.00");
}
