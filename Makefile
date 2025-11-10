NAME = Date_test.out

SRCS = \
	src/calendar_system/EthiopianCalendar.cpp \
	src/calendar_system/FrenchRepublicanCalendar.cpp \
	src/calendar_system/GregorianCalendar.cpp \
	src/calendar_system/JapaneseEra.cpp \
	src/calendar_system/JapaneseWarekiCalendar.cpp \
	src/calendar_system/JulianCalendar.cpp \
	src/calendar_system/NonProlepticGregorianCalendar.cpp \
	src/Date.cpp \
	src/string.cpp \
	src/test.cpp
OBJS = $(SRCS:.cpp=.o)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -I./src -pedantic

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all
