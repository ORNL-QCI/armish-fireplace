#ifndef _MODULE_BRAZIL_PROC_UNIT_BOBWIRE_CIRCUIT_HPP
#define _MODULE_BRAZIL_PROC_UNIT_BOBWIRE_CIRCUIT_HPP

#include <common.hpp>
#include "../itrx_proc_unit.hpp"
#include "../../../net/simulation/client.hpp"
#include <stdlib.h>
#include <fstream>
#include <cppzmq/zmq.hpp>

#define BOBWIRE_CIRCUIT_RX_RECEIVE_TIMEOUT 250 // milliseconds

namespace module {
	namespace brazil {
		namespace proc_unit {
			/**
			 * \brief Virtualized transceiver hardware to trx circuit.
			 */
			class bobwire_circuit : public ::module::brazil::itrx_proc_unit<bobwire_circuit> {
			 public:
				/**
				 * \brief Constructor.
				 */
				bobwire_circuit();
				
				/**
				 * \brief Destructor.
				 */
				~bobwire_circuit();
				
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
				
				/**
				 * \brief File of bases to measure in.
				 */
				std::ifstream basesFile;
				
				char basisByte;
				size_t basisBytePos;
				
				char get_next_basis() {
					if(basisBytePos == 7) {
					 bfread:
						// Read latest byte
						if(!basesFile.get(basisByte)) {
							// If EOF, wrap around to beginning of file
							if(basesFile.eof()) {
								basesFile.seekg(0, basesFile.beg);
							} else {
								throw std::runtime_error("IO ERROR");
							}
							goto bfread;
						}
					}
					
					basisBytePos++;
					basisByte >>= 1;
					return basisByte & 1;
				}
				
				
				//
				// This could be moved back into the base class, but this is
				// just a proof of concept.
				//
				
				using bw_requestServer_t = ::net::server<bobwire_circuit,
					::net::request,
					::net::response>;
				
				inline void start_request_listening() {
					requestThread = std::thread(&bobwire_circuit::work, this);
				}
				
				inline void work() {
					// Protect until we have launched
					std::unique_lock<std::mutex> uLock(requestMutex);
					bw_requestServer_t requestServer(
							ioService,
							NTH_BYTE_ORD(netInfo.sin_addr.s_addr),
							NTH_BYTE_ORD(netInfo.sin_port),
							*this,
							&bobwire_circuit::process);
					
					// Unlock while we block below
					uLock.unlock();
					
					// Block until ioService.stop() is called
					boost::asio::io_service::work work(ioService);
					ioService.run();
				}
				
				inline void process(::net::request& incomingMessage,
						::net::response& outgoingMessage)  {
					UNUSED(incomingMessage);
					UNUSED(outgoingMessage);
					
					ulock_t  uLock(requestMutex);
					
					if(isReceiving) {
						// Wait for the simulated result
						hasReceivedCV.wait(uLock, [this] { return hasReceived == true; });
						
						// uLock still locked
						hasReceived = false;
						
						// todo: send proper outgoing
					} else {
						// todo: send proper outgoing
						std::string basisChange;
						if(get_next_basis() == 1) {
							basisChange += "h 0\nh 1\n";
						}
						basisChange += "m 0\nm 1\n";
						::net::simulation::request rqst("configure_node", false);
						rqst.add<unsigned int>(nIP)
								.add<const char*, false>("receiver")
								.add<const char*, false>("chpext")
								.add<const char*, false>(basisChange.c_str())
								.add<const char*, false>("\n");
						dispatcher->call(rqst);
					}
					isReceiving = !isReceiving;
				}
			};
		}
	}
}

#endif
