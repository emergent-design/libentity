# configure apt to be noninteractive
ENV DEBIAN_FRONTEND noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN true
WORKDIR /code

FROM ubuntu:18.04
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates curl build-essential clang fakeroot chrpath
RUN curl -L https://github.com/premake/premake-core/releases/download/v5.0.0-alpha15/premake-5.0.0-alpha15-linux.tar.gz | tar -xz -C /usr/bin/


code:
	COPY --dir include packages src premake5.lua .

build:
	FROM +code
	RUN premake5 gmake && make -j$(nproc)

package:
	FROM +code
	RUN cd packages && ./build
	SAVE ARTIFACT packages/libentity-dev_*.deb libentity-dev.deb
	#AS LOCAL build/

#deploy:
#	FROM +package

