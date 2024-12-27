# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Ilib

# Directories
SRCDIR = src
INCDIR = lib
BINDIR = bin
OBJDIR = obj

# Files
TARGET = $(BINDIR)/program
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Create binary
$(TARGET): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BINDIR) $(OBJDIR)

# Rebuild project
rebuild: clean all

.PHONY: all clean rebuild
