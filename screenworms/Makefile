TARGET = screen-worms-server screen-worms-client
EXT_LIBS = -lz -pthread
CC = g++
CFLAGS = -Wall -Wextra -Wpedantic --std=c++14 -O3 -I.
LFLAGS = -Wall -Wextra $(EXT_LIBS)


HEADERS = \
        connection.h \
        types.h \
        utils.h \
	server/server.h \
	server/networking.h \
	client/client.h \
	client/gui.h \
	network/conversation.h \
	network/poll.h \
	network/socket.h \
	game/board.h \
	game/message.h \
	game/random_generator.h \
	game/stats.h \
	game/worm.h \
	game/events/create.h \
	game/events/event.h \
	game/events/game_over.h \
	game/events/new_game.h \
	game/events/pixel.h \
	game/events/player_eliminated.h \

COMMON_OBJS = server/networking.o client/gui.o network/conversation.o network/socket.o game/board.o game/message.o game/random_generator.o game/stats.o game/worm.o game/events/create.o game/events/event.o game/events/game_over.o game/events/new_game.o game/events/pixel.o game/events/player_eliminated.o client/client.o server/server.o
	
SERVER_OBJS = screen-worms-server.o $(COMMON_OBJS)
	
CLIENT_OBJS = screen-worms-client.o $(COMMON_OBJS)


all: screen-worms
screen-worms: $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

screen-worms-server: $(SERVER_OBJS)
	$(CC) $(LFLAGS) $^ -o $@

screen-worms-client: $(CLIENT_OBJS)
	$(CC) $(LFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(CLIENT_OBJS) $(SERVER_OBJS)
