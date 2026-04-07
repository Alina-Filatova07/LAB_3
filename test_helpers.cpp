#include "test_helpers.h"

int tests_passed = 0;
int tests_failed = 0;

void run_test(const std::string& name, std::function<void()> test) {
    std::cout << "  Testing: " << name << "... ";
    try {
        test();
        std::cout << "PASSED" << std::endl;
        tests_passed++;
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
        tests_failed++;
    }
}
