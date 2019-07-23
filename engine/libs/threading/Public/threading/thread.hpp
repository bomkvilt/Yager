#ifndef THREADING_THREAD_HPP
#define THREADING_THREAD_HPP

#include <thread>


namespace threading
{
    using ID = std::thread::id;

	ID GetThreadID();
}

#endif //!THREADING_THREAD_HPP
