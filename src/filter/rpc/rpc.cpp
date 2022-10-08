#include "frei0r.hpp"
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

class RPC : public frei0r::filter
{

private:
    std::string host;
    double port;
    std::string metadata;
    bool udp;

    int sockfd = 0;
    unsigned int width;
    unsigned int height;
    struct sockaddr_in servaddr;

public:
    RPC(unsigned int width, unsigned int height)
    {
        this->width = width;
        this->height = height;

        host = "localhost";
        register_param(host, "host", "Host of the RPC filter server");
        port = 7890;
        register_param(port, "port", "Port of the RPC filter server");
        metadata = "";
        register_param(metadata, "metadata", "Metadata for the RPC filter server");
        udp = false;
        register_param(udp, "udp", "Use UDP instead of TCP");
    }

    ~RPC()
    {
        std::cout << "[rpc] closing socket" << std::endl;
        close(sockfd);
    }

    void init()
    {
        uint16_t uport = (uint16_t)port;

        fprintf(stdout, "[rpc] initialized host=%s port=%d metadata=%s udp=%d\n",
                host.c_str(), uport, metadata.c_str(), udp);

        if ((sockfd = socket(AF_INET, udp ? SOCK_DGRAM : SOCK_STREAM, 0)) < 0)
        {
            perror("[rpc] socket creation failed");
            exit(EXIT_FAILURE);
        }

        memset(&servaddr, 0, sizeof(servaddr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(uport);
        servaddr.sin_addr.s_addr = inet_addr(host.c_str());

        fprintf(stdout, "[rpc] sending metadata to server\n");
        // send((void*)width, sizeof(unsigned int));
        // send((void*)height, sizeof(unsigned int));
        send(metadata.c_str(), metadata.length());

        fprintf(stdout, "[rpc] metadata sent to server\n");
    }

    void send(const void *data, size_t size)
    {
        fprintf(stdout, "[rpc] sending message of size %d\n", size);

        sendto(sockfd,
               data, size,
               MSG_BATCH,
               (const struct sockaddr *)&servaddr, sizeof(servaddr));
    }

    virtual void update(double time,
                        uint32_t *out,
                        const uint32_t *in)
    {
        if (!sockfd)
        {
            init();
        }

        send(in, 65507);

        socklen_t len = width * height;
        recvfrom(sockfd, out,
                 width * height, MSG_WAITALL,
                 (struct sockaddr *)&servaddr, &len);
    }
};

frei0r::construct<RPC> plugin("RPC",
                              "Remote Procedural Call Filter",
                              "Wei",
                              0, 2,
                              F0R_COLOR_MODEL_RGBA8888);
