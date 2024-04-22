#include "Event.hpp"

void Event::init() {
	luabridge::getGlobalNamespace(ComponentDB::__State)
		.beginNamespace("Event")
		.addFunction("Publish", Event::Pub)
		.addFunction("Subscribe", Event::Sub)
		.addFunction("Unsubscribe", Event::Unsub)
		.endNamespace();
}

void Event::Pub(std::string event_type, luabridge::LuaRef event_obj)
{
	for (auto& [comp, func] : publications[event_type]) {
		func(comp, event_obj);
	}
}

void Event::Sub(std::string event_type, luabridge::LuaRef component,
	luabridge::LuaRef function)
{
	to_sub.push(Publication(event_type, component, function));
}

void Event::Unsub(std::string event_type, luabridge::LuaRef component,
	luabridge::LuaRef function)
{
	to_unsub.push(Publication(event_type, component, function));
}

void Event::update_subscriptions() {

	while (!to_sub.empty()) {
		std::string event_type = to_sub.front().event_type;
		publications[event_type].push_back(std::make_pair(
			to_sub.front().component,
			to_sub.front().function)
		);
		to_sub.pop();
	}

	while (!to_unsub.empty()) {
		std::string event_type = to_unsub.front().event_type;
		int idx = 0;
		for (auto& [comp, func] : publications[event_type]) {
			if (to_unsub.front().component == comp && to_unsub.front().function == func) {
				publications[event_type].erase(publications[event_type].begin() + idx);
			}
		}
        to_unsub.pop();
	}
}
