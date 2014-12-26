
#include "itc.h"

namespace whisper { namespace timestamp {

/**
 * Wrapper class around the C interval-tree-clock API
 * We use destructive updates by default when performing
 * various operations against the underlying ITC stamp object
 * To avoid such destruction, you must perform a copy first
 * although the utility of such an operation is questionable
 */
class Timestamp {

    public:
    
        /**
         * Default construct the timestamp, note that we do not make it a seed yet
         */
        Timestamp()
            : stamp_(nullptr),
            isInited_(false) {
        }

        /**
         * Move constructor
         */
        Timestamp(const Timestamp&& r)
            : stamp_(r.stamp_) {
            r.stamp_ = nullptr;
        }


        ~Timestamp() {
            if (stamp_) {
                free(stamp_);
            }
        }

        bool operator<(const Timestamp& rhs) {
            // TODO, assert when badness happens
            return itc_leq(stamp_, rhs.stamp_) && !(rhs <= *this); 
        }

        bool operator<=(const Timestamp& rhs) {
            // TODO, assert when badness happens
            return itc_leq(stamp_, rhs.stamp_);
        }

        Timestamp fork() {
            if (UNLIKELY(!isInited_)) {
                itc_seed(stamp);
                isInited_ = true;
            }

            Timestamp forkedTimestamp;

            // Allocate empty stamp for the new child
            forkedTimestamp.stamp_ = newStamp();

            itc_fork(stamp_, stamp_, forkedTimestamp.stamp_);
            forkedTimestamp.isInited = true;
            return forkedTimestamp;
        }

        /**
         * Modify the current timestamp by joining with another Timestamp
         */
        void join(Timestamp other) {
            if (!other.isInited_) {
                // TODO, BADNESS
                ;
            }

            itc_join(stamp_, stamp_, other.stamp_);
            isInited_ = true;
        }

        /**
         * Peek at the current timestamp by creating a new timestamp with no ID
         */
        Timestamp peek() {
            if (UNLIKELY(!isInited_)) {
                itc_seed(stamp_);
                isInited_ = true;
            }

            Timestamp peekedTimestamp;
            itc_peek(stamp_, peekedTimestamp.stamp_);
            peekedTimestamp.isInited_ = true;
            return peekedTimestamp;
        }

        /**
         * Apply an event to the interval-tree-clock, modifying this clock in the process
         */
        void event() {
            if (UNLIKELY(!isInited_)) {
                itc_seed(stamp_);
                isInited_ = true;
            }

            itc_event(stamp_, stamp_);
        }

    private:

        // ITC based timestamp
        stamp* stamp_;

        // Track if we've been started as a seed, or the product of a fork, etc
        bool isInited_;
}

}} // ::whisper::timestamp


