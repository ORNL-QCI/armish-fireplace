#include "queue_buffer.hpp"

namespace buffer {
	queue_buffer::queue_buffer()
				: pushWaitEnabled(false),
				pushWaitNew(0),
				pushWaitThreshold(0) {
		}
	
	void queue_buffer::set_push_wait_threshold(const std::size_t threshold) {
		lock_t lock(queueMutex);
		
		pushWaitEnabled = true;
		pushWaitThreshold = threshold;
	}
	
	std::size_t queue_buffer::size() {
		lock_t lock(queueMutex);
		
		return queue.size();
	}
	
	void queue_buffer::push(buffer_item&& item) {
		lock_t lock(queueMutex);
		
		queue.push(std::move(item));
		
		if(pushWaitEnabled && ++pushWaitNew >= pushWaitThreshold) {
			pushWaitCV.notify_all();
		}
	}
	
	std::queue<buffer_item> queue_buffer::pop_all() {
		lock_t lock(queueMutex);
		
		std::queue<buffer_item> returnQueue;
		std::swap(queue, returnQueue);
		
		return returnQueue;
	}
	
	bool queue_buffer::push_wait(const std::size_t milliseconds) {			
		lock_t lock(queueMutex);
		
		if(queue.size() >= pushWaitThreshold ||
				pushCV.wait_for(lock, std::chrono::milliseconds(milliseconds), [this] {
					return pushWaitNew >= pushWaitThreshold;
				})) {
			pushWaitNew = 0;
			return true;
		}
		
		return false;
	}
}
