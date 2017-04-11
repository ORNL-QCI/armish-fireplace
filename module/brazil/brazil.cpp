#include "brazil.hpp"

namespace module {
	namespace brazil {
		const module_register<brazil> brazil::name("brazil");
		
		brazil::brazil()
				: imodule(::actions::pack<
					::actions::actions_t::PUSH,
					::actions::actions_t::WAIT>()) {
			imodule::proc_unit_list_t m;
			m["bobwire_circuit"] = &proc_unit::bobwire_circuit::initialize;
			m["trx_circuit"] = &proc_unit::trx_circuit::initialize;
			m["lcc_and_fpga"] = &proc_unit::lcc_and_fpga::initialize;
			register_proc_units(std::move(m));
		}
		
		void brazil::string_initialize_parameters(const char* const parameters) {
			UNUSED(parameters);
		}
		
		::module::imodule::response* brazil::proc_act_request(
				const ::module::imodule::request& request) {
			return loaded_proc_unit().proc_act_request(request);
		}
		
		bool brazil::proc_act_push(const ::module::imodule::request& request) {
			return loaded_proc_unit().proc_act_push(request);
		}
	}
}
