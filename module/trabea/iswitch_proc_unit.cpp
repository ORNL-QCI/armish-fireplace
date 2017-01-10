#include "iswitch_proc_unit.hpp"

namespace module {
	namespace trabea{
		iswitch_proc_unit::iswitch_proc_unit() {
		}
		
		iswitch_proc_unit::~iswitch_proc_unit() {
		}
		
		iswitch_proc_unit* iswitch_proc_unit::initialize() {
			throw std::runtime_error(err_msg::_stcimpl);
		}
		
		void iswitch_proc_unit::async_work(::buffer::queue_buffer& out) {
			UNUSED(out);
			
			throw std::logic_error(err_msg::_unrchcd);
		}
		
		iswitch_proc_unit::response* iswitch_proc_unit::proc_act_request(
				const iswitch_proc_unit::request& request) {
			lock_t lock(stateMutex);
			
			const auto action = request.method();
			response* ret = NULL;
			
			if(strcmp(action, "get_state") == 0) {
				const auto result = get_switch_state(request.parameter<std::size_t>(0),
						request.parameter<std::size_t>(1));
				
				ret = new response(result);
			} else {
				throw std::runtime_error(err_msg::_malinpt);
			}
			
			return ret;
		}
		
		bool iswitch_proc_unit::proc_act_push(
				const iswitch_proc_unit::request& request) {
			lock_t lock(stateMutex);
			
			const auto action = request.method();
			bool ret = false;
			
			if(strcmp(action, "configure") == 0) {
				ret = set_switch_state(request.parameter<std::size_t>(0),
						request.parameter<std::size_t>(1));
			} else {
				throw std::runtime_error(err_msg::_malinpt);
			}
			
			return ret;
		}
	}
}
