#ifndef _MODULE_IPROC_UNIT_HPP
#define _MODULE_IPROC_UNIT_HPP

#include <common.hpp>
#include <buffer/queue_buffer.hpp>
#include <net/middleware/request.hpp>
#include <net/middleware/response.hpp>

namespace module {
	/**
	 * \brief The interface definition for processing units that exist within a module.
	 * 
	 * A processing unit is the processing code in a module. Each unit represents
	 * a different configuration (hardware or software) which are unified under a common
	 * module, which contains code common to all units within it.
	 */
	class iproc_unit {
	 public:
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
		iproc_unit();
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		iproc_unit(const iproc_unit&) = delete;
		
		/**
		 * \brief Move constructor is disabled.
		 */
		iproc_unit(iproc_unit&&) = delete;
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		iproc_unit& operator=(const iproc_unit&) = delete; 
		
		/**
		 * \brief Move assignment operator is disabled.
		 */
		iproc_unit& operator=(iproc_unit&&) = delete;
		
		/**
		 * \brief Virtual destructor.
		 */
		virtual ~iproc_unit();
		
		/**
		 * \brief Initialize a new instance of the processing unit.
		 * 
		 * \warning We must override this in an implementation.
		 */
		static iproc_unit* initialize();
		
		/**
		 * \brief Initialize module with a parameter string.
		 * 
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual void string_initialize_parameters(const char* const parameters) = 0;
		
		/**
		 * \brief Function that, if asynchronous actions are supported, is launched on a
		 * seperate thread and pushes items into the supplied output buffer.
		 * 
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual void async_work(::buffer::queue_buffer& out) = 0;
		
		/**
		 * \brief Process a sync request.
		 * 
		 * This is used when the module does not have code to directly process or
		 * preprocess it, so we send it to the proccesing unit.
		 *
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual response* proc_act_request(const request& request) = 0;
		
		/**
		 * \brief Process a sync push.
		 * 
		 * This is used when the module does not have code to directly process or
		 * preprocess it, so we send it to the processing unit.
		 *
		 * \warning The implementation of this function must be threadsafe.
		 */
		virtual bool proc_act_push(const request& request) = 0;
	};
}

#endif
