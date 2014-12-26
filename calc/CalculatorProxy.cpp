#include <iostream>

#include "gen-cpp2/Calculator.h"
#include <thrift/lib/cpp2/server/ThriftServer.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::async;
using namespace example::cpp2;
using namespace folly::wangle;

// This wrapper is used to capture unique_ptr<Callback>
// inside the lambda expressions since there is no "correct"
// way to capture unique_ptrs with lambdas
template<typename T>
struct CallbackWrapper {
    std::unique_ptr<HandlerCallback<T>> ptr;
    CallbackWrapper(std::unique_ptr<HandlerCallback<T>> &p)
        : ptr(std::move(p)) { }
    CallbackWrapper(const CallbackWrapper& o) {
        
        // The static cast is not a good idea, but is a
        // required workaround
        CallbackWrapper& cw = const_cast<CallbackWrapper&>(o);
        ptr = std::move(cw.ptr);
    }
};

class CalculatorProxy : public CalculatorSvIf {

    private:

        // Create a client object to send proxy requests to the main server
        CalculatorAsyncClient* getClient(TEventBase* ev, const char* addr, int port) {
            std::shared_ptr<TAsyncSocket> socket(
                    TAsyncSocket::newSocket(ev, addr, port));

            // Create the HeaderClientChannel
            auto client_channel = HeaderClientChannel::newChannel(socket);

            return new CalculatorAsyncClient(std::move(client_channel));
        }


    public:

    virtual ~CalculatorProxy() { }

    void async_tm_add(
        std::unique_ptr<apache::thrift::HandlerCallback<int64_t>> callback,
        int32_t num1,
        int32_t num2) {
    
        std::cout << "Received async request" << std::endl;

        TEventBase* ev = callback->getEventBase();

        CallbackWrapper<int64_t> cb(callback);
        
        ev->runInEventBaseThread([num1, num2, cb, this]() {
            CalculatorAsyncClient* client = getClient(cb.ptr->getEventBase(), "127.0.0.1", 8081);

            // Send the real call to the server
            folly::wangle::Future<int64_t> f = client->future_add(num1, num2);
            f.then(
                [cb](Try<int64_t>&& t) {
                    // Return the response from the main server back to the client
                std::cout << "Received response from server" << std::endl;
                cb.ptr->result(t.value());
            });
        });
    }
};

int main(int argc, char **argv) {
    
    std::shared_ptr<CalculatorProxy> ptr(new CalculatorProxy());
    ThriftServer* s = new ThriftServer();
    s->setInterface(ptr);
    s->setAddress("127.0.0.1", 8082);
    s->serve();

    return 0;
}
