FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential libcurl4-openssl-dev python3 ca-certificates \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY concord ./concord
RUN mkdir -p concord/lib && make -C concord
COPY Makefile ./
COPY inc ./inc
COPY src ./src
COPY tests ./tests
COPY bot.conf.example ./bot.conf.example
RUN make check

FROM debian:bookworm-slim AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends \
    libcurl4 ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/build/dc_bot ./dc_bot
COPY --from=build /app/bot.conf.example ./bot.conf
USER 10001:10001
ENTRYPOINT ["/app/dc_bot"]
CMD ["--settings", "/app/bot.conf"]
