# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

# Enable debug mode if DEBUG=1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG_PRINT -g
endif

# Source files
SOURCES = src/main.cpp \
		  src/common/args.cpp \
		  src/common/message_validator.cpp \
		  src/messages/tcp_message.cpp \
		  src/messages/udp_message.cpp \
		  src/network/tcp_client.cpp \
		  src/network/udp_client.cpp \
		  src/state/state_machine.cpp \
		  src/io/input_handler.cpp \
		  src/io/output_handler.cpp \
		  src/client_application.cpp \
		  src/signal_handler.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# Targets
all: ipk25chat-client

ipk25chat-client: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	# Remove all .o files and the binary
	find src -type f -name '*.o' -delete
	rm -f ipk25chat-client

# Phony targets
.PHONY: all clean