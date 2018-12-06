FROM alpine:3.8
RUN apk add --no-cache cmake make g++ openssh git libexecinfo-dev libbsd-dev util-linux-dev openssl-dev libc6-compat bash curl

WORKDIR /usr/src

RUN git clone https://github.com/neo4j/seabolt.git
WORKDIR /usr/src/seabolt/build
RUN cmake -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=dist /usr/src/seabolt \
    && cmake --build . --target install

WORKDIR /usr/src
RUN git clone https://github.com/tjb1982/lal.git
WORKDIR /usr/src/lal
RUN make install

ADD . /usr/src/lal-seabolt
RUN rm -rf /usr/src/lal-seabolt/lib64 /usr/src/lal-seabolt/build/CMakeCache.txt
RUN cp -R /usr/src/seabolt/build/dist/lib64 /usr/src/lal-seabolt/
WORKDIR /usr/src/lal-seabolt/build

RUN ls -l /usr/src/lal-seabolt/lib64/
RUN cmake /usr/src/lal-seabolt && make
CMD ./bin/lal-seabolt

