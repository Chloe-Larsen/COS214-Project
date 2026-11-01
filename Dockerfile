# TIVIDY - COS214 Workflow Management System
#
# Two runnable images are defined here:
#
#   core (default)  Builds the C++11 workflow engine in src/ + include/ and runs it.
#       docker build -t tividy .
#       docker run --rm tividy
#
#   gui             Builds the Qt 5 front end in gui/ (needs the gui/ folder in the
#                   build context) and runs it. Requires a display; see README.
#       docker build --target gui -t tividy-gui .


ARG UBUNTU_VERSION=24.04

# -----------------------------------
# GUI: build stage
FROM ubuntu:${UBUNTU_VERSION} AS gui-build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential make pkg-config qtbase5-dev qtbase5-dev-tools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
# gui/Makefile expects the core at ../include and ../src (CORE_ROOT=..)
COPY include ./include
COPY src ./src
COPY gui ./gui
RUN make -C gui -j"$(nproc)"

# -----------------------------------
# GUI: runtime stage
FROM ubuntu:${UBUNTU_VERSION} AS gui

# Runtime Qt libraries only (libqt5gui5t64 brings in the xcb platform plugin deps)
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libqt5widgets5t64 libqt5concurrent5t64 libqt5gui5t64 \
    && rm -rf /var/lib/apt/lists/*

RUN useradd --create-home --shell /bin/bash tividy
USER tividy
WORKDIR /home/tividy

COPY --from=gui-build /app/gui/build/tividy-gui /usr/local/bin/tividy-gui

# Runtime dir silences Qt's XDG_RUNTIME_DIR warning
ENV XDG_RUNTIME_DIR=/tmp/runtime-tividy
RUN mkdir -p "$XDG_RUNTIME_DIR" && chmod 700 "$XDG_RUNTIME_DIR"

CMD ["tividy-gui"]

# -----------------------------------
# Core: build stage
FROM ubuntu:${UBUNTU_VERSION} AS core-build

RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY Makefile ./
COPY include ./include
COPY src ./src
# `make program` builds only; plain `make` would also run the binary
RUN make program

# ------------------------------------
# Core: runtime stage (default image)
FROM ubuntu:${UBUNTU_VERSION} AS core

RUN useradd --create-home --shell /bin/bash tividy
USER tividy
WORKDIR /home/tividy

COPY --from=core-build /app/program /usr/local/bin/tividy

CMD ["tividy"]