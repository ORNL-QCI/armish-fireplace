#ifndef _ACTIONS_HPP
#define _ACTIONS_HPP

#include <common.hpp>
#include <type_traits>

namespace actions {
	/**
	 * \brief Hack to emulate scoped enum.
	 * 
	 * Scoped enums do not currently work with variadic templates for some reason.
	 */
	struct _actions_list_s {
		/**
		 * Alias declaration type of an action, which since we use enums, is an integer.
		 */
		using _actions_t = AF_UINTN;
		
		/**
		 * \brief The list of actions a client may make.
		 */
		enum class _actions_list_e : _actions_t {
			PUSH = 1,
			WAIT = 2,
			REQUEST = 4,
			REPLY = 8,
		};
	};
	
	/**
	 * \brief Alias declaration type that refers to the base enum type of actions.
	 */
	using actions_t = _actions_list_s::_actions_list_e;
	
	/**
	 * \brief Alias declaration type of a list of actions.
	 */
	using actions_list_t = AF_UINTN;
	
	/**
	 * \brief End of recusion for variadic template function.
	 */
	template <actions_t Last>
			constexpr actions_list_t pack() {
		return static_cast<std::underlying_type<actions_t>::type>(Last);
	}
	
	/**
	 * \brief Pack a list of actions into a single action_list_t.
	 * 
	 * Since an action is an enum entry, this is merely the xor of actions.
	 */
	template <actions_t First, actions_t Second, actions_t ...Rest>
			constexpr actions_list_t pack() {
		return pack<First>() ^ pack<Second, Rest...>();
	}
	
	/**
	 * \brief Check if an action is contained within an action list.
	 */
	template <actions_t Query>
			constexpr bool check(const actions_list_t List) {
		static_assert(std::is_integral<actions_list_t>::value == true,
				"List must be numeric");
		static_assert(
				std::is_same<
					std::underlying_type<actions_t>::type,
					_actions_list_s::_actions_t
				>::value,
				"Query must be of type _actions_t or this function must be changed");
				
		return ((List & static_cast<_actions_list_s::_actions_t>(Query)) >>
				(std::size_t)(log(static_cast<_actions_list_s::_actions_t>(Query))/log(2)));
	}
	
	/**
	 * \brief Provides a mapping between string representations of actions and the action
	 * enum type.
	 */
	actions_t str_map(const char* const str);
}

#endif
