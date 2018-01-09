#!/usr/bin/env bash

rm -rf plustache && rm -rf Simple-Web-Server && rm -rf asio

# Download and install plustache
git clone https://github.com/mrtazz/plustache.git && ./generator-plustache-hxx.sh

# Download Simple-Web-Server
git clone https://github.com/eidheim/Simple-Web-Server.git

# Download and install asio
# git clone https://github.com/chriskohlhoff/asio.git && cd asio/asio && ./autogen.sh



