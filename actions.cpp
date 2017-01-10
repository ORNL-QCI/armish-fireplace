#include <actions.hpp>

namespace actions {
	actions_t str_map(const char* const str) {
		if(strcmp(str, "request") == 0) {
			return actions_t::REQUEST;
		} else if(strcmp(str, "reply") == 0) {
			return actions_t::REPLY;
		} else if(strcmp(str, "wait") == 0) {
			return actions_t::WAIT;
		} else if(strcmp(str, "push") == 0) {
			return actions_t::PUSH;
		} else {
			throw std::runtime_error(err_msg::_undhcse);
		}
	}
}