VERSION 0.8

FROM ubuntu:22.04
ENV DEBIAN_FRONTEND noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN true
WORKDIR /code

COPY --if-exists proxy.conf /etc/apt/apt.conf.d/30-proxy
RUN apt-get update -q && apt-get install -y --no-install-recommends ca-certificates curl build-essential cmake clang fakeroot chrpath dh-exec


code:
	COPY --dir include packages src CMakeLists.txt .

check:
	FROM +code
	RUN cmake -B build \
		&& make -j8 -C build \
		&& make -C build test ARGS=--output-on-failure

package:
	FROM +code
	RUN cd packages && dpkg-buildpackage -b -uc -us
	SAVE ARTIFACT --keep-ts libentity-dev_*.deb AS LOCAL build/
	# SAVE ARTIFACT libentity-dev_*.deb libentity-dev.deb

entity-all:
	BUILD +package
