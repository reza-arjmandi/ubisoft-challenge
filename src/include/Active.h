#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <functional>
#include <atomic>

class Active 
{

public:

  using MessageType = std::function<void()>;

  Active() 
  {
    thd = std::thread([&]() 
  	{
	  while (!isStopped)
	  {
		MessageType work;
		if (msgQueue.size() == 0)
		{
		  std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
  		else
  		{
  		  MessageType work;
		  {
			std::lock_guard<std::mutex> lock{ mtx };
			work = msgQueue.front();
		  }
		  work();
		  {
			std::lock_guard<std::mutex> lock{ mtx };
			msgQueue.pop();
		  }
		}
	  }
	});
  }

  ~Active() {
    stop();
  }

  void send(MessageType m) 
  {
    std::lock_guard<std::mutex> lock{ mtx };
  	msgQueue.push(m);
  }

  void stop() 
  {
  	if (isStopped)
  	{
  		return;
  	}
  	msgQueue.push([&]() 
  		{
  		isStopped = true;
  		}
  	);
  	thd.detach();
  }

private:

  std::queue<MessageType> msgQueue;
  std::atomic<bool> isStopped{false};
  std::mutex mtx;
  std::thread thd;

};
