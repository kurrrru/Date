NAME = Date.out
NAME_TEST = Date_test.out

SRCS_DATE = \
	src/calendar_system/EthiopianCalendar.cpp \
	src/calendar_system/FrenchRepublicanCalendar.cpp \
	src/calendar_system/GregorianCalendar.cpp \
	src/calendar_system/JapaneseEra.cpp \
	src/calendar_system/JapaneseWarekiCalendar.cpp \
	src/calendar_system/JulianCalendar.cpp \
	src/calendar_system/NonProlepticGregorianCalendar.cpp \
	src/Date.cpp \
	src/string.cpp \

SRCS = ${SRCS_DATE} \
	src/main.cpp
SRCS_TEST = ${SRCS_DATE} \
	src/test.cpp

OBJS = $(SRCS:.cpp=.o)
OBJS_TEST = $(SRCS_TEST:.cpp=.o)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -I./src -pedantic

.PHONY: all test clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(NAME_TEST)

$(NAME_TEST): $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS) $(OBJS_TEST)

fclean: clean
	$(RM) $(NAME) $(NAME_TEST)

re: fclean all
