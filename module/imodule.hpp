#ifndef _MODULE_IMODULE_HPP
#define _MODULE_IMODULE_HPP

#include <common.hpp>
#include <actions.hpp>
#include <module/iproc_unit.hpp>
#include <buffer/queue_buffer.hpp>
#include <net/middleware/request.hpp>
#include <net/middleware/response.hpp>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>

namespace module {
	/**
	 * \brief The interface definition for modules.
	 * 
	 * A module is comprised of processing units, which define how actions and action
	 * methods are processed. Synchronous actions are processed on demand while, if the
	 * module supports them, asynchronous actions are processed in another thread.
	 */
	class imodule {
	 private:
		/**
		 * \brief Alias declaration type for processing unit constructor.
		 */
		using proc_unit_constructor = iproc_unit*(*)();
		
		/**
		 * \brief Standard mutex lock type for the class.
		 */
		using lock_t = std::lock_guard<std::mutex>;
	
	 public:
		/**
		 * \brief Alias declaration type used when models register their processing units.
		 */
		using proc_unit_list_t = std::map<const char* const, proc_unit_constructor>;
		
		/**
		 * \brief Alias declaration type of the request object supplied to processing
		 * functions.
		 */
		using request = ::net::middleware::request;
		
		/**
		 * \brief Alias declaration type of the response object created by processing
		 * functions.
		 */
		using response = ::net::middleware::response;
		
		/**
		 * \brief Constructor.
		 */
		imodule(const ::actions::actions_list_t supportedActions);
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		imodule(const imodule&) = delete;
		
		/**
		 * \brief Move constructor is disabled.
		 */
		imodule(imodule&&) = delete;
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		imodule& operator=(const imodule&) = delete; 
		
		/**
		 * \brief Move assignment operator is disabled.
		 */
		imodule& operator=(imodule&&) = delete;
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~imodule();
		
		/**
		 * \brief Initialize module with a parameter string.
		 * 
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual void string_initialize_parameters(const char* const parameters) = 0;
		
		/**
		 * \brief Process a sync request.
		 *
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual response* proc_act_request(const request& request) = 0;
		
		/**
		 * \brief Process a sync push.
		 *
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual bool proc_act_push(const request& request) = 0;
		
		/**
		 * \brief Load a processing unit by its name and with some parameters.
		 * 
		 * This starts async processing if it is requested by the module or transceiver.
		 * 
		 * \note Threadsafe.
		 */
		void load_proc_unit(const char* const name, const char* const parameters);
		
		/**
		 * \brief Unload the currently loaded processing unit.
		 * 
		 * If async processing threads are running, we stop them.
		 * 
		 * \note Threadsafe.
		 */
		void unload_proc_unit();
		
		/**
		 * \brief Return whether or not a processing unit is currently loaded in the
		 * module.
		 * 
		 * \note Threadsafe.
		 */
		bool is_proc_unit_loaded();
		
		/**
		 * \brief Return the list of actions the module supports.
		 * 
		 * \note Threadsafe.
		 */
		inline actions::actions_list_t supported_actions() const {
			return supportedActions;
		}
		
		/**
		 * \brief Return a reference to the module's async buffer.
		 * 
		 * \note Threadsafe, but the threadsafety of the returned object depends on the
		 * implemention of it.
		 */
		inline ::buffer::queue_buffer& async_buffer() {
			return asyncBuffer;
		}
	
	 protected:
		/**
		 * \brief Register a module's processing units.
		 */
		inline void register_proc_units(proc_unit_list_t&& procUnitList) {
			procUnits = procUnitList;
		}
		
		/**
		 * \brief The current loaded processing unit.
		 * 
		 * \note \TODO
		 */
		inline iproc_unit& loaded_proc_unit() {
			#ifdef THROW
			if(loadedProcUnit == NULL) {
				throw std::runtime_error(err_msg::_nllpntr);
			}
			#endif
			
			return *loadedProcUnit;
		}
	
	 private:
		/**
		 * \brief Supported actions.
		 */
		::actions::actions_list_t supportedActions;
		
		/**
		 * \brief List of processing units for the current module.
		 */
		proc_unit_list_t procUnits;
		
		/**
		 * \brief The loaded processing unit of the current model.
		 */
		iproc_unit* loadedProcUnit;
		
		/**
		 * \brief Whether or not the transceiver is currently running.
		 */
		bool isRunning;
		
		/**
		 * \brief Whether or not the transceiver is currently running and in the process
		 * of exiting.
		 */
		std::atomic_bool doExit;
		
		/**
	 	 * \brief The buffer of async items.
	 	 */
		::buffer::queue_buffer asyncBuffer;
		
		/**
		 * \brief The thread that is running async action processing, if required.
		 */
		std::thread asyncProcThread;
		
		/**
		 * \brief Mutex for transceiver modifying functions within the class.
		 */
		std::mutex stateMutex;
		
		/**
		 * \brief Function for starting threads with supplied lock.
		 * 
		 * This is meant to be called from functions within this class that already hold a
		 * lock on the mutex. Calling this when async actions are not supported results in
		 * undefined behavior.
		 * 
		 * \note Threadsafe.
		 */
		void _start_async_proc(lock_t& lock);
		
		/**
		 * \brief Function for stopping threads with supplied lock.
		 * 
		 * This is meant to be called from functions within this class that already hold a
		 * lock on the mutex. Calling this when async actions are not supported results in
		 * undefined behavior.
		 * 
		 * \note Threadsafe.
		 */
		void _stop_async_proc(lock_t& lock);
		
		/**
		 * \brief \todo
		 * 
		 * \todo
		 * 
		 * \note This function must only perform thread safe operations.
		 */
		void _async_proc_wrapper();
	};
	
	/**
	 * \brief Create a new module of type T.
	 */
	template<typename T> imodule* create_module() {
		return new T();
	}
	
	/**
	 * \brief Factory to create and keep track of modules.
	 */
	struct module_factory {
	 public:
		typedef std::map<const char* const, imodule*(*)()> module_map_type;
		
		/**
		 * \brief Instantiate a module from its name.
		 */
		static imodule* instantiate(const char* const name) {
			for(auto item : module_map()) {
				if(strcmp(item.first, name) == 0) {
					return item.second();
				}
			}
			throw std::invalid_argument(err_msg::_tpntfnd);
		}
		
	 protected:
		/**
		 * \brief Return the map of modules.
		 */
		static module_map_type& module_map() {
			return _module_map;
		}
	
	 private:
		static module_map_type _module_map;
	};
	
	/**
	 * \brief Template to register a module in the module factory.
	 */
	template<typename T> struct module_register : module_factory {
	 public:
		/**
		 * \brief Constructor takes a module to be referenced when instantiating later.
		 */
		module_register(const char* const name) {
			auto it(module_map().insert(std::make_pair(name, &create_module<T>)));
			if(!it.second) {
				throw std::runtime_error(err_msg::_rgstrfl);
			}
		}
	};
}

#endif
