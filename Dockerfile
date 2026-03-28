FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest AS builder

WORKDIR /app
COPY . /app/

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --target myservice -j$(nproc)

FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest

COPY --from=builder /app/build/myservice /app/myservice
COPY --from=builder /app/configs /app/configs

EXPOSE 8080

WORKDIR /app
CMD ["./myservice", "--config", "configs/static_config.yaml", "--config_vars", "configs/config_vars.yaml"]