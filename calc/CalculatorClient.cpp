
#include "gen-cpp2/Calculator.h"
#include <iostream>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::async;
using namespace example::cpp2;
using namespace folly::wangle;

int main(int argc, char **argv) {


    TEventBase base;

    int port = 8082;

    std::shared_ptr<TAsyncSocket> socket(
            TAsyncSocket::newSocket(&base, "127.0.0.1", port));

    auto client_channel = HeaderClientChannel::newChannel(socket);
    CalculatorAsyncClient client(std::move(client_channel));

    folly::wangle::Future<int64_t> f = client.future_add(2, 3);

    

    f.then(
        [](Try<int64_t>&& t) {
            std::cout << "Result = " << t.value() << std::endl;
        }
    );

    // Run a single iteration of the event loop, in reality, this would be a 
    // nearly infinite loop, where we stop the looping when the client should terminate
    base.loopForever();

}
