#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <cmath>

extern int tests_passed;
extern int tests_failed;

#define TEST_CASE(name, tags) void test_##name()
#define REQUIRE(expr) do { \
    if (!(expr)) { \
        std::cerr << "FAILED: " << #expr << " at line " << __LINE__ << std::endl; \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps) do { \
    if (std::abs((a) - (b)) > (eps)) { \
        std::cerr << "FAILED: " << #a << " == " << (a) << ", expected " << (b) << " at line " << __LINE__ << std::endl; \
        tests_failed++; \
        return; \
    } \
} while(0)

void run_test(const std::string& name, std::function<void()> test);

