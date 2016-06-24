#include <rtt/TaskContext.hpp>
#include <rtt/extras/FileDescriptorActivity.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "udp_server.h"

using namespace RTT;
using RTT::extras::FileDescriptorActivity;

class FileDescriptorActivityTest : public TaskContext
{
public:
    FileDescriptorActivityTest(const std::string &name)
        : TaskContext(name)
        , sockfd_(0)
        , port_(0)
        , first_run_updatehook_(true)
    {
        this->addProperty("port", port_);
        this->addProperty("addr", addr_);

        fda_ = new FileDescriptorActivity(0);
        this->setActivity(fda_);

    }

    ~FileDescriptorActivityTest()
    {}

    bool configureHook() {

        server_.reset(new UDPServer(addr_, port_));
        return true;
    }

    bool startHook()
    {
        // if (!sockfd_) {
        //     sockfd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
        //     if (sockfd_ < 0) {
        //         log(Error) << "startHook(): socket() failed with errno " << errno << ":"
        //                    << strerror(errno) << endlog();
        //         sockfd_ = 0;
        //         return false;
        //     }
        if (fda_) { 

            fda_->watch(server_->sockfd_); 
            // optional, set a timeout in milliseconds 
            fda_->setTimeout(1000); 
        }


        // struct sockaddr_in bind_address;
        // memset(&bind_address, 0, sizeof(struct sockaddr_in));
        // bind_address.sin_family = AF_INET;
        // bind_address.sin_addr.s_addr = INADDR_ANY;
        // bind_address.sin_port = htons(port_);
        // if (::bind(sockfd_, (struct sockaddr *) &bind_address, sizeof(bind_address)) < 0) {
        //     log(Error) << "startHook(): bind() failed with errno " << errno << ":"
        //                << strerror(errno) << endlog();
        //     stopHook();
        //     return false;
        // }

        // log(Info) << "startHook(): Successfully bound socket to UDP port " << ntohs(bind_address.sin_port) << endlog();
        return true;
    }

    void stopHook()
    {
        // if (sockfd_) {
            fda_->unwatch(server_->sockfd_);
            // close(sockfd_);
            server_.reset();
            // sockfd_ = 0;
        // }
    }

    void updateHook()
    {
        if (first_run_updatehook_){

            server_.reset(new UDPServer(addr_, port_));

            fda_->watch(server_->sockfd_);
            fda_->setTimeout(1000);

            std::cout << "first run updatehook" << std::endl;
            std::cout << "going to watch: " << fda_ << std::endl;

            // server_->set_timeout(100);
            first_run_updatehook_ = false;
        }
        // char buffer[1024];
        // ssize_t len = recv(sockfd_, buffer, sizeof(buffer), 0);
        // if (len > 0) {
        //     std::string data(buffer, len);
        //     if (*data.rbegin() == '\n') data = data.substr(0, data.size() - 1);
        //     log(Info) << "updateHook(): Received \"" << data << "\"" << endlog();
        // } else if (len == 0) {
        //     log(Info) << "updateHook(): recv() returned 0 bytes" << endlog();
        // } else {
        //     log(Error) << "updateHook(): recv() failed with errno " << errno << ":"
        //                << strerror(errno) << endlog();
        // }

    }

private:
    int sockfd_;
    int port_;
    std::string addr_;
    bool first_run_updatehook_;

    FileDescriptorActivity *fda_;

    boost::shared_ptr<UDPServer> server_;

};

#include <rtt/Component.hpp>
ORO_CREATE_COMPONENT(FileDescriptorActivityTest)

