/*===============================*
 * Author/s:
 *  - silentrald
 * Version: 1.0
 * Created: 2023-04-15
 *===============================*/

#ifndef DS_THREAD_THREAD_HPP
#define DS_THREAD_THREAD_HPP

#include <pthread.h>

namespace ds {

class thread {
private:
  pthread_t _thread{};

public:
};

} // namespace ds

#endif

