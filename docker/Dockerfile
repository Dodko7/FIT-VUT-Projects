FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    git \
    cmake \
    sudo \
    valgrind \
    gdb \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Vytvoríme používateľa devuser s automatickým UID/GID (nefixujeme 1000)
ARG USERNAME=devuser
ARG USER_HOME=/home/$USERNAME

RUN useradd -m -s /bin/bash $USERNAME && \
    echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME && \
    chmod 0440 /etc/sudoers.d/$USERNAME

# Nakopírujeme a zostavíme SIMLIB
COPY lib/simlib /tmp/simlib
RUN cd /tmp/simlib && \
    make && make test && \
    cd src && make install && \
    rm -rf /tmp/simlib

# Prepneme na nového používateľa
USER $USERNAME
WORKDIR /workspace

CMD ["/bin/bash"]