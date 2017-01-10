#include "iproc_unit.hpp"

namespace module {
	iproc_unit::iproc_unit() {
	}
	
	iproc_unit::~iproc_unit() {
	}
	
	iproc_unit* iproc_unit::initialize() {
		throw std::runtime_error(err_msg::_stcimpl);
	}
}
