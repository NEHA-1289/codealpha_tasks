
FROM gcc:14

WORKDIR /app

COPY server.cpp .

RUN g++ -std=c++17 -O2 server.cpp -o server

CMD ["./server"]
