#include "global_zcontext.hpp"

namespace net {
	::zmq::context_t global_zcontext = ::zmq::context_t(1);
}
