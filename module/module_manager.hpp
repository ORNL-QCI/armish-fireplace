#ifndef _MODULE_MODULE_MANAGER_HPP
#define _MODULE_MODULE_MANAGER_HPP

#include <common.hpp>
#include <module/imodule.hpp>
#include <module/iproc_unit.hpp>
#include <module/module_list.hpp>
#include <mutex>

namespace net {
	namespace middleware {
		class server;
	}
}

namespace module {
	/**
	 * \brief A class which loads a module and controls the included modules.
	 */
	class module_manager {
	 private:
		/**
		 * \brief Standard mutex lock type for the class.
		 */
		using lock_t = std::lock_guard<std::mutex>;
	
	 public:
		/**
		 * \brief Callback container that holds instance and member function pointer to
		 * notify function when a new processing module is loaded.
		 * 
		 * \note Guaranteed to be a POD struct and thus be used with aggregate
		 * initialization.
		 */
		template <typename T>
				struct callback_t {
		 public:
			/**
			 * \brief Type to function callback.
			 */
			typedef void(T::*mfp_t)(::buffer::queue_buffer&, const ::actions::actions_list_t);
			
			/**
			 * \brief Instance of class.
			 */
			T* instance;
			
			/**
			 * \brief Member function pointer.
			 */
			mfp_t callback;
		};
		
		/**
		 * \brief Type alias for an imodule constructor.
		 */
		using module_constructor_t = imodule*();
		
		/**
		 * \brief Type alias for a processing unit constructor.
		 */
		using proc_unit_constructor_t = iproc_unit*();
		
		/**
		 * \brief Constructor.
		 */
		module_manager();
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		module_manager(const module_manager&) = delete;
		
		/**
		 * \brief Move constructor is disabled.
		 */
		module_manager(module_manager&&) = delete;
		
		/**
		 * \brief Copy assignment operator is disabled.
		 */
		module_manager& operator=(const module_manager&) = delete;
		
		/**
		 * \brief Move assignment operator is disabled.
		 */
		module_manager& operator=(module_manager&&) = delete;
		
		/**
		 * \brief Destructor unloads a loaded module.
		 */
		~module_manager();
		
		/**
		 * \brief Load a module by its name and with parameter string.
		 * 
		 * If a module is already loaded we do nothing and return.
		 * 
		 * \note Threadsafe.
		 */
		void load_module(const char* const name, const char* const parameters);
		
		/**
		 * \brief Load a processing unit within the given loaded module by name and with
		 * a parameter string.
		 * 
		 * If a processing unit is already loaded or if no module is loaded, we do nothing
		 * and return. If the processing unit has async processing threads, we start them.
		 * 
		 * \note Threadsafe.
		 */
		void load_proc_unit(const char* const name, const char* const parameters);
		
		/**
		 * \brief Unload the currently loaded module.
		 * 
		 * If no module is loaded, we do nothing and return. If a processing unit is
		 * loaded, we unload that first.
		 * 
		 * \note Threadsafe.
		 */
		void unload_module();
		
		/**
		 * \brief Unload the currently loaded processing unit.
		 * 
		 * If no processing unit or no module is loaded, we do nothing and return. If the
		 * processing unit has async processing threads, we stop them.s
		 * 
		 * \note Threadsafe.
		 */
		void unload_proc_unit();
		
		/**
		 * \brief Process an incoming request action from a client..
		 * 
		 * \note Threadsafe with respect to the state of the loaded module and processing
		 * unit, i.e. the currently loaded module and processing unit cannot be unloaded
		 * while this function is executing. Other threadsafety depends on
		 * proc_act_request() implementation in the loaded module. See module interface
		 * class for more details.
		 * 
		 * \throws If no module and processing unit is loaded, we throw a runtime_error.
		 */
		imodule::response* proc_act_request(const imodule::request& request);
		
		/**
		 * \brief Process an incoming push action from a client.
		 * 
		 * \note Threadsafe with respect to the state of the loaded module and processing
		 * unit, i.e. the currently loaded module and processing unit cannot be unloaded
		 * while this function is executing. Other threadsafety depends on
		 * proc_act_push() implementation in the loaded module. See module interface
		 * class for more details.
		 * 
		 * \throws If no module and processing unit is loaded, we throw a runtime_error.
		 */
		bool proc_act_push(const imodule::request& request);
		
		/**
		 * \brief Return whether or not a module is currently loaded.
		 * 
		 * \note Threadsafe.
		 */
		bool is_module_loaded();
		
		/**
		 * \brief Return whether or not a processing unit is currently loaded with the
		 * loaded module.
		 * 
		 * If no module is loaded, this returns false.
		 * 
		 * \note Threadsafe.
		 */
		bool is_proc_unit_loaded();
		
		/**
		 * \brief Return what actions are supported by the currently loaded module.
		 *
		 * \note Threadsafe.
		 * 
		 * \throws If no module and processing unit is loaded, we throw a runtime_error.
		 */
		::actions::actions_list_t supported_actions();
		
		/**
		 * \brief Register a callback to be used when the loaded processing unit changes.
		 * 
		 * \note Threadsafe.
		 * 
		 * \warning Specializations must be threadsafe.
		 */
		template<typename T>
				void register_callback(callback_t<T>&& callback) {
			lock_t lock(stateMutex);
			
			serverCallback = callback;
		}
		
		/**
		 * \brief Unregister the registered callback.
		 * 
		 * If no callback is registered, we do nothing and return.
		 *
		 * \note Threadsafe.
		 */
		void unregister_callback();
		
		/**
		 * \brief Return whether or not a valid callback is registered.
		 * 
		 * \note Threadsafe.
		 */
		bool is_callback_registered();
	
	 private:
		/**
		 * \brief The loaded module.
		 */
		::module::imodule* loadedModule;
		
		/**
		 * \brief The registered callback to notify the server of an updated state.
		 */
		callback_t<::net::middleware::server> serverCallback;
		
		/**
		 * \brief Mutex for functions modifying the state of the loaded module and
		 * processing unit.
		 */
		std::mutex stateMutex;
	};
}

#endif
