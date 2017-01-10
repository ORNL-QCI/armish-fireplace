#include "client.hpp"

namespace net {
	namespace simulation {
		client::client(const char* const endpoint)
				: endpoint{'\0'},
				socket(::net::global_zcontext, ZMQ_REQ) {
			const auto endpointLen = strlen(endpoint)+sizeof(endpoint[0]);
			
			if(UNLIKELY(endpointLen > sizeof(this->endpoint)*sizeof(this->endpoint[0]))) {
				throw std::invalid_argument(err_msg::_arybnds);
			}
			
			memcpy(this->endpoint, endpoint, endpointLen);
			socket.connect(this->endpoint);
		}
		
		client::client(client&& old)
				: socket(std::move(old.socket)) {
			memcpy(endpoint, old.endpoint, sizeof(endpoint)*sizeof(endpoint[0]));
		}
		
		client& client::operator=(client&& old) {
			memcpy(endpoint, old.endpoint, sizeof(endpoint)*sizeof(endpoint[0]));
			socket = std::move(old.socket);
			
			return *this;
		}
		
		client::~client() {
			socket.disconnect(endpoint);
			socket.close();
		}
		
		response client::call(request& request) {
			request.generate_json();
			
			// Zero-copy idiom
			::zmq::message_t zrequest((void*)request.get_json(),
					request.get_json_str_size(),
					0,
					0);
			bool rc = socket.send(zrequest);
			
			if(UNLIKELY(!rc)) {
				throw std::runtime_error(err_msg::_ntwrkdn);
			}
			
			::zmq::message_t zreply;
			rc = socket.recv(&zreply);
			
			if(UNLIKELY(!rc)) {
				throw std::runtime_error(err_msg::_ntwrkdn);
			}
			
			return response((char*)zreply.data(), zreply.size());
		}
	}
}
