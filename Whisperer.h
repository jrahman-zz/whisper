


#include <thrift/lib/cpp/async/HHWheelTimer.h>

namespace whisper {


typedef function<void(GossipNode)> NodeDeadCob;
typedef function<void(GossipNode)> NodeDiscoveredCob;

class Whisperer {

    public:

        Whisperer() 
            : gossipInterval(50),
            updateTimeout(4000),

        // Disallow copies
        Whisperer(const Whisperer& other) = delete;

    
        void setNodeDeadCallback(NodeDeadCob cob);

        void setNodeDeadCallback(cob);


    private:

        // Determines the number of milliseconds between outbound gossip messages
        int gossipInterval_;

        // Timeout in ms for when sending an update to another node before
        // we mark that send operation as failed
        int updateTimeout_;

};

} // ::whisper
