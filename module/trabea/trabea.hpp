#ifndef _MODULE_TRABEA_TRABEA_HPP
#define _MODULE_TRABEA_TRABEA_HPP

#include <common.hpp>
#include "../imodule.hpp"
#include "proc_unit/circulator_switch.hpp"

namespace module {
	namespace trabea {
		/**
		 * \brief A module for switches.
		 */
		class trabea : public ::module::imodule {
		 public:
			/**
			 * \brief Constructor.
			 * 
			 * Registers processing units.
			 */
			trabea();
			
			/**
			 * \brief Initialize the module with a parameter string.
			 */
			void string_initialize_parameters(const char* const parameters);
			
			/**
			 * \brief Process a request action and produce an response.
			 *
			 * \note Threadsafe.
			 */
			::module::imodule::response* proc_act_request(
					const ::module::imodule::request& request);
			
			/**
			 * \brief Process a push action.
			 * 
			 * \note Threadsafe.
			 * 
			 * \returns Whether or not we successfully processed the action.
			 */
			bool proc_act_push(const ::module::imodule::request& request);
		
		 private:
			/**
			 * \brief Register module with factory.
			 */
			static const module_register<trabea> name;
		};
	}
}
#endif
