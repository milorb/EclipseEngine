CXX	=	clang++
CXXFLAGS	= -Wall -std=c++17	\
-I./ \
-I./game_engine/third_party/	\
-I./game_engine/third_party/SDL2	\
-I./game_engine/third_party/SDL2_image	\
-I./game_engine/third_party/SDL2_mixer \
-I./game_engine/third_party/SDL2_ttf \
-I./game_engine/third_party/glm	\
-I./game_engine/third_party/rapidjson \
-I./game_engine/third_party/lua \
-I./game_engine/third_party/box2d \
-I./game_engine/third_party/LuaBridge

#	Libraries	to	link
LIBS	=	-lSDL2	-lSDL2_image -lSDL2main -lSDL2_mixer -lSDL2_ttf -llua5.4

#	Source	files
SOURCES	=	$(wildcard	game_engine/src/*.cpp)	$(wildcard	game_engine/third_party/*.cpp)  $(wildcard game_engine/third_party/box2d/*.cpp)
#	Header	files
HEADERS	=	$(wildcard	game_engine/src/*.h)	$(wildcard	game_engine/third_party/*.h)
#	Object	files
OBJECTS	=	$(SOURCES:.cpp=.o)
#	Output	executable
EXECUTABLE	=	game_engine_linux

#	Default	target
all:	$(EXECUTABLE)

#	Linking	the	executable	from	the	object	files
$(EXECUTABLE):	$(OBJECTS)
				$(CXX)	$(CXXFLAGS)	$(OBJECTS)	-o	$@	$(LIBS)

#	Compiling	source	files	into	object	files
%.o:	%.cpp	$(HEADERS)
				$(CXX)	$(CXXFLAGS)	-c	$<	-o	$@

#	Clean	target	for	removing	compiled	files
clean:
				rm	-f	game_engine/*.o	game_engine/path_to_new_directory/*.o	$(EXECUTABLE)

.PHONY:	all	clean
