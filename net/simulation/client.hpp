#ifndef _NET_SIMULATION_CLIENT_HPP
#define _NET_SIMULATION_CLIENT_HPP

#include <common.hpp>
#include "request.hpp"
#include "response.hpp"
#include <cppzmq/zmq.hpp>

/**
 * \brief The maximum string length for the endpoint, including null terminator.
 * 
 * Keep in mind this value will probably be padded by the compiler.
 * 
 * \note Bytes.
 */
#define NET_SIMULATION_CLIENT_ENDPNT_LENGTH 128

namespace net {
	namespace simulation {
		/**
		 * \brief A client used to connect to a given simulation server to send requests
		 * and receive responses.
		 * 
		 * The supplied protocol should be one that zmq supports. Hint: If you want the
		 * endpoint to be "localhost" you have to use "127.0.0.1" or use the device name,
		 * which on Linux is normally lo.
		 * 
		 * \warning Not threadsafe.
		 */
		class client {
		 public:
			/**
			 * \brief Constructor takes the endpoint to connect to.
			 * 
			 * \throws If the length of the endpoint is too long for our storage array,
			 * we throw std::invalid_argument.
			 */
			client(const char* const endpoint);
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			client(const client&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			client(client&& old);
			
			/**
			 * \brief Assignment operator is disabled.
			 */
			client& operator=(const client&) = delete;
			
			/**
			 * \brief Move assignment operator.
			 */
			client& operator=(client&& old);
			
			/**
			 * \brief Destructor will disconnect from the server.
			 */
			~client();
			
			/**
			 * \brief Send a request to the server and receive a response.
			 * 
			 * \throws If there are any transmission or reception problems, we throw
			 * std::runtime_error.
			 */
			response call(request& request);
		
		 private:
			/**
			 * \brief The cstring endpoint.
			 */
			char endpoint[NET_SIMULATION_CLIENT_ENDPNT_LENGTH];
			
			/**
			 * \brief The zmq socket we use to connec to the endpoint.
			 */
			::zmq::socket_t socket;
		};
	}
}

#endif
