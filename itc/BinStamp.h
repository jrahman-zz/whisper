
#include "itc.h"

#include <arpa/inet.h>

#include <string>
#include <memory>

namespace whisper { namespace timestamp {

class BinStamp {

    public:

        /**
         * Ban the default constructor
         */
        BinStamp() = delete;

        /**
         * Build a binary stamp from a string representation
         */
        explicit BinStamp(const std::string& str) {
            char *data_ = char[str.size()];
            memcpy(data, str.c_str(), str.size());

            uint32_t idLength = ntohl(*(uint32_t*)data);
            uint32_t eventLength = ntohl(*(uint32_t)&data[idLength + sizeof(idLength));

            stamp_.idsize = idLength;
            stamp_.eventsize = eventLength;
            stamp_.bid = &data[sizeof(idLength)];
            stamp_.bevent = &data[sizeof(idLength) + idLength + sizeof(eventLength)];
        }

        /**
         * Convert a timestamp into binary format
         */
        explicit BinStamp(const Timestamp& timestamp)
            : data_(nullptr) {
            
        }

        /**
         * Move constructor for BinStamps
         */
        BinStamp(BinStamp&& r) 
            : stamp_(r.stamp_),
            data_(r.data_) {
            r.data_ = nullptr;
            r.stamp_.bid = nullptr;
            r.stamp_.bevent = nullptr;
        }

        ~BinStamp() {
            if (data_) {
                delete data_;
                data_ = nullptr;
            }

            stamp_.bid = nullptr;
            stamp_.bevent = nullptr;
        }

        /**
         * Serialize the given binary stamp
         */
        std::string serialize() {
            if (!data_) {
                // TODO, badness
                return std::string("");
            }

            int length = stamp_.idsize + stamp_.eventsize
                + sizeof(stamp_.idsize) + sizeof(stamp_.eventsize);

        }

    private:

        // Binary stamp for later use
        binStamp stamp_;

        // Array within which we store our data for the binStamp
        char* data_;

    // Make Timestamp a friend so it can deserialize from us
    friend class Timestamp;
}

}} // ::whisper::timestamp
