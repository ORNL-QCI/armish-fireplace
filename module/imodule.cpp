#include "imodule.hpp"

namespace module {
	imodule::imodule(const ::actions::actions_list_t supportedActions)
			: supportedActions(supportedActions),
			loadedProcUnit(NULL),
			isRunning(false),
			doExit(false) {
	}
	
	imodule::~imodule() {
		unload_proc_unit();
	}
	
	void imodule::load_proc_unit(const char* const name,
			const char* const parameters) {
		lock_t lock(stateMutex);
		
		if(UNLIKELY(loadedProcUnit != NULL)) {
			return;
		}
		
		for(auto item : procUnits) {
			if(strcmp(item.first, name) == 0) {
				loadedProcUnit = item.second();
				loadedProcUnit->string_initialize_parameters(parameters);
				
				if(::actions::check<::actions::actions_t::REPLY>(supportedActions) ||
						::actions::check<::actions::actions_t::WAIT>(supportedActions)) {
					_start_async_proc(lock);
				}
				
				return;
			}
		}
		
		throw std::invalid_argument(err_msg::_tpntfnd);
	}
	
	void imodule::unload_proc_unit() {
		lock_t lock(stateMutex);
		
		if(UNLIKELY(loadedProcUnit == NULL)) {
			return;
		}
		if(::actions::check<::actions::actions_t::REPLY>(supportedActions) ||
						::actions::check<::actions::actions_t::WAIT>(supportedActions)) {
			_stop_async_proc(lock);
		}
		delete loadedProcUnit;
		loadedProcUnit = NULL;
	}
	
	bool imodule::is_proc_unit_loaded() {
		lock_t lock(stateMutex);
		
		return loadedProcUnit != 0;
	}
	
	void imodule::_start_async_proc(lock_t& lock) {
		// The lock argument is just to ensure this function is threadsafe.
		UNUSED(lock);
		
		if(UNLIKELY(loadedProcUnit == NULL)) {
			return;
		}
		
		if(!isRunning) {
			asyncProcThread = std::thread(&imodule::_async_proc_wrapper, this);
			
			isRunning = true;
		}
	}
	
	void imodule::_stop_async_proc(lock_t& lock) {
		// The lock argument is just to ensure this function is threadsafe.
		UNUSED(lock);
		
		if(UNLIKELY(loadedProcUnit == NULL)) {
			return;
		}
		
		if(isRunning) {
			doExit = true;
			
			asyncProcThread.join();
			
			doExit = false;
			isRunning = false;
		}
	}
	
	void imodule::_async_proc_wrapper() {
		while(!doExit) {
			loadedProcUnit->async_work(asyncBuffer);
			
			/** \todo */
			//std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(5));
		}
	}
	
	module_factory::module_map_type module_factory::_module_map;
}
