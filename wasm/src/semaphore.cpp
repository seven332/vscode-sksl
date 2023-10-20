#include <semaphore.h>

#include <cerrno>

extern "C" {

// fake semaphore

int sem_init(sem_t* sem, int /*pshared*/, unsigned value) {
    static_assert(sizeof(sem_t) >= sizeof(unsigned), "sem_t < unsigned");
    *reinterpret_cast<unsigned*>(sem) = value;
    return 0;
}

int sem_post(sem_t* sem) {
    *reinterpret_cast<unsigned*>(sem) += 1;
    return 0;
}

int sem_wait(sem_t* sem) {
    auto* ptr = reinterpret_cast<unsigned*>(sem);
    if (*ptr > 0) {
        *ptr -= 1;
        return 0;
    } else {
        errno = EINTR;
        return -1;
    }
}
}
