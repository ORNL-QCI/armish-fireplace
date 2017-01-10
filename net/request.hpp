#ifndef _NET_REQUEST_HPP
#define _NET_REQUEST_HPP

#include <common.hpp>
#include <utility>

namespace net {
	/**
	 * \brief A request message.
	 */
	struct request {
	 public:
		/**
		 * \brief Empty constructor.
		 */
		request()
				: _data{0} {
		}
		
		/**
		 * \brief Initialization constructor.
		 */
		request(const unsigned char protocol,
				const unsigned char action,
				const unsigned char flags)
				: request() {
			_data[0] = protocol;
			_data[1] = action;
			_data[2] = flags;
			
			// Reserved
			_data[3] = 0;
		}
		
		/**
		 * \brief Copy constructor.
		 */
		request(request& old) {
			memcpy(_data, old._data, sizeof(_data));
		}
		
		/**
		 * \brief Move constructor.
		 */
		request(request&& old) {
			memcpy(_data, old._data, sizeof(_data));
		}
		
		/**
		 * \brief Assignment operator.
		 */
		request& operator=(const request& old) {
			memcpy(_data, old._data, sizeof(_data));
			
			return *this;
		}
		
		/**
		 * \brief Move assignment operator.
		 */
		request& operator=(request&& old) {
			memcpy(_data, old._data, sizeof(_data));
			
			return *this;
		}
		
		/**
		 * \brief Return the number of bytes in the header.
		 */
		std::size_t header_length() const {
			return _header_length;
		}
		
		/**
		 * \brief Return the number of data bytes.
		 */
		std::size_t length() const {
			return _length;
		}
		
		/**
		 * \brief Access data, length given by length.
		 */
		inline char* data() {
			return _data;
		}
		
		/**
		 * 
		 */
		inline unsigned char protocol() const {
			return _data[0];
		}
		
		/**
		 * 
		 */
		inline unsigned char action() const {
			return _data[1];
		}
		
		/**
		 * 
		 */
		inline unsigned char flags() const {
			return _data[2];
		}
	
	 private:
		/**
		 * \brief The number of bytes of the header.
		 */
		static const std::size_t _header_length = 0;
		
		/**
		 * \brief The number of bytes of the data.
		 */
		static const std::size_t _length = 4;
	 
		/**
		 * \brief Data.
		 */
		char _data[_length];
	};
}

#endif
