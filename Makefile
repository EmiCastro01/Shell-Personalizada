# Makefile principal

# Directorios de include
INCLUDE_DIRS = -Iinclude -Imonitor/include

# Archivos fuente
SRCS = src/main.c src/shell.c src/util.c

# Nombre del ejecutable
TARGET = main

# Comando de compilación
CC = gcc
CFLAGS = $(INCLUDE_DIRS)

all: $(TARGET) monitor_all

$(TARGET): $(SRCS)
		$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

monitor_all:
		$(MAKE) -C monitor all

clean:
		rm -f $(TARGET)
		$(MAKE) -C monitor clean