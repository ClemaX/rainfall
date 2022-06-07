FROM alpine:latest

VOLUME /home/r2/share

RUN adduser -D r2

RUN apk add --update-cache \
	git \
	build-base \
	pkgconfig \
	radare2 \
	radare2-dev \
	openssl-dev \
	capstone-dev \
	zlib-dev \
	libzip-dev \
	&& rm -rf /var/cache/apk/*

USER r2
WORKDIR /home/r2

RUN r2pm init && r2pm install r2ghidra && r2pm --clean

WORKDIR /home/r2/share
