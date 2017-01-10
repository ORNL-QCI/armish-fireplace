#include "module_manager.hpp"

namespace module {
	module_manager::module_manager()
			: loadedModule(NULL),
			serverCallback{NULL, NULL} {
	}
	
	module_manager::~module_manager() {
		unload_module();
	}
	
	void module_manager::load_module(const char* const name,
			const char* const parameters) {
		lock_t lock(stateMutex);
		
		if(UNLIKELY(loadedModule != NULL)) {
			return;
		}
		
		loadedModule = module_factory::instantiate(name);
		loadedModule->string_initialize_parameters(parameters);
	}
	
	void module_manager::load_proc_unit(const char* const name,
			const char* const parameters) {
		lock_t lock(stateMutex);
		
		if(UNLIKELY(loadedModule == NULL || loadedModule->is_proc_unit_loaded())) {
			return;
		}
		
		loadedModule->load_proc_unit(name, parameters);
		
		#ifdef THROW
		if(UNLIKELY(serverCallback.instance == NULL || serverCallback.callback == NULL)) {
			throw std::runtime_error(err_msg::_nllpntr);
		}
		#endif
		
		((*serverCallback.instance).*serverCallback.callback)(loadedModule->async_buffer(),
				loadedModule->supported_actions());
	}
	
	void module_manager::unload_module() {
		lock_t lock(stateMutex);
		
		if(UNLIKELY(loadedModule == NULL)) {
			return;
		}
		
		loadedModule->unload_proc_unit();
		delete loadedModule;
		loadedModule = NULL;
	}
	
	void module_manager::unload_proc_unit() {
		lock_t lock(stateMutex);
		
		if(UNLIKELY(loadedModule == NULL)) {
			return;
		}
		
		loadedModule->unload_proc_unit();
	}
	
	imodule::response* module_manager::proc_act_request(
			const imodule::request& request) {
		lock_t lock(stateMutex);
		
		#ifdef THROW
		if(UNLIKELY(loadedModule == NULL || !loadedModule->is_proc_unit_loaded())) {
			throw std::runtime_error(err_msg::_nllpntr);
		}
		#endif
	
		return loadedModule->proc_act_request(request);
	}
	
	bool module_manager::proc_act_push(const imodule::request& request) {
		lock_t lock(stateMutex);
		
		#ifdef THROW
		if(UNLIKELY(loadedModule == NULL || !loadedModule->is_proc_unit_loaded())) {
			throw std::runtime_error(err_msg::_nllpntr);
		}
		#endif
	
		return loadedModule->proc_act_push(request);
	}
	
	bool module_manager::is_module_loaded() {
		lock_t lock(stateMutex);
		
		return loadedModule != NULL;
	}
	
	bool module_manager::is_proc_unit_loaded() {
		lock_t lock(stateMutex);
		
		return loadedModule != NULL && loadedModule->is_proc_unit_loaded();
	}
	
	::actions::actions_list_t module_manager::supported_actions() {
		lock_t lock(stateMutex);
		
		#ifdef THROW
		if(UNLIKELY(!is_module_loaded() || !loadedModule->is_proc_unit_loaded())) {
			throw std::runtime_error(err_msg::_nllpntr);
		}
		#endif
		
		return loadedModule->supported_actions();
	}
	
	void module_manager::unregister_callback() {
		lock_t lock(stateMutex);
		
		serverCallback.instance = NULL;
		serverCallback.callback = NULL;
	}
	
	bool module_manager::is_callback_registered() {
		lock_t lock(stateMutex);
		
		return (serverCallback.instance == NULL && serverCallback.callback == NULL);
	}
}
