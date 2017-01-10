#ifndef _MODULE_TREBEA_PROC_UNIT_CIRCULATOR_SWITCH_HPP
#define _MODULE_TREBEA_PROC_UNIT_CIRCULATOR_SWITCH_HPP

#include <common.hpp>
#include "../iswitch_proc_unit.hpp"
#include "../../../net/simulation/client.hpp"

namespace module {
	namespace trabea {
		namespace proc_unit {
			/**
			 * \brief A circulator switch is a switch that has multiple ports connected
			 * in such a way that they may be described via a circulant matrix. That is,
			 * for example in a 3 port switch, 1->2, 2->3, 3->1.
			 */
			class circulator_switch : public ::module::trabea::iswitch_proc_unit {
			 private:
		 		/**
				 * \brief Standard mutex lock type for the class.
				 */
				using lock_t = std::lock_guard<std::mutex>;
				
				/**
				 * \brief The rotations the switch may be in.
				 */
				enum class chirality {
					cw,
					ccw
				};
				
				/**
				 * \brief convert a chirality state to string.
				 */
				static const char* chirality_to_string(const chirality state);
			
			 public:
				/**
				 * \brief Constructor starts the switch in some arbitrary state.
				 */
				circulator_switch();
				
				/**
				 * \brief Copy constructor.
				 */
				circulator_switch(const circulator_switch&) = default;
				
				/**
				 * \brief Move constructor.
				 */
				circulator_switch(circulator_switch&&) = default;
				
				/**
				 * \brief Assignment operator.
				 */
				circulator_switch& operator=(const circulator_switch&) = default; 
				
				/**
				 * \brief Move assignment operator.
				 */
				circulator_switch& operator=(circulator_switch&&) = default;
				
				/**
				 * \brief Destructor.
				 */
				~circulator_switch();
				
				/**
				 * \brief Return a new instance of the class.
				 */
				static ::module::iproc_unit* initialize();
				
				/**
				 * \brief Initialize the switch with a parameter string.
				 */
				void string_initialize_parameters(const char* const parameters);
			
			 protected:
				/**
				 * \brief Return whether or not two ports are currently connected.
				 */
				bool get_switch_state(const std::size_t inPort,
						const std::size_t outPort);
				
				/**
				 * \brief Set the switch such that two ports are connected.
				 */
				bool set_switch_state(const std::size_t inPort,
						const std::size_t outPort);
			
			 private:
				/**
				 * \brief Connection to simulation dispatcher.
				 */
				::net::simulation::client* dispatcher;
				
				/**
				 * \brief Numerical IP address of the switch.
				 */
				std::uint_fast32_t nIP;
				
				/**
				 * \brief The number of ports on the switch.
				 */
				std::size_t portCount;
				
				/**
				 * \brief Current state of the switch.
				 */
				chirality state;
			};
		}
	}
}

#endif
