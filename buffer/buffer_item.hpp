#ifndef _BUFFER_BUFFER_ITEM_HPP
#define _BUFFER_BUFFER_ITEM_HPP

#include <common.hpp>

namespace buffer {
		/**
		 * \brief An array of parameters.
		 */
		struct parameters {
		 public:
			/**
			 * \brief Constructor.
			 */
			parameters(const char* const*&& parameterList, const std::size_t size, bool isAllocated = false)
					: _parameters(parameterList), _size(size), _isAllocated(isAllocated) {
			}
			
			/**
			 * \brief Copy constructor is disabled.
			 */
			parameters(const parameters&) = delete;
			
			/**
			 * \brief Move constructor.
			 */
			parameters(parameters&& item) {
				_parameters = item._parameters;
				item._parameters = 0;
				_size = item._size;
				item._size = 0;
				_isAllocated = item._isAllocated;
				item._isAllocated = false;
			}
			
			/**
			 * \brief Assignment operator is disabled.
			 */
			parameters& operator=(const parameters&) = delete;
			
			/**
			 * \brief Move assignment operator.
			 */
			parameters& operator=(parameters&& item) {
				_parameters = item._parameters;
				item._parameters = 0;
				_size = item._size;
				item._size = 0;
				_isAllocated = item._isAllocated;
				item._isAllocated = false;
				return *this;
			}
			
			/**
			 * \brief Destructor.
			 */
			~parameters() {
				if(_isAllocated && _parameters != 0) {
					for(std::size_t i = 0; i < _size; i++) {
						delete[] _parameters[i];
					}
					delete[] _parameters;
				}
			}
			
			/**
			 * \brief Return a parameter string by index.
			 */
			inline const char* operator[](const std::size_t index) const {
				assert(index < _size);
				
				return _parameters[index];
			}
			
			/**
			 * \brief Return the number of parameter strings.
			 */
			inline std::size_t size() const {
				return _size;
			}
			
		 private:
			/**
			 * \brief \todo
			 */
			const char* const* _parameters;
		
			/**
			 * \brief \todo
			 */
			std::size_t _size;
			
			/**
			 * \brief \todo
			 */
			bool _isAllocated;
		};
	
	/**
	 * \brief An item in the buffer.
	 */
	class buffer_item {
	 public:
		/**
		 * \brief Default initialization constructor.
		 */
		buffer_item(const char* const data,
				const std::size_t size,
				const char* const*&& parameterList,
				const std::size_t count,
				const bool isAllocated);
		
		/**
		 * \brief Copy constructor is disabled.
		 */
		buffer_item(const buffer_item&) = delete;
		
		/**
		 * \brief Move constructor.
		 */
		buffer_item(buffer_item&& item);
		
		/**
		 * \brief Assignment operator is disabled.
		 */
		buffer_item& operator=(const buffer_item&) = delete;
		
		/**
		 * \brief Move assignment operator.
		 */
		buffer_item& operator=(buffer_item&& item);
		
		/**
		 * \brief Destructor.
		 * 
		 * Deletes the allocated memory for data.
		 */
		~buffer_item();
		
		/**
		 * \brief Return pointer to data.
		 */
		const char* data() const;
		
		/**
		 * \brief Return number of bytes of data.
		 */
		std::size_t size() const;
		
		/**
		 * \brief Return a reference to the item parameters.
		 */
		const ::buffer::parameters& parameters() const;
	
	 private:
		/**
		 * \brief \todo
		 */
		char* _data;
		
		/**
		 * \brief \todo
		 */
		std::size_t _size;
		
		/**
		 * \brief \todo
		 */
		::buffer::parameters _parameters;
	};
}

#endif
