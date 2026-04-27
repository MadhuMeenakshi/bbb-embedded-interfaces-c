CC = gcc

# ========================
# BASE FLAGS
# ========================
BASE_FLAGS = -std=c11 -Wall -Wextra -Werror -D_POSIX_C_SOURCE=200809L -Isrc

# Normal build
CFLAGS = $(BASE_FLAGS) -O2
TEST_CFLAGS = $(CFLAGS) -Itests/unit/mocks

# Coverage build
COV_FLAGS = $(BASE_FLAGS) -O0 --coverage
COV_TEST_FLAGS = $(COV_FLAGS) -Itests/unit/mocks

# Sanitizer build
SAN_FLAGS = $(BASE_FLAGS) -O1 -fsanitize=address,undefined -fno-omit-frame-pointer
SAN_TEST_FLAGS = $(SAN_FLAGS) -Itests/unit/mocks

SRC_DIR = src
TEST_DIR = tests/unit

TARGET = pmsh

SRCS = $(SRC_DIR)/pmsh_main.c \
       $(SRC_DIR)/pmsh_ring.c \
       $(SRC_DIR)/pmsh_sensor.c \
       $(SRC_DIR)/pmsh_cmd.c \
       $(SRC_DIR)/pmsh_power.c \
       $(SRC_DIR)/pmsh_platform.c

OBJS = $(SRCS:.c=.o)

TEST_SRCS = $(filter-out $(TEST_DIR)/test_runner.c, $(wildcard $(TEST_DIR)/*.c))
TEST_BINS = $(patsubst %.c, %, $(TEST_SRCS))

MOCK_SRCS = tests/unit/mocks/mock_platform.c

# ========================
# DEFAULT BUILD
# ========================
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ========================
# UNIT TEST BUILD
# ========================
#$(TEST_DIR)/%: $(TEST_DIR)/%.c $(SRCS) $(MOCK_SRCS)
#	$(CC) $(TEST_CFLAGS) $< \
#	$(filter-out $(SRC_DIR)/pmsh_main.c $(SRC_DIR)/pmsh_platform.c,$(SRCS)) \
#	$(MOCK_SRCS) \
#	-o $@

TEST_OBJS = $(filter-out $(SRC_DIR)/pmsh_main.o $(SRC_DIR)/pmsh_platform.o,$(OBJS))

$(TEST_DIR)/%: $(TEST_DIR)/%.c $(TEST_OBJS) $(MOCK_SRCS)
	$(CC) $(TEST_CFLAGS) $< \
	$(TEST_OBJS) \
	$(MOCK_SRCS) \
	-o $@

# ========================
# TEST RUNNER
# ========================
test: $(TEST_BINS)
	@echo "=============================="
	@echo "Running Unit Tests"
	@echo "=============================="
	@total=0; pass=0; fail=0; \
	for test in $(TEST_BINS); do \
		echo "Running $$test"; \
		if ./$$test; then \
			echo "[PASS] $$test"; pass=$$((pass+1)); \
		else \
			echo "[FAIL] $$test"; fail=$$((fail+1)); \
		fi; \
		total=$$((total+1)); \
	done; \
	echo "=============================="; \
	echo "Total: $$total  Passed: $$pass  Failed: $$fail"; \
	if [ $$fail -ne 0 ]; then exit 1; fi

# ========================
# COVERAGE
# ========================
cov: clean
	$(MAKE) CFLAGS="$(COV_FLAGS)" TEST_CFLAGS="$(COV_TEST_FLAGS)" all
	$(MAKE) CFLAGS="$(COV_FLAGS)" TEST_CFLAGS="$(COV_TEST_FLAGS)" test
	@echo "Generating gcov report..."
	@gcov -o src src/*.c > coverage.txt
	@grep "Lines executed" coverage.txt

# HTML coverage (optional)
cov-html: clean
	$(MAKE) CFLAGS="$(COV_FLAGS)" TEST_CFLAGS="$(COV_TEST_FLAGS)" all
	$(MAKE) CFLAGS="$(COV_FLAGS)" TEST_CFLAGS="$(COV_TEST_FLAGS)" test
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report
	@echo "Open coverage_report/index.html"

# ========================
# SANITIZER BUILD
# ========================
san: clean
	$(MAKE) CFLAGS="$(SAN_FLAGS)" TEST_CFLAGS="$(SAN_TEST_FLAGS)" all
	$(MAKE) CFLAGS="$(SAN_FLAGS)" TEST_CFLAGS="$(SAN_TEST_FLAGS)" test

# ========================
# CLEAN
# ========================
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)
	rm -f $(TEST_BINS)

	# Remove coverage files everywhere
	find . -name "*.gcda" -delete
	find . -name "*.gcno" -delete
	find . -name "*.gcov" -delete

	# Remove temp files
	rm -f coverage.txt coverage.info
	rm -rf coverage_report

	# Remove temp IO files from tests
	rm -f *.txt out.txt input.txt

# ========================
# RUN
# ========================
run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run cov cov-html san
