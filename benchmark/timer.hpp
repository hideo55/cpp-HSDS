#if !defined(TIMER_HPP_)
#define TIMER_HPP_

#include <sys/time.h>

class Timer {
public:
    Timer() :
            base_(get_time()) {
    }

    double elapsed() const {
        return get_time() - base_;
    }

    void reset() {
        base_ = get_time();
    }

private:
    double base_;

    static double get_time() {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        return tv.tv_sec + (tv.tv_usec * 0.000001);
    }

    // Disallows copy and assignment.
    Timer(const Timer &);
    Timer &operator=(const Timer &);
};

#endif /* TIMER_HPP_ */
