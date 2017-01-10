#ifndef _NET_MIDDLEWARE_SERVER_HPP
#define _NET_MIDDLEWARE_SERVER_HPP

#include <common.hpp>
#include <buffer/queue_buffer.hpp>
#include <module/module_manager.hpp>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace net {
	namespace middleware {
		/**
		 * \brief A server for interfacing with the client's software layer.
		 */
		class server {
		 private:
			/**
			 * \brief Standard mutex lock type for the class.
			 */
			using lock_t = std::lock_guard<std::mutex>;
			
			/**
			 * \brief Standard start mutex lock type for the class.
			 */
			using start_lock_t = std::unique_lock<std::mutex>;
			
			/**
			 * \brief Alias type for wrapped referenced to module manager.
			 */
			using mdmn_ref_wrap_t = std::reference_wrapper<::module::module_manager>;
		
		 public:
			/**
			 * \brief Constructor takes a mutable reference to a module manager.
			 * 
			 * This registers a notify callback with the module manager which is called
			 * when the state of the manager is updated, which means the server processing
			 * characteristics change to the requirements of the currently loaded module.
			 */
			server(::module::module_manager& moduleManager);
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			server(const server&) = delete;
			
			/**
			 * \brief Move constructor is disabled.
			 */
			server(server&&) = delete;
			
			/**
			 * \brief Copy assignment operator is disabled.
			 */
			server& operator=(const server&) = delete;
			
			/**
			 * \brief Move assignment operator is disabled.
			 */
			server& operator=(server&&) = delete;
			
			/**
			 * \brief Destructor stops the server.
			 */
			virtual ~server();
			
			/**
			 * \brief Set the incoming and outgoing endpoints.
			 * 
			 * If the server is running we do nothing and return. The incoming endpoint is
			 * used to listen for sync action requests and the outgoing endpoint is used
			 * to send outgoing async actions to.
			 * 
			 * \note Threadsafe.
			 * 
			 * \throws If the length of either endpoint exceeds the size of the endpoint
			 * string array then we throw an invalid_argument.
			 */
			void setup(const char* const iEndpoint,
					const char* const oEndpoint);
			
			/**
			 * \brief Stop any running server tasks and changes the state to stopped.
			 *
			 * If the server is not running we do nothing and return. The server can only
			 * be started again by the module manager calling the registered notify()
			 * function.
			 * 
			 * \note Threadsafe.
			 * 
			 * \throws Only throws std::logic_error if there is a serious programming
			 * error. Other exceptions thrown from thread.join() or thrown in the exiting
			 * of thread processing functions are suppressed.
			 */
			UNROLL_LOOP
			void stop();
			
			/**
			 * \brief The callback function called when the loaded processing unit changes
			 * which may alter the configuration of the server.
			 * 
			 * We register this function in with the module manager. The buffer argument
			 * is a reference to the module's async action output buffer. Although the
			 * buffer always exists for a module and we always store a reference to it,
			 * the thread that pops items from it is run only if the module reports that
			 * it is pushing items into the buffer.
			 * 
			 * We need to supply the actions here as to avoid a deadlock trying to lock a
			 * mutex in the module manager that is alreayd locked by the thread calling
			 * this function.
			 * 
			 * \note Threadsafe.
			 */
			void notify(::buffer::queue_buffer& asyncBuffer,
					const ::actions::actions_list_t supActions);
			
			/**
			 * \brief Return whether or not the server is currently running.
			 * 
			 * \note Threadsafe.
			 */
			inline bool is_running() {
				lock_t lock(stateMutex);
				
				return isRunning;
			}
		
		 protected:
			/**
			 * \brief Return whether or not the server is in the process of shutting down.
			 * 
			 * \note Threadsafe only when called within implemented sync_work() or
			 * async_work() functions. Do NOT cache this as a child class member variable.
			 */
			inline bool do_exit() const {
				return doExit;
			}
			
			/**
			 * \brief Return the endpoint of the incoming sync traffic.
			 * 
			 * \note Threadsafe only when called within implemented sync_work() or
			 * async_work() functions. Do NOT cache this as a child class member variable.
			 */
			inline const char* get_iEndpoint() const {
				return iEndpoint;
			}
			
			/**
			 * \brief Return the endpoint of the outgoing async traffic.
			 * 
			 * \note Threadsafe only when called within implemented sync_work() or
			 * async_work() functions. Do NOT cache this as a child class member variable.
			 */
			inline const char* get_oEndpoint() const {
				return oEndpoint;
			}
			
			/**
			 * \brief Return a reference to the module manager linked with the server
			 * instance.
			 * 
			 * \note Threadsafe only when called within implemented sync_work() or
			 * async_work() functions. Do NOT cache this as a child class member variable.
			 */
			inline ::module::module_manager& module_manager() {
				return moduleManager;
			}
			
			/**
			 * \brief Return a reference to the async action buffer linked with the server
			 * instance.
			 * 
			 * \note Threadsafe only when called within implemented sync_work() or
			 * async_work() functions. Do NOT cache this as a child class member variable.
			 */
			inline ::buffer::queue_buffer& module_async_buffer() {
				return *moduleAsyncBuffer;
			}
			
			/**
			 * \brief Method used to signal that a launched thread is ready.
			 * 
			 * If the number of ready threads is equal to the total number threads then we
			 * notify any waiting threads with the condition variable.
			 * 
			 * \note Threadsafe.
			 * 
			 * \warning Only call once in both sync_work() and async_work().
			 */
			void notify_thread_started();
		
		 private:
			/**
			 * \brief A mutable reference to the module manage we register with.
			 */
			::module::module_manager& moduleManager;
			
			/**
			 * \brief A pointer to the loaded module's async buffer.
			 * 
			 * If the module manager has not called our notify function, this is NULL.
			 * 
			 * \note This object should have internal thread safety.
			 */
			::buffer::queue_buffer* moduleAsyncBuffer;
			
			/**
			 * \brief The endpoint of the incoming sync traffic, e.g. where we bind to.
			 */
			char iEndpoint[128];
			
			/**
			 * \brief The endpoint of the outgoing async traffic, e.g. where we connect
			 * to.
			 */
			char oEndpoint[128];
			
			/**
			 * \brief Whether or not the server is currently running.
			 */
			bool isRunning;
			
			/**
			 * \brief Flag used to signal to our threads to exit.
			 */
			std::atomic_bool doExit;
			
			/**
			 * \brief Mutex protector for the state of the server.
			 */
			std::mutex stateMutex;
			
			/**
			 * \brief Mutex protector for the startup of threads.
			 */
			std::mutex startMutex;
			
			/**
			 * \brief Condition variable used in conjuntion with startMutex to signal the
			 * main thread that threads have launched.
			 */
			std::condition_variable startCV;
			
			/**
			 * \brief The number of threads that have signaled that they are ready upon
			 * startup.
			 */
			AF_HUINTN startupState;
			
			/**
			 * \brief The number of threads that must signal that they are ready before
			 * notifying the main thread that startup is complete.
			 */
			AF_HUINTN startupStateTarget;
			
			/**
			 * \brief A container for the work threads.
			 * 
			 * This can contain a sync listening thread, an async sending thread, or both.
			 */
			std::thread workThreads[2];
			
			/**
			 * \brief Sync action listening function that is called in a seperate thread.
			 * 
			 * \warning Implementation must be threadsafe.
			 */
			virtual void sync_work() = 0;
			
			/**
			 * \brief Async action sending function that is called in a seperate thread.
			 * 
			 * \warning Implementation must be threadsafe.
			 */
			virtual void async_work() = 0;
		};
	}
}

#endif
