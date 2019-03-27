#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>


class Thread final
{
public:
    using ID = std::thread::id;
    
	static ThreadID GetID();
};


#endif //!THREAD_HPP
