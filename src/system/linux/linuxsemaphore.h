#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <semaphore.h>
#include <errno.h>

class Semaphore  {
 public:
  Semaphore(unsigned int initialCount = 0)  {
      sem_init(&m_sema, 0, initialCount);
  }

  ~Semaphore()  {
      sem_destroy(&m_sema);
  }

  void wait()  {
      // http://stackoverflow.com/questions/2013181/gdb-causes-sem-wait-to-fail-with-eintr-error
      int rc;
      do  {
          rc = sem_wait(&m_sema);
      }  while (rc == -1  &&  errno == EINTR);
  }

  void signal()  {
      sem_post(&m_sema);
  }

  void signal(unsigned int  count)  {
    while (count--) {
      sem_post(&m_sema);
    }
  }

 private:
    sem_t m_sema;
    Semaphore(const Semaphore& other) = delete;
    Semaphore& operator=(const Semaphore& other) = delete;
};

#endif // SEMAPHORE_H
