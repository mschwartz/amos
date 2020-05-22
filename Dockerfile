FROM gcc:latest
RUN apt-get update && apt-get -y install nasm
RUN apt-get -y install bochs
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
