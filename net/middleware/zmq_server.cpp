#include "zmq_server.hpp"

namespace net {
	namespace middleware {
		const int zmq_server::sync_send_to;
		const int zmq_server::sync_receive_to;
		const int zmq_server::async_send_to;
		const int zmq_server::async_receive_to;
		const int zmq_server::async_wait_to;
		const int zmq_server::async_wait_count;
		const int zmq_server::async_wait_fail;
		
		zmq_server::zmq_server(::module::module_manager& moduleManager)
				: server(moduleManager) {
		}
		
		zmq_server::~zmq_server() {
		}
		
		void zmq_server::sync_work() {
			try {
				zmq::socket_t socket(::net::global_zcontext, ZMQ_PAIR);
				socket.setsockopt(ZMQ_SNDTIMEO, &sync_send_to, sizeof(sync_send_to));
				socket.setsockopt(ZMQ_RCVTIMEO, &sync_receive_to, sizeof(sync_receive_to));
				socket.bind(get_iEndpoint());
				
				notify_thread_started();
				
				while(!do_exit()) {
					zmq::message_t rcvMsg;
					// Block until we receive a message or timeout
					if(socket.recv(&rcvMsg)) {
						#ifdef THROW
						// Check that we can do insitu parsing
						if(static_cast<char*>(rcvMsg.data())[rcvMsg.size()-1] != '\0') {
							throw std::runtime_error(err_msg::_malinpt);
						}
						#endif
						
						const request rqst(rcvMsg.data());
						response* rspns = NULL;
						
						switch(rqst.action()) {
						 case ::actions::actions_t::REQUEST:
							/**  \todo Make this catch more specific. */
							try {
								rspns = module_manager().proc_act_request(rqst);
							} catch(const std::exception& e) {
								rspns = new response(e.what(), true);
							}
							break;
						
						 case ::actions::actions_t::PUSH:
							/**  \todo Make this catch more specific. */
							try {
								rspns = new response(module_manager().proc_act_push(rqst));
							} catch(const std::exception& e) {
								rspns = new response(e.what(), true);
							}
							break;
						
						 case ::actions::actions_t::WAIT:
						 case ::actions::actions_t::REPLY:
							#ifdef THROW
							throw std::runtime_error(err_msg::_malinpt);
							#endif
							
							continue;
						}
						
						#ifdef THROW
						if(UNLIKELY(rspns == NULL)) {
							throw std::runtime_error(err_msg::_nllpntr);
						}
						#endif
						
						// This conforms to the requirement imposed by zmq::message_t
						// zero-copy idiom that passes a pointer to the data along
						// with a hint object. Because our data is within the hint
						// object, we just deallocate the hint object, which is our
						// case is a response object. The use of the idiom ensures we
						// do not copy the data of a request in zmq and rather we tell
						// zmq the buffer is safe to use until the message is sent.
						// This function is then called automatically to delete the
						// request object.
						auto voidHelper = static_cast<const void*>(rspns->json());
						socket.send(::zmq::message_t(const_cast<void*>(voidHelper),
								rspns->size(),
								// Capture nothing
								[] (void* data, void* hint) {
									UNUSED(data);
									delete static_cast<response*>(hint);
								},
								rspns));
					}
				}
				
				socket.unbind(get_iEndpoint());
				socket.close();
			} catch(const zmq::error_t& e) {
				std::cerr << e.what() << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		
		void zmq_server::async_work() {
			try {
				zmq::socket_t socket(::net::global_zcontext, ZMQ_PAIR);
				socket.setsockopt(ZMQ_SNDTIMEO, &async_send_to, sizeof(async_send_to));
				socket.setsockopt(ZMQ_SNDTIMEO, &async_send_to, sizeof(async_receive_to));
				socket.bind(get_oEndpoint());
				
				auto& asyncBuffer = module_async_buffer();
				
				asyncBuffer.set_push_wait_threshold(async_wait_count);
				std::size_t failCount = 0;
				
				notify_thread_started();
				
				while(!do_exit()) {
					if(asyncBuffer.push_wait(async_wait_to)
							|| ++failCount == async_wait_fail) {
						failCount = 0;
						
						auto&& localBuffer = asyncBuffer.pop_all();
						
						while(localBuffer.size() != 0) {
							using namespace ::buffer;
							
							// This is to get around an oversight in the C++11 standard
							// where our std::queue cannot get an item from the queue by
							// move, only by reference. This is safe because this we hold
							// the only copy of this queue so we know nothing will change
							// between front() and pop().
							auto& tempRef = const_cast<buffer_item&>(localBuffer.front());
							auto rspns = new buffer_item(std::move(tempRef));
							localBuffer.pop();
							
							// This conforms to the requirement imposed by zmq::message_t
							// zero-copy idiom that passes a pointer to the data along
							// with a hint object. Because our data is within the hint
							// object, we just deallocate the hint object, which is our
							// case is a response object. The use of the idiom ensures we
							// do not copy the data of a request in zmq and rather we tell
							// zmq the buffer is safe to use until the message is sent.
							// This function is then called automatically to delete the
							// request object.
							auto voidHelper = static_cast<const void*>(rspns->data());
							socket.send(::zmq::message_t(const_cast<void*>(voidHelper),
									rspns->size(),
									// Capture nothing
									[] (void* data, void* hint) {
										UNUSED(data);
										delete static_cast<buffer_item*>(hint);
									},
									rspns));
						
						}
					}
				}
				
				socket.unbind(get_oEndpoint());
				socket.close();
			} catch(const zmq::error_t& e) {
				std::cerr << e.what() << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}
