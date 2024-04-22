#ifndef Event_hpp
#define Event_hpp

#include <string>
#include <queue>
#include <unordered_map>
#include "lua.hpp"
#include "LuaBridge.h"

#include "ComponentDB.hpp"

struct Publication {
	Publication(std::string e, luabridge::LuaRef c, luabridge::LuaRef f) : 
		event_type(e), 
		component(c), 
		function(f) {}

	std::string event_type;
	luabridge::LuaRef component;
	luabridge::LuaRef function;
};

class Event
{
public:
	static void init();

	static void Pub(std::string event_type, luabridge::LuaRef event_obj);

	static void Sub(std::string event_type, luabridge::LuaRef component, 
		luabridge::LuaRef function);

	static void Unsub(std::string event_type, luabridge::LuaRef component, 
		luabridge::LuaRef function);

	static void update_subscriptions();

	static inline std::queue<Publication> to_sub;
	static inline std::queue<Publication> to_unsub;

	static inline std::unordered_map<std::string, 
		std::vector< 
			std::pair< luabridge::LuaRef, luabridge::LuaRef>
		>> publications;
};


#endif
