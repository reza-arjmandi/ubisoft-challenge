#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <functional>
#include <iostream>
#include <atomic>

class Active 
{

public:

	using MessageType = std::function<void()>;

	Active() 
	{
		_thd = std::thread([&]() 
			{

				while (!_is_stopped)
				{
					MessageType work;
					if (_msg_queue.size() == 0)
					{
						std::this_thread::sleep_for(
							std::chrono::milliseconds(100));
					}
					else
					{
						MessageType work;
                        {
							std::lock_guard<std::mutex> lock{ _mutex };
                            work = _msg_queue.front();
                        }
						work();
						{
							std::lock_guard<std::mutex> lock{ _mutex };
							_msg_queue.pop();
                        }
					}
				}
			}
		);
	}

	~Active() {
		stop();
	}

	void send(MessageType m) 
	{
		std::lock_guard<std::mutex> lock{ _mutex };
		_msg_queue.push(m);
	}

	void stop() 
	{
		if (_is_stopped)
		{
			return;
		}

		_msg_queue.push([&]() 
			{
			_is_stopped = true;
			}
		);
		_thd.detach();
	}

private:

	std::queue<MessageType> _msg_queue;
	std::atomic<bool> _is_stopped{false};
	std::mutex _mutex;
	std::thread _thd;

};
