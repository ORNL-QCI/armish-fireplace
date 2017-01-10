#ifndef _NET_RESPONSE_HPP
#define _NET_RESPONSE_HPP

#include <common.hpp>

namespace net {
	/**
	 * \brief \todo
	 */
	enum class response_status_code {
		EMPTY,
		OK,
		NO,
		PROBLEM,
	};
	
	/**
	 * \brief A response message.
	 */
	class response {
	 public:
		/**
		 * \brief Empty constructor.
		 */
		response()
				: _data{0} {
		}
		
		/**
		 * \brief Initialization constructor.
		 */
		response(const response_status_code status)
				: response() {
			set_status(status);
		}
		
		constexpr static std::size_t header_length() {
			return _header_length;
		}
		
		/**
		 * \brief Return the number of bytes of the data.
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
		 * \brief Set the length of the data.
		 */
		inline void allocate(const std::size_t length) {
			UNUSED(length);
		}
		
		/**
		 * \brief \todo
		 */
		inline void set_status(const response_status_code status) {
			UNUSED(status);
			//auto temp((std::uint_fast32_t)status);
			/** \todo: fix! */
			//_data[1] = ((char*)(&temp))[0];
			
		}
	
	 private:
		/**
		 * \brief Number of bytes in data.
		 */
		static const std::size_t _length = 4;
		
		/**
		 * \brief Number of bytes of the header.
		 */
		static constexpr const std::size_t _header_length = 0;
		
		/**
		 * \brief Data.
		 * 
		 * Static length given by length.
		 */
		char _data[_length];
	};
}

#endif
