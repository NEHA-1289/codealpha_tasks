
FROM gcc:14

WORKDIR /app

COPY banking_system.cpp .
COPY accounts.txt .
COPY customers.txt .

RUN g++ -std=c++17 -O2 banking_system.cpp -o banking_system

CMD ["./banking_system"]
