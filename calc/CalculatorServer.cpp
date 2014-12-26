#include <iostream>

#include "gen-cpp2/Calculator.h"
#include <thrift/lib/cpp2/server/ThriftServer.h>

using namespace std;
using namespace apache::thrift;
using namespace example::cpp2;

class CalculatorService: public CalculatorSvIf, public boost::noncopyable {
    
    public:
        CalculatorService()
            : addRequests_(0) { }
        virtual ~CalculatorService() { }
        void async_tm_add(
                std::unique_ptr<apache::thrift::HandlerCallback<int64_t>> callback,
                int32_t num1,
                int32_t num2) {
            std::cout << "Revieved async request" << std::endl;

            // TODO, does this need to be locked?
            // Where can this handler be called from (in terms of which threads)?
            addRequests_++;

            callback->result(num1 + num2);

        }

        void httpHandler(
                apache::thrift::async::TEventBase* ev,
                std::shared_ptr<apache::thrift::async::TAsyncTransport> tr,
                std::unique_ptr<folly::IOBuf> buf) {

            std::cout << "Received HTTP request" << std::endl;

            std::string content = std::to_string(addRequests_);
            std::string resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
            resp = resp + std::to_string(content.size()) + std::string("\r\n\r\n") + content;

            cout << "Sending response: " << content << std::endl; 

            // Write response out onto the transport
            tr->write(nullptr, resp.c_str(), resp.size());
        }

    private:
        int addRequests_;
};

int main (int argc, char **argv) {
    
    std::shared_ptr<CalculatorService> ptr(new CalculatorService());
     
    getHandlerFunc getHandler = [ptr] (
            apache::thrift::async::TEventBase* ev,
            std::shared_ptr<apache::thrift::async::TAsyncTransport> tr,
            std::unique_ptr<folly::IOBuf> buf) {
        ptr->httpHandler(ev, tr, std::move(buf));
    };
    
    ThriftServer* s = new ThriftServer();
    s->setInterface(ptr);

    // Configure getHandler for the server
    s->setGetHandler(getHandler); 
    cout << "Setting port..." << std::endl;
    s->setAddress("127.0.0.1", 8081);
    
    cout << "Starting to serve requests..." << std::endl;
    s->serve();

    return 0;
}
