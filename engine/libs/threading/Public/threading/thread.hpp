#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>


namespace threading
{
    using ID = std::thread::id;

	ID GetThreadID();
}

#endif //!THREAD_HPP
