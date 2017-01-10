#ifndef _NET_ZCONTEXT_HPP
#define _NET_ZCONTEXT_HPP

#include <common.hpp>
#include <cppzmq/zmq.hpp>

namespace net {
	/**
	 * \brief This is an applicationw-wide instance of a zmq context object.
	 * 
	 * If something requires a zmq socket, initialize it with this.
	 */
	extern ::zmq::context_t global_zcontext;
}

#endif
