CC = arm-linux-gnueabihf-gcc

CFLAGS = -Wall -g -Iinc
LDFLAGS = -lgpiod

SRC = src/main.c src/gpio.c
OBJ = $(SRC:.c=.o)

TARGET = app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	@echo "Copying to BBB..."
	scp $(TARGET) debian@192.168.7.2:/home/debian/
	@echo "Running on BBB..."
	ssh debian@192.168.7.2 "chmod +x ~/app && ~/app"