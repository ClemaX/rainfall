FROM blacktop/ghidra:latest

RUN adduser --disabled-password --gecos '' ghidra 

VOLUME /home/ghidra/share

COPY --chown=ghidra:ghidra . /home/ghidra/

USER ghidra

WORKDIR /home/ghidra/share
