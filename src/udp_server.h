/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Norwegian University of Science and Technology
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Norwegian University of Science and
 *     Technology, nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/*
 * Author: Lars Tingelstad
 * Modified: Keivan Zavari
 */

#ifndef UDP_SERVER_
#define UDP_SERVER_

#include <iostream>
#include <string>

// Select includes
#include <sys/time.h>

#include <stdio.h>
#include <unistd.h> //ssize_t
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static const int BUFSIZE=2048;


class UDPServer
{
public:

	int sockfd_;


	UDPServer(){};

	UDPServer(std::string host, int port) : 
	  local_host_(host)
	, local_port_(port)
	, timeout_(false)
	{
		/* create a udp/ip socket */
		sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
		
		if (sockfd_ < 0) {
			// std::cout << "ERROR opening socket" << std::endl;
			perror("ERROR cannot create socket\n");

		}
		
		// optval = 1;
		// setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
		memset(&serveraddr_, 0, sizeof(serveraddr_));
		serveraddr_.sin_family = AF_INET;
		serveraddr_.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr(local_host_.c_str());
		serveraddr_.sin_port = htons(local_port_); // htons(0);
		
		int bind_int = bind(sockfd_, (struct sockaddr *) &serveraddr_, sizeof(serveraddr_));
		
		if (bind_int < 0) {
			std::cout << "bind result is negative: " << bind_int << std::endl;
			int enable = 1;
			if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
				perror("setsockopt(SO_REUSEADDR) failed");
			#ifdef SO_REUSEPORT
			if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, (const char*)&enable, sizeof(enable)) < 0)
					perror("setsockopt(SO_REUSEPORT) failed");
			#endif

			// perror("ERROR binding socket\n");
		}

		// char const *server_address = "127.0.0.1"; /* change this to use a different server */


		memset((char *) &clientaddr_, 0, sizeof(clientaddr_));
		clientaddr_.sin_family = AF_INET;
		clientaddr_.sin_port = htons(port);
		if (inet_aton(local_host_.c_str(), &clientaddr_.sin_addr)==0) {
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
		clientlen_ = sizeof(clientaddr_);

		std::cout << "\n------------------------------------------------"  << std::endl;
		std::cout << "with local_host_: "  << local_host_ << std::endl;
		std::cout << "and local_port_: " << local_port_ << std::endl;
		std::cout << "------------CONSTRUCTED SUCCESSFULLY------------"  << std::endl;
	}

	~UDPServer()
	{
		std::cout << "\n-------------------DESTRUCTED-------------------\n"  << std::endl;
		close(sockfd_);
	}

	bool set_timeout(int millisecs) {

		if (millisecs != 0)
		{
			tv_.tv_sec  = millisecs / 1000;
			tv_.tv_usec = (millisecs % 1000) * 1000;
			timeout_ = true;
			return timeout_;
		}
		else {
			timeout_ = false;
			return timeout_;
		}

	}

	// Send function
	ssize_t send(std::string& buffer){
		std::cout << "\nbroadcasting: " << buffer << std::endl;
		ssize_t bytes = 0;
		bytes = sendto(sockfd_, buffer.c_str(), buffer.size(), 0, (struct sockaddr *) &clientaddr_, clientlen_);
		if (bytes < 0)
			std::cout << "ERROR in sendto" << std::endl;

		return bytes;
	}


	// Receive function
	ssize_t recv(std::string& buffer) {

		ssize_t bytes = 0;

		if (timeout_) {

			fd_set read_fds;
			FD_ZERO(&read_fds);
			FD_SET(sockfd_, &read_fds);

			struct timeval tv;
			tv.tv_sec = tv_.tv_sec;
			tv.tv_usec = tv_.tv_usec;

			if (select(sockfd_+1, &read_fds, NULL, NULL, &tv) < 0)
				return 0;

			if (FD_ISSET(sockfd_, &read_fds))
			{
				memset(buffer_, 0, BUFSIZE);
				bytes = recvfrom(sockfd_, buffer_, BUFSIZE, 0, (struct sockaddr *) &clientaddr_, &clientlen_);
				if (bytes < 0)
					std::cout << "ERROR in recvfrom" << std::endl;
			}
			else
				return 0;

		}
		else
		{
			printf("waiting on port %d \t %d \n", clientaddr_.sin_port, ntohs(clientaddr_.sin_port));

			memset(buffer_, 0, BUFSIZE);

			bytes = recvfrom(sockfd_, buffer_, BUFSIZE, 0, (struct sockaddr *) &clientaddr_, &clientlen_);
			if (bytes < 0)
				std::cout << "ERROR in recvfrom" << std::endl;
			else {
				buffer_[bytes] = 0;
				std::cout << "received: " << buffer_ << std::endl;
			}

		}

		buffer = std::string(buffer_);

		return bytes;
	}

private:
	std::string local_host_;
	unsigned short local_port_;
	bool timeout_;
	struct timeval tv_;

	socklen_t clientlen_;
	struct sockaddr_in serveraddr_;
	struct sockaddr_in clientaddr_;
	char buffer_[BUFSIZE];
	// int optval;

};

#endif

