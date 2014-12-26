
#include "Gossiper.h"
#include "GossipNode.h"

#include <thrift/lib/cpp/async/HHWheelTimer.h>

using namespace apache::thrift::async

namespace whisper {
    
class Gossiper {
    
    public:
   

    void async_tm_sendUpdate(
            std::unique_ptr<apache::thrift::HandlerCallback<GossipState>> callback,
            GossipState state) {

        auto iface = state.sender.iface;
        LOG(INFO) << "Received update from " << iface.addr_ << ":" << std::to_string(iface.port_);

        // TODO, merge into our state, using logical clocks on individual elements to resolve conflicts
        
    }


    private:
    /**
     * Callback timer to send an update to a given Gossip node
    */
    class UpdateCallback : public HHWheelTimer::Callback {
        public:

            UpdateCallback(shared_ptr<GossipNode> target, GossipState* state, TEventBase* ev) 
                : target_(target),
                state_(state),
                ev_(ev) { }

            // Send 
            void timeoutExpired() override {
                // TODO Send RPC
                
                auto client = getClient(ev_, target->iface.addr_, target->iface.port_);
                

                // TODO, and locking here
                folly::wangle::future<GossipState> f = client->future_sendUpdate(*state_);

                // Handle the response
                f.then(
                    [](Try<GossipState>&& state) {
                        if (state.hasException()) {
                            // We couldn't talk to the given GossipNode
                            // TODO, hook back into Gossiper and tell it that we
                            // failed to speak to the given node
                        } else {
                            // Get GossipState and run
                            // TODO, hook back into Gossiper and tell it that we
                            // got a Gossip State back
                        }
                    }
                );
                
            }


        protected:

            // Create an asynchronous client to send our update to the remote node
            GossipAsyncClient* getClient(TEventBase* ev, GossipNodeInterface iface) {
                
                std::shared_ptr<TAsyncSocket> socket(
                        TAsyncSocket::newSocket(ev, iface.addr_, iface.port_));

                auto clientChannel = HeaderClientChannel::newChannel(socket);

                return new GossipAsyncClient(std::move(clientChannel));
            }

        private:

            // Record which GossipNode we are targeting here
            shared_ptr<GossipNode> target_;

            // Hold on to a pointer to the GossipState so that we can serialize
            // it out when we need to send it
            // TODO, determine locking needs since ThriftServer is multi-threaded
            GossipState* state_;

            // Hold onto the eventbase
            TEventBase* ev_;
    };


}

