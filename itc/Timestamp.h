
#include "itc.h"

#include "BinStamp.h"

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
            : stamp_(r.stamp_),
            isInited_(r.isInited_) {
            r.stamp_ = nullptr;
            r.isInited_ = false;
        }


        /**
         * Restore a timestamp from encoded binary format stored in string
         */
        explicit Timestamp(const std::string& encoded)
            : isInited_(false),
            stamp_(nullptr) {
            BinStamp binStamp(encoded);
            buildFromBinStamp(std::move(binStamp));
        }

        explicit Timestamp(BinStamp&& binStamp)
            : isInited_(false),
            stamp_(nullptr) {
            buildFromBinStamp(std::move(binStamp));
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

    
    // TODO, need to create a serialiable version, suitable for thrift use

    private:

        void buildFromBinStamp(BinStamp&& binStamp) {
            if (binStamp.stamp_) {
                stamp_ = decodeStamp(binStamp.stamp_);
                isInited_ = true;
            }
        }

        // ITC based timestamp
        stamp* stamp_;

        // Track if we've been started as a seed, or the product of a fork, etc
        bool isInited_;

    // Make BinStamp our friend so it can serialize us
    friend class BinStamp;
};

}} // ::whisper::timestamp


