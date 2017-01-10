#ifndef _MODULE_TRABEA_ISWITCH_PROC_UNIT_HPP
#define _MODULE_TRABEA_ISWITCH_PROC_UNIT_HPP

#include <common.hpp>
#include <module/iproc_unit.hpp>
#include <arpa/inet.h>

namespace module {
	namespace trabea {
		/**
		 * \brief Interface for specialized processing unit for trabea switches.
		 */
		class iswitch_proc_unit : public ::module::iproc_unit {
		 public:
			/**
			 * \brief Standard mutex lock type for the class.
			 */
			using lock_t = std::lock_guard<std::mutex>;
			
			/**
			 * \brief Alias declaration type of the request object supplied to processing
			 * functions.
			 */
			using request = ::module::iproc_unit::request;
			
			/**
			 * \brief Alias declaration type of the response object created by processing
			 * functions.
			 */
			using response = ::module::iproc_unit::response;
			
			/**
			 * \brief Constructor.
			 */
			iswitch_proc_unit();
			
			/**
			 * \brief Copy constructor.
			 */
			iswitch_proc_unit(const iswitch_proc_unit&) = default;
			
			/**
			 * \brief Move constructor.
			 */
			iswitch_proc_unit(iswitch_proc_unit&&) = default;
			
			/**
			 * \brief Assignment operator.
			 */
			iswitch_proc_unit& operator=(const iswitch_proc_unit&) = default; 
			
			/**
			 * \brief Move assignment operator.
			 */
			iswitch_proc_unit& operator=(iswitch_proc_unit&&) = default;
			
			/**
			 * \brief Virtual destructor.
			 */
			virtual ~iswitch_proc_unit();
			
			/**
			 * \brief Initialize a new instance of the processing unit.
			 * 
			 * \warning We must override this in an implementation.
			 */
			static iswitch_proc_unit* initialize();
			
			/**
			 * \brief This processing unit does not currently have any async actions
			 * that can be processed.
			 * 
			 * \note Threadsafe.
			 */
			void async_work(::buffer::queue_buffer& out);
			
			/**
			 * \brief Process a request action.
			 * 
			 * \note Threadsafe.
			 */
			response* proc_act_request(
					const request& request);
			
			/**
			 * \brief Process a push action.
			 * 
			 * \note Threadsafe.
			 */
			bool proc_act_push(const request& request);
		
		 protected:
			/**
			 * \brief Return whether or not two ports are currently connected.
			 * 
			 * \note The implementation of this function will be called by a thread
			 * with a mutex lock on the state of the switch, blocking other calls to this
			 * function and the corresponding set function.
			 */
			virtual bool get_switch_state(const std::size_t inPort,
					const std::size_t outPort) = 0;
			
			/**
			 * \brief Set the switch such that two ports are connected.
			 * 
			 * \note The implementation of this function will be called by a thread
			 * with a mutex lock on the state of the switch, blocking other calls to this
			 * function and the corresponding set function.
			 */
			virtual bool set_switch_state(const std::size_t inPort,
					const std::size_t outPort) = 0;
		
		 private:
			/**
			 * \brief Mutex protector of the state of the switch.
			 */
			std::mutex stateMutex;
		};
	}
}

#endif
