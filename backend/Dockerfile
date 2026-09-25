FROM ubuntu:24.04 AS dependencias

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    libpqxx-dev \
    libboost-system-dev \
    libasio-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

FROM dependencias AS build

WORKDIR /app

COPY backend/CMakeLists.txt backend/CMakeLists.txt
COPY backend/lib backend/lib
COPY backend/tests backend/tests
COPY backend/api backend/api

WORKDIR /app/backend

RUN mkdir -p build \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF .. \
    && cmake --build . --config Release --target ensalamento_api -j "$(nproc)"

FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    libpqxx-dev \
    libboost-system-dev \
    libssl3 \
    libcurl4 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd --system ensalamento \
    && useradd --system --gid ensalamento --no-create-home --shell /usr/sbin/nologin ensalamento

WORKDIR /app

COPY --from=build /app/backend/build/api/ensalamento_api /app/ensalamento_api

RUN chown -R ensalamento:ensalamento /app

USER ensalamento

EXPOSE 8080

HEALTHCHECK --interval=30s --timeout=5s --start-period=10s --retries=3 \
    CMD sh -c 'curl -f http://localhost:${PORT:-${BACKEND_PORT:-8080}}/api/saude || exit 1'

ENTRYPOINT ["/bin/sh", "-c"]
CMD ["exec /app/ensalamento_api"]