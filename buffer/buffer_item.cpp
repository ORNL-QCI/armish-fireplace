#include "buffer_item.hpp"

namespace buffer {
	buffer_item::buffer_item(const char* const data,
			const std::size_t size,
			const char* const*&& parameterList,
			const std::size_t count,
			const bool isAllocated)
			: _size(size),
			_parameters(std::move(parameterList), count, isAllocated) {
		if(data != 0 && size != 0) {
			_data = new char[_size];
			memcpy(_data, data, _size);
		} else {
			_data = 0;
		}
	}
	
	buffer_item::buffer_item(buffer_item&& item)
			: _parameters(std::move(item._parameters)) {
		_data = item._data;
		item._data = 0;
		_size = item._size;
		item._size = 0;
	}
	
	buffer_item& buffer_item::operator=(buffer_item&& item) {
		_data = item._data;
		item._data = 0;
		_size = item._size;
		item._size = 0;
		_parameters = std::move(item._parameters);
		return *this;
	}
	
	buffer_item::~buffer_item() {
		if(_data != 0) {
			delete[] _data;
		}
	}
	
	const char* buffer_item::data() const {
		return _data;
	}
	
	std::size_t buffer_item::size() const {
		return _size;
	}
	
	const parameters& buffer_item::parameters() const {
		return _parameters;
	}
}
