# Install
BIN = nuklear.so

# Flags
CFLAGS = -shared -fPIC -std=c99 -pedantic -O2

SRC = main.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lm -lGLU32 
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LIBS = -lSDL2 -framework OpenGL -lm
else
	LIBS = -lSDL2 -lGL -lm -lGLU -lpthread
	LIBS += -lsporth -lsoundpipe -lm -lpthread -ldl -ljack
endif
endif

$(BIN): main.c
	$(CC) $(SRC) $(CFLAGS) -o $(BIN) $(LIBS)

clean:
	rm -rf $(BIN)
