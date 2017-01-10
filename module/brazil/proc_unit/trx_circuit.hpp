#ifndef _MODULE_BRAZIL_PROC_UNIT_TRX_CIRCUIT_HPP
#define _MODULE_BRAZIL_PROC_UNIT_TRX_CIRCUIT_HPP

#include <common.hpp>
#include "../itrx_proc_unit.hpp"
#include "../../../net/simulation/client.hpp"
#include <stdlib.h>
#include <cppzmq/zmq.hpp>

#define TRX_CIRCUIT_LANGUAGE "chpext"
#define TRX_CIRCUIT_NEWLINE_DELIMITER "\n"
#define TRX_CIRCUIT_MEASURE "c 0,1\nh 0\nm 0\nm 1"
#define TRX_CIRCUIT_RX_RECEIVE_TIMEOUT 250 // milliseconds

namespace module {
	namespace brazil {
		namespace proc_unit {
			/**
			 * \brief Virtualized transceiver hardware to trx circuit.
			 */
			class trx_circuit : public ::module::brazil::itrx_proc_unit<trx_circuit> {
			 public:
				/**
				 * \brief Constructor.
				 */
				trx_circuit();
				
				/**
				 * \brief Destructor.
				 */
				~trx_circuit();
				
				/**
				 * \brief Return a new instance of the class.
				 */
				static ::module::iproc_unit* initialize();
				
				/**
				 * \brief Initialize the transceiver with a parameter string.
				 */
				void string_initialize_parameters(const char* const parameters);
				
				/**
				 * \brief This processing unit pushes incoming messages from the hardware
				 * into the buffer for the software layer.
				 * 
				 * \note Threadsafe.
				 */
				void async_work(::buffer::queue_buffer& out);
				
				/**
				 * \brief Process a request action.
				 * 
				 * \note Threadsafe.
				 */
				::module::iproc_unit::response* proc_act_request(
						const ::module::iproc_unit::request& request);
			
			 protected:
				/**
				 * \brief
				 * 
				 * \note Threadsafe.
				 */
				bool transmit(const unsigned long ip,
						const unsigned short port,
						const char* const buf,
						const std::size_t len);
			
			 private:
				/**
				 * \brief Connection to network dispatcher for tx.
				 */
				::net::simulation::client* dispatcher;
				
				/**
				 * \brief Connection to network dispatcher for rx.
				 */
				::zmq::socket_t socket;
				
				/**
				 * \brief A class-local cache of the IP address from itrx_proc_unit.
				 * 
				 * Since we only set the original value once, we cache it here to avoid
				 * locking a mutex every time we need the value, which happens each time
				 * we transmit and receive.
				 */
				unsigned int nIP;
				
				/**
				 * \brief Host byte order version of NIP.
				 */
				unsigned int nIP_hbo;
			};
		}
	}
}

#endif
