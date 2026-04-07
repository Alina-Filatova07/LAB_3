CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -g

# Файлы для основной программы
MAIN_SRCS = main.cpp lexer.cpp ast.cpp parser.cpp utils.cpp
TARGET = lab

# Файлы для тестов
TEST_SRCS = test_main.cpp test_helpers.cpp lexer.cpp ast.cpp parser.cpp
TEST_TARGET = test_runner

# По умолчанию собираем всё
all: $(TARGET) $(TEST_TARGET)

# Сборка основной программы
$(TARGET): $(MAIN_SRCS)
	$(CXX) $(CXXFLAGS) $(MAIN_SRCS) -o $(TARGET)

# Сборка тестов
$(TEST_TARGET): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) -o $(TEST_TARGET)

# Специальная команда для запуска тестов
test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

.PHONY: all clean test