#include "ComponentDB.hpp"
#include "Listener.hpp"
#include "Actor.hpp"

void report_lua_error(const std::string& actor_name, const luabridge::LuaException& e);

std::string Actor::GetName() {
	return name;
}

int Actor::GetID() {
	return id;
}

luabridge::LuaRef Actor::GetComponentByKey(std::string key) {
	if (components.find(key) == components.end() || 
		keys_to_remove.find(key) != keys_to_remove.end()) {
		return luabridge::LuaRef(ComponentDB::__State);
	}
	return *(components[key]).ref;
}

luabridge::LuaRef Actor::GetComponent(std::string type_name) {
	
	auto keys = component_type_to_key[type_name];
	keys.sort();

	if (keys.empty()) {
		lua_pushnil(ComponentDB::__State);
		return luabridge::LuaRef::fromStack(ComponentDB::__State);
	}
	for (auto& key : keys) {
		if (keys_to_remove.find(key) != keys_to_remove.end()) {
			lua_pushnil(ComponentDB::__State);
			return luabridge::LuaRef::fromStack(ComponentDB::__State);
		}
	}
	
	return *components[*keys.begin()].ref;
}

luabridge::LuaRef Actor::GetComponents(std::string type_name) {
	if (component_type_to_key.find(type_name) == component_type_to_key.end()) {
		return luabridge::newTable(ComponentDB::__State);
	}

	luabridge::LuaRef table = luabridge::newTable(ComponentDB::__State);

	std::list<std::string>& keys = component_type_to_key[type_name];
	keys.sort();

	int idx = 1;

	for (auto& key : keys) {
		if (keys_to_remove.find(key) != keys_to_remove.end()) {
			return luabridge::newTable(ComponentDB::__State);
		} else {
			luabridge::LuaRef component = *components[key].ref;
			table[idx] = component;
		}
		idx++;
	}

	return table;
}

luabridge::LuaRef Actor::AddComponent(std::string type_name) {

	std::string key = "r" + std::to_string(ComponentDB::runtime_components_added);
	++ComponentDB::runtime_components_added;

	if (type_name == "Rigidbody") {
		RigidBody rb;
		Component to_add;

		to_add.type = "Rigidbody";

		luabridge::push(ComponentDB::__State, rb);
		luabridge::LuaRef ref = luabridge::LuaRef::fromStack(ComponentDB::__State, -1);
		to_add.ref = std::make_shared<luabridge::LuaRef>(ref);

		to_add.inject_variable("key", key);

		component_type_to_key["Rigidbody"].push_back(key);
		components_to_add.push(std::make_shared<Component>(to_add));
		on_start_keys.insert(key);

		return *(to_add.ref);
	}

	// if component doesn't exist in the database, create on
	if (!ComponentDB::component_type_exists(type_name)) {
		Component comp_new;
		comp_new.load_ref(type_name, ComponentDB::__State);
		ComponentDB::components[type_name] = comp_new;
	}

	//create the new child component
	Component to_add;

	ComponentDB::create_child(key, to_add, ComponentDB::components[type_name]);
	//add it to the queue of components that will be attached post update()
	components_to_add.push(std::make_shared<Component>(to_add));
	
	return *(to_add.ref);
}

void Actor::RemoveComponent(luabridge::LuaRef comp_ref) {
	std::string key = comp_ref["key"];
	std::string type = components[key].type;
	comp_ref["enabled"] = false;
	keys_to_remove.insert(key);
}

