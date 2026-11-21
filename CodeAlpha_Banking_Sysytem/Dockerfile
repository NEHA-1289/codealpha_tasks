
FROM gcc:14

WORKDIR /app

COPY server.cpp .
COPY accounts.txt .
COPY customers.txt .

RUN g++ -std=c++17 -O2 server.cpp -o server

EXPOSE 10000

CMD ["./server"]
