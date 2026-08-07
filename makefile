# =====================================
# PXPT-Racer Makefile
# Cross Platform C Game
# =====================================


# Compiler
CC ?= gcc


# Project name
SERVER_BIN = pxpt-server
CLIENT_BIN = pxpt-client



# Directories

COMMON_DIR = common
CLIENT_DIR = client
SERVER_DIR = server


RAYLIB_DIR = raylib/src
RAYGUI_DIR = raygui-5.0/src



# Include paths

CFLAGS = \
-I$(COMMON_DIR) \
-I$(CLIENT_DIR) \
-I$(SERVER_DIR) \
-I$(RAYLIB_DIR) \
-I$(RAYGUI_DIR)



# Compiler flags

CFLAGS += -std=c99
CFLAGS += -Wall



# Libraries

MATH_LIB = -lm



# macOS / Linux pthread

THREAD_LIB = -pthread



# =====================================
# Source files
# =====================================


COMMON_SRC = \
$(COMMON_DIR)/socket.c



SERVER_SRC = \
$(SERVER_DIR)/main.c \
$(SERVER_DIR)/server.c \
$(COMMON_SRC)



CLIENT_SRC=\
client/main.c \
client/gui.c \
client/game.c \
client/network.c \
client/raygui_impl.c \
common/socket.c



# =====================================
# Default
# =====================================


.PHONY: all client server clean


all: client server



# =====================================
# Server
# =====================================


server:

	$(CC) \
	$(SERVER_SRC) \
	$(CFLAGS) \
	$(THREAD_LIB) \
	$(MATH_LIB) \
	-o $(SERVER_BIN)



# =====================================
# Client
# =====================================


client:

	$(CC) \
	$(CLIENT_SRC) \
	$(CFLAGS) \
	$(THREAD_LIB) \
	$(MATH_LIB) \
	raylib/src/libraylib.a \
	-framework OpenGL \
	-framework Cocoa \
	-framework IOKit \
	-framework CoreVideo \
	-o $(CLIENT_BIN)



# =====================================
# Clean
# =====================================


clean:

	rm -f $(SERVER_BIN)

	rm -f $(CLIENT_BIN)



# =====================================
# Debug build
# =====================================


debug:

	$(CC) \
	$(CLIENT_SRC) \
	$(CFLAGS) \
	-g \
	$(THREAD_LIB) \
	$(MATH_LIB) \
	-lraylib \
	-o $(CLIENT_BIN)



# =====================================
# Install
# =====================================


install:

	mkdir -p /usr/local/bin

	cp $(SERVER_BIN) /usr/local/bin/

	cp $(CLIENT_BIN) /usr/local/bin/