void Actor::Start() {
	for (auto& key : on_start_keys) {

		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnStart"](self);
		} catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::Update() {

	for (auto key : on_update_keys) {

		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnUpdate"](self);
		} catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::LateUpdate() {

	for (auto key : on_late_update_keys) {

		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnLateUpdate"](self);
		} catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::OnDestroy() {

	// case for actor destruction/scene change
	if (to_destroy) {
		for (auto [key, component] : components) {

			if (!component.has_on_destroy) {
				continue;
			}
			luabridge::LuaRef self = *component.ref;
			try {
				self["OnDestroy"](self);
			}
			catch (const luabridge::LuaException& e) {
				report_lua_error(name, e);
			}
		}
	}

	// case for component remmoval
	for (auto key : keys_to_remove) {
		if (!components[key].has_on_destroy && 
			components[key].type != "Rigidbody") {
			continue;
		}

		luabridge::LuaRef self = *components[key].ref;
		try {
			self["OnDestroy"](self);
		}
		catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::EnterCollision(Collision& collision) {
	for (auto key : on_collision_enter_keys) {
		std::string update = "OnCollisionEnter";
		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnCollisionEnter"](self, collision);
		}
		catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::ExitCollision(Collision& collision) {
	for (auto key : on_collision_exit_keys) {
		std::string update = "OnCollisionExit";
		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnCollisionExit"](self, collision);
		}
		catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::EnterTrigger(Collision& collision) {
	for (auto key : on_trigger_enter_keys) {
		std::string update = "OnTriggerEnter";
		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnTriggerEnter"](self, collision);
		}
		catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

void Actor::ExitTrigger(Collision& collision) {
	for (auto key : on_trigger_exit_keys) {
		std::string update = "OnTriggerExit";
		luabridge::LuaRef self = *components[key].ref;
		if (!(self)["enabled"]) {
			continue;
		}
		try {
			self["OnTriggerExit"](self, collision);
		}
		catch (const luabridge::LuaException& e) {
			report_lua_error(name, e);
		}
	}
}

// resources/
std::string truncate_resources_path(std::string& path) {
	size_t start = path.find("resources");
	return path.substr(start);
}

void report_lua_error(const std::string& actor_name, const luabridge::LuaException& e) {
	std::string error_message = e.what();

	std::replace(error_message.begin(), error_message.end(), '\\', '/');

	std::cout << "\033[31m" << actor_name << " : " 
		<< truncate_resources_path(error_message) << "\033[0m" << std::endl;
}

void Actor::inherit_from_template(const Actor& actor_template) {
	name = actor_template.name;

	for (auto& [key, template_comp] : actor_template.components) {
		Component new_comp;
		ComponentDB::create_child(key, new_comp, template_comp);
		attach_component(new_comp, key);
	}
}

void Actor::inject_reference_to_self(std::shared_ptr<luabridge::LuaRef> component) {
	(*component)["actor"] = this;
	self_ref = std::make_shared<luabridge::LuaRef>((*component)["actor"]);
}

void Actor::attach_component(Component& owned_component, const std::string& key) {
	std::string type = owned_component.type;
	components.insert(std::make_pair(key, owned_component));
	component_type_to_key[type].push_back(key);

	if (owned_component.has_start) {
		on_start_keys.insert(key);
	}
	if (owned_component.has_update) {
		on_update_keys.insert(key);
	}
	if (owned_component.has_late_update) {
		on_late_update_keys.insert(key);
	}
	if (owned_component.has_enter_collision) {
		on_collision_enter_keys.insert(key);
	}
	if (owned_component.has_exit_collision) {
		on_collision_exit_keys.insert(key);
	}
	if (owned_component.has_enter_trigger) {
		on_trigger_enter_keys.insert(key);
	}
	if (owned_component.has_exit_trigger) {
		on_trigger_exit_keys.insert(key);
	}

	if (type != "Rigidbody") {
		inject_reference_to_self(owned_component.ref);
	}
}

void Actor::remove_components() {
	if (!keys_to_remove.empty()) {

		OnDestroy();
	}
	for (auto key : keys_to_remove) {
		std::string type = components[key].type;

		components.erase(key);
		on_start_keys.erase(key);
		on_update_keys.erase(key);
		on_late_update_keys.erase(key);
		on_collision_enter_keys.erase(key);
		on_collision_exit_keys.erase(key);
		on_trigger_enter_keys.erase(key);
		on_trigger_exit_keys.erase(key);

		component_type_to_key.erase(type);
	}
	keys_to_remove.clear();
}

void Actor::add_components() {
	while (!components_to_add.empty()) {
		std::string key = (*(*components_to_add.front()).ref)["key"];
		attach_component(*components_to_add.front(), key);
		components_to_add.pop();
	}
}
