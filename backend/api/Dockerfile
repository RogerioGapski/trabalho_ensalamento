FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libpqxx-dev \
    libboost-system-dev \
    libasio-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY backend/lib /app/backend/lib
COPY backend/api /app/backend/api
COPY backend/CMakeLists.txt /app/backend/CMakeLists.txt

WORKDIR /app/backend
RUN mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --config Release -j

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libpqxx-dev \
    libssl3 \
    libcurl4 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /app/backend/build/api/ensalamento_api /app/ensalamento_api

EXPOSE 8080

CMD ["/app/ensalamento_api"]