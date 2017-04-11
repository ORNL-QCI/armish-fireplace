#ifndef _MODULE_BRAZIL_BRAZIL_HPP
#define _MODULE_BRAZIL_BRAZIL_HPP

#include <common.hpp>
#include "../imodule.hpp"
#include "proc_unit/bobwire_circuit.hpp"
#include "proc_unit/trx_circuit.hpp"
#include "proc_unit/lcc_and_fpga.hpp"

namespace module {
	namespace brazil {
		/**
		 * \brief A module for client communication.
		 */
		class brazil : public ::module::imodule {
		 public:
			/**
			 * \brief Constructor.
			 * 
			 * Registers processing units.
			 */
			brazil();
			
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
			static const module_register<brazil> name;
		};
	}
}
#endif
