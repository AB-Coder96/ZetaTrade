# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DZF_ENABLE_TESTS=OFF \
  && cmake --build build -j

FROM ubuntu:24.04 AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /src/build/Zetaforge_replay /app/
COPY --from=build /src/build/Zetaforge_trade  /app/
COPY --from=build /src/build/Zetaforge_bench  /app/
COPY --from=build /src/configs /app/configs
COPY --from=build /src/data    /app/data

ENTRYPOINT ["/app/Zetaforge_replay"]
