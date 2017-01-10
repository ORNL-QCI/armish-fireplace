#ifndef _BUFFER_QUEUE_BUFFER_HPP
#define _BUFFER_QUEUE_BUFFER_HPP

#include <common.hpp>
#include "buffer_item.hpp"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace buffer {
	/**
	 * \brief A threadsafe FIFO buffer.
	 */
	class queue_buffer {
	 private:
		typedef	std::unique_lock<std::mutex> lock_t;
	
	 public:
		/**
		 * \brief Constructor.
		 */
		queue_buffer();
		
		/**
		 * \brief Copy constructor is disabled.
		 * 
		 * It is disabled implicitly because of the mutex member, so we make this deletion
		 * obvious.
		 */
		queue_buffer(const queue_buffer&) = delete;
		
		/**
		 * \brief Move constructor is disabled.
		 * 
		 * It is disabled implicitly because of the mutex member, so we make this deletion
		 * obvious.
		 */
		queue_buffer(queue_buffer&&) = delete;
		
		/**
		 * \brief Assignment operator is disabled.
		 * 
		 * It is disabled implicitly because of the mutex member, so we make this deletion
		 * obvious.
		 */
		queue_buffer& operator=(const queue_buffer&) = delete; 
		
		/**
		 * \brief Move assignment operator is disabled.
		 * 
		 * It is disabled implicitly because of the mutex member, so we make this deletion
		 * obvious.
		 */
		queue_buffer& operator=(queue_buffer&&) = delete;
		
		/**
		 * \brief Set the threshold when a call to push_wait_for will be successful.
		 * 
		 * \note Threadsafe.
		 */
		void set_push_wait_threshold(const std::size_t threshold);
		
		/**
		 * \brief Return the number of items in the queue.
		 *
		 * \note Threadsafe.
		 */
		std::size_t size();
		
		/**
		 * \brief Push an item into the queue.
		 *
		 * \note Threadsafe.
		 */
		void push(buffer_item&& item);
		
		/**
		 * \brief Pop all the items current within the queue and return them in a queue of
		 * their own.
		 * 
		 * Safe to call when the queue is empty, as it simply returns an empty queue.
		 *
		 * \note Threadsafe
		 */
		std::queue<buffer_item> pop_all();
		
		/**
		 * \brief Block up to a specified amount of time waiting for an item to be pushed
		 * into the queue.
		 * 
		 * If an item is pushed into the queue before the specified amount of time then we
		 * return, otherwise we return after the specified amount of time as a timeout.
		 * 
		 * \note Threadsafe.
		 * 
		 * \returns Whether or not an item was pushed into the queue during our waiting.
		 */
		bool push_wait(const std::size_t milliseconds);
	
	 private:
		/**
		 * \brief \todo
		 */
		std::queue<buffer_item> queue;
		
		/**
		 * \brief \todo
		 */
		std::mutex queueMutex;
		
		/**
		 * \brief \todo
		 */
		std::condition_variable pushCV;
		
		/**
		 * \brief \todo
		 */
		std::condition_variable pushWaitCV;
		
		/**
		 * \brief \todo
		 */
		bool pushWaitEnabled;
		
		/**
		 * \brief \todo
		 */
		std::size_t pushWaitNew;
		
		/**
		 * \brief \todo
		 */
		std::size_t pushWaitThreshold;
	};
}

#endif
