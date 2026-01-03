#!/bin/bash
# Comprehensive test suite for Displexity compiler

set -e

COMPILER="./build/disp"
CC="gcc"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

test_count=0
pass_count=0
fail_count=0

run_test() {
    local test_name=$1
    local dis_file=$2
    local expected_output=$3
    
    test_count=$((test_count + 1))
    echo -n "Test $test_count: $test_name... "
    
    if [ ! -f "$dis_file" ]; then
        echo -e "${RED}SKIP${NC} (file not found)"
        return
    fi
    
    # Compile
    output_c="build/${test_name}.c"
    output_exe="build/${test_name}.exe"
    
    if ! $COMPILER "$dis_file" -o "$output_c" > /dev/null 2>&1; then
        echo -e "${RED}FAIL${NC} (compilation error)"
        fail_count=$((fail_count + 1))
        return
    fi
    
    # Link
    if ! $CC "$output_c" -o "$output_exe" > /dev/null 2>&1; then
        echo -e "${RED}FAIL${NC} (linking error)"
        fail_count=$((fail_count + 1))
        return
    fi
    
    # Run and check output
    actual_output=$("$output_exe" 2>&1 || true)
    
    if echo "$actual_output" | grep -q "$expected_output"; then
        echo -e "${GREEN}PASS${NC}"
        pass_count=$((pass_count + 1))
    else
        echo -e "${RED}FAIL${NC}"
        echo "  Expected: $expected_output"
        echo "  Got: $actual_output"
        fail_count=$((fail_count + 1))
    fi
}

echo "=== Displexity Test Suite ==="
echo ""

# Basic tests
run_test "hello" "examples/hello.dis" "Hello"
run_test "conditional" "examples/conditional.dis" "Large"

echo ""
echo "Summary:"
echo "  Total: $test_count"
echo -e "  ${GREEN}Passed: $pass_count${NC}"
echo -e "  ${RED}Failed: $fail_count${NC}"
echo ""

if [ $fail_count -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
