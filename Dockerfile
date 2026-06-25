FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
    g++ \
    libasio-dev \
    libssl-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN g++ app.cpp -std=c++17 -I Crow/include -pthread -o app

EXPOSE 8080

CMD ["./app"]