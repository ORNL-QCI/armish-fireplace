#ifndef _NET_MIDDLEWARE_ZMQ_SERVER_HPP
#define _NET_MIDDLEWARE_ZMQ_SERVER_HPP

#include <common.hpp>
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"
#include <cppzmq/zmq.hpp>

namespace net {
	namespace middleware {
		/**
		 * \brief A zmq server for interfacing with the client's software layer.
		 */
		class zmq_server : public server {
		 public:
			/**
			 * \brief Constructor takes a mutable reference to a module manager.
			 * 
			 * This registers a notify callback with the module manager which is called
			 * when the state of the manager is updated, which means the server processing
			 * characteristics change to the requirements of the currently loaded module.
			 */
			zmq_server(::module::module_manager& moduleManager);
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			zmq_server(const zmq_server&) = delete;
			
			/**
			 * \brief Move constructor is disabled.
			 */
			zmq_server(zmq_server&&) = delete;
			
			/**
			 * \brief Copy assignment operator is disabled.
			 */
			zmq_server& operator=(const zmq_server&) = delete;
			
			/**
			 * \brief Move assignment operator is disabled.
			 */
			zmq_server& operator=(zmq_server&&) = delete;
			
			/**
			 * \brief Destructor cleans up resources allocated in this child.
			 */
			virtual ~zmq_server();
		
		 private:
			/*
			 * \brief The timeout period when waiting to receive a tx request from the
			 * client.
			 * 
			 * Changing this will alter the responsiveness when stopping the server and
			 * the idle CPU usage. The former and latter have an inverse relationship.
			 * 
			 * \note Milliseconds.
			 */
			static const int sync_send_to = 300;
			
			/**
			 * \brief The timeout period when waiting to receive confirmation a response
			 * was sent to a client in response to their tx request.
			 * 
			 * Changing this will alter the responsiveness when stopping the server and
			 * the reliability of transmission to the client. This should be increased if
			 * network is congested.
			 * 
			 * \note Milliseconds.
			 */
			static const int sync_receive_to = 300;
			
			/**
			 * \brief The timeout period when waiting to send a response to the client.
			 * 
			 * \note Milliseconds.
			 */
			static const int async_send_to = 300;
			
			/**
			 * \brief The timeout period when waiting to receive confirmation a response
			 * was sent to a client.
			 * 
			 * \note Milliseconds.
			 */
			static const int async_receive_to = 100;
			
			/**
			 * \brief The timeout period when waiting to achieve the desired buffer size
			 * before pushing rx data to the client.
			 * 
			 * Making this smaller will make the buffer less likely to achieve the desired
			 * size, increasing the failure rate, and making the buffer flush before the
			 * desired size is reached.
			 * 
			 * \note Milliseconds.
			 */
			static const int async_wait_to = 300;
			
			/**
			 * \brief The number of items in the rx buffer we want before we empty it.
			 * 
			 * Changing this affects the throughput of the rx server and the overhead 
			 * associated with locking and unlocking the queue. Making this smaller will
			 * make the rx thread more active as it will spend less time sleeping waiting
			 * for a condition variable.
			 */
			static const int async_wait_count = 100;
			
			/**
			 * \brief The number of times the rx thread will wait within the timeout
			 * period for the buffer to achieve the desired size.
			 * 
			 * Changing this will affect the responsiveness of the rx data being pushed to
			 * the client and the responsiveness when stopping the server.
			 */
			static const int async_wait_fail = 4;
		 
			/**
			 * \brief Sync action listening function that is called in a seperate thread.
			 * 
			 * \note Threadsafe.
			 */
			void sync_work();
			
			/**
			 * \brief Async action sending function that is called in a seperate thread.
			 * 
			 * \note Threadsafe.
			 */
			void async_work();
		};
	}
}

#endif
