#include "trabea.hpp"

namespace module {
	namespace trabea {
		const module_register<trabea> trabea::name("trabea");
		
		trabea::trabea()
				: imodule(
					::actions::pack<::actions::actions_t::REQUEST,
							::actions::actions_t::PUSH>()
				) {
			imodule::proc_unit_list_t m;
			m["circulator_switch"] = &proc_unit::circulator_switch::initialize;
			
			register_proc_units(std::move(m));
		}
		
		void trabea::string_initialize_parameters(const char* const parameters) {
			UNUSED(parameters);
		}
		
		::module::imodule::response* trabea::proc_act_request(
				const ::module::imodule::request& request) {
			return loaded_proc_unit().proc_act_request(request);
		}
		
		bool trabea::proc_act_push(const ::module::imodule::request& request) {
			return loaded_proc_unit().proc_act_push(request);
		}
	}
}
