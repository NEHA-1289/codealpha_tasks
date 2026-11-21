
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

struct Customer { int id; string name, phone; };
struct Account { int number, customerId; double balance; };
struct Tx { int account; string type, details, date; double amount; };

vector<Customer> customers;
vector<Account> accounts;
vector<Tx> txs;

string nowText() {
    time_t t = time(nullptr);
    string s = ctime(&t);
    if (!s.empty() && s.back() == '\n') s.pop_back();
    return s;
}

void load() {
    ifstream c("customers.txt");
    Customer x;
    while (c >> x.id >> x.name >> x.phone) customers.push_back(x);

    ifstream a("accounts.txt");
    Account y;
    while (a >> y.number >> y.customerId >> y.balance)
        accounts.push_back(y);
}

void save() {
    ofstream c("customers.txt");
    for (auto &x : customers)
        c << x.id << " " << x.name << " " << x.phone << "\n";

    ofstream a("accounts.txt");
    for (auto &x : accounts)
        a << x.number << " " << x.customerId << " " << x.balance << "\n";
}

string decode(const string &s) {
    string o;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '+') o += ' ';
        else if (s[i] == '%' && i + 2 < s.size()) {
            try {
                o += (char)stoi(s.substr(i + 1, 2), nullptr, 16);
                i += 2;
            } catch (...) { o += s[i]; }
        } else o += s[i];
    }
    return o;
}

string field(const string &b, const string &k) {
    string key = k + "=";
    size_t p = b.find(key);
    if (p == string::npos) return "";
    p += key.size();
    size_t e = b.find('&', p);
    return decode(b.substr(p, e == string::npos ? string::npos : e - p));
}

int num(const string &s) {
    try { return stoi(s); } catch (...) { return 0; }
}

double money(const string &s) {
    try { return stod(s); } catch (...) { return 0; }
}

Account* findAcc(int n) {
    for (auto &a : accounts)
        if (a.number == n) return &a;
    return nullptr;
}

bool customerExists(int n) {
    for (auto &c : customers)
        if (c.id == n) return true;
    return false;
}

string esc(string s) {
    string o;
    for (char c : s) {
        if (c == '&') o += "&amp;";
        else if (c == '<') o += "&lt;";
        else if (c == '>') o += "&gt;";
        else if (c == '"') o += "&quot;";
        else o += c;
    }
    return o;
}

string page(const string &msg = "") {
    ostringstream h;
    h << "<!doctype html><html><head>"
      << "<meta name='viewport' content='width=device-width,initial-scale=1'>"
      << "<title>Banking System</title>"
      << "<style>"
      << "body{font-family:Arial;background:#f2f5fa;margin:0;color:#172033}"
      << ".wrap{max-width:950px;margin:30px auto;padding:16px}"
      << "h1{color:#123b70}"
      << ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(260px,1fr));gap:14px}"
      << ".card{background:white;padding:18px;border-radius:12px;box-shadow:0 2px 9px #0001}"
      << "input,button{box-sizing:border-box;width:100%;padding:10px;margin:6px 0;border:1px solid #ccd3df;border-radius:6px}"
      << "button{background:#1557a0;color:white;border:0;cursor:pointer}"
      << "table{width:100%;border-collapse:collapse}"
      << "td,th{padding:8px;border-bottom:1px solid #ddd;text-align:left}"
      << ".msg{background:#e6f5e9;padding:12px;border-radius:8px;margin:12px 0}"
      << "</style></head><body><div class='wrap'>"
      << "<h1>Banking Management System</h1>"
      << "<p>Manage customers, accounts and transactions</p>";

    if (!msg.empty())
        h << "<div class='msg'>" << esc(msg) << "</div>";

    h << "<div class='grid'>"
      << "<div class='card'><h3>Create Customer</h3>"
      << "<form method='POST' action='/customer'>"
      << "<input name='id' type='number' placeholder='Customer ID' required>"
      << "<input name='name' placeholder='Name (one word)' required>"
      << "<input name='phone' placeholder='Phone number' required>"
      << "<button>Create Customer</button></form></div>"

      << "<div class='card'><h3>Create Account</h3>"
      << "<form method='POST' action='/account'>"
      << "<input name='number' type='number' placeholder='Account number' required>"
      << "<input name='customer' type='number' placeholder='Customer ID' required>"
      << "<button>Create Account</button></form></div>"

      << "<div class='card'><h3>Deposit Money</h3>"
      << "<form method='POST' action='/deposit'>"
      << "<input name='number' type='number' placeholder='Account number' required>"
      << "<input name='amount' type='number' min='0.01' step='0.01' placeholder='Amount' required>"
      << "<button>Deposit</button></form></div>"

      << "<div class='card'><h3>Withdraw Money</h3>"
      << "<form method='POST' action='/withdraw'>"
      << "<input name='number' type='number' placeholder='Account number' required>"
      << "<input name='amount' type='number' min='0.01' step='0.01' placeholder='Amount' required>"
      << "<button>Withdraw</button></form></div>"

      << "<div class='card'><h3>Transfer Funds</h3>"
      << "<form method='POST' action='/transfer'>"
      << "<input name='from' type='number' placeholder='Sender account' required>"
      << "<input name='to' type='number' placeholder='Receiver account' required>"
      << "<input name='amount' type='number' min='0.01' step='0.01' placeholder='Amount' required>"
      << "<button>Transfer</button></form></div>"

      << "<div class='card'><h3>View Account</h3>"
      << "<form method='GET' action='/account'>"
      << "<input name='number' type='number' placeholder='Account number' required>"
      << "<button>View Account</button></form></div></div>"

      << "<div class='card'><h3>Customers</h3>"
      << "<table><tr><th>ID</th><th>Name</th><th>Phone</th></tr>";

    for (auto &c : customers)
        h << "<tr><td>" << c.id << "</td><td>" << esc(c.name)
          << "</td><td>" << esc(c.phone) << "</td></tr>";

    h << "</table></div><div class='card'><h3>Accounts</h3>"
      << "<table><tr><th>Account</th><th>Customer ID</th>"
      << "<th>Balance (Rs.)</th></tr>";

    for (auto &a : accounts)
        h << "<tr><td>" << a.number << "</td><td>" << a.customerId
          << "</td><td>" << a.balance << "</td></tr>";

    h << "</table></div><div class='card'><h3>Recent Transactions</h3>"
      << "<table><tr><th>Account</th><th>Type</th><th>Amount</th>"
      << "<th>Details</th><th>Date</th></tr>";

    for (auto it = txs.rbegin(); it != txs.rend(); ++it)
        h << "<tr><td>" << it->account << "</td><td>" << esc(it->type)
          << "</td><td>" << it->amount << "</td><td>" << esc(it->details)
          << "</td><td>" << esc(it->date) << "</td></tr>";

    h << "</table></div></div></body></html>";
    return h.str();
}

string handle(const string &method, const string &path, const string &body) {
    string msg = "";

    if (method == "POST" && path == "/customer") {
        int id = num(field(body, "id"));
        string n = field(body, "name"), p = field(body, "phone");

        if (id <= 0 || n.empty() || p.empty())
            msg = "Please enter valid customer details.";
        else if (customerExists(id))
            msg = "Customer ID already exists.";
        else {
            customers.push_back({id, n, p});
            save();
            msg = "Customer created successfully.";
        }
    }
    else if (method == "POST" && path == "/account") {
        int no = num(field(body, "number"));
        int cid = num(field(body, "customer"));

        if (no <= 0 || !customerExists(cid))
            msg = "Invalid account number or customer ID not found.";
        else if (findAcc(no))
            msg = "Account number already exists.";
        else {
            accounts.push_back({no, cid, 0});
            save();
            msg = "Account created successfully.";
        }
    }
    else if (method == "POST" &&
             (path == "/deposit" || path == "/withdraw")) {
        int no = num(field(body, "number"));
        double amount = money(field(body, "amount"));
        Account *a = findAcc(no);

        if (!a) msg = "Account not found.";
        else if (amount <= 0) msg = "Enter an amount greater than zero.";
        else if (path == "/withdraw" && amount > a->balance)
            msg = "Insufficient balance.";
        else {
            a->balance += (path == "/deposit" ? amount : -amount);
            txs.push_back({no,
                path == "/deposit" ? "Deposit" : "Withdrawal",
                path == "/deposit" ? "Money deposited" : "Money withdrawn",
                nowText(), amount});
            save();
            msg = path == "/deposit" ? "Deposit successful." : "Withdrawal successful.";
        }
    }
    else if (method == "POST" && path == "/transfer") {
        int from = num(field(body, "from"));
        int to = num(field(body, "to"));
        double amount = money(field(body, "amount"));
        Account *a = findAcc(from), *b = findAcc(to);

        if (!a || !b) msg = "Sender or receiver account not found.";
        else if (a == b) msg = "Sender and receiver must be different accounts.";
        else if (amount <= 0) msg = "Enter an amount greater than zero.";
        else if (amount > a->balance) msg = "Insufficient balance.";
        else {
            a->balance -= amount;
            b->balance += amount;
            txs.push_back({from, "Transfer", "Sent to account " + to_string(to), nowText(), amount});
            txs.push_back({to, "Received", "Received from account " + to_string(from), nowText(), amount});
            save();
            msg = "Transfer successful.";
        }
    }
    else if (method == "GET" && path.rfind("/account?", 0) == 0) {
        string q = path.substr(path.find('?') + 1);
        int no = num(field(q, "number"));
        Account *a = findAcc(no);

        if (!a) msg = "Account not found.";
        else msg = "Account " + to_string(a->number) +
            " | Customer ID " + to_string(a->customerId) +
            " | Balance Rs. " + to_string(a->balance);
    }
    else if (method != "GET" ||
             (path != "/" && path != "/favicon.ico"))
        msg = "Unknown request.";

    return page(msg);
}

int main() {
    load();

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return 1;

    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(getenv("PORT") ? atoi(getenv("PORT")) : 10000);

    if (bind(s, (sockaddr*)&addr, sizeof(addr)) < 0) return 2;
    if (listen(s, 20) < 0) return 3;

    while (true) {
        int c = accept(s, nullptr, nullptr);
        if (c < 0) continue;

        string req;
        char buf[4096];
        int n;

        while ((n = recv(c, buf, sizeof(buf), 0)) > 0) {
            req.append(buf, n);
            size_t he = req.find("\r\n\r\n");

            if (he != string::npos) {
                size_t clp = req.find("Content-Length:");
                int cl = 0;
                if (clp != string::npos)
                    cl = atoi(req.c_str() + clp + 15);

                if (req.size() >= he + 4 + (size_t)cl) break;
            }
        }

        string head = req.substr(0, req.find("\r\n"));
        istringstream hs(head);
        string method, path, ver;
        hs >> method >> path >> ver;

        size_t he = req.find("\r\n\r\n");
        string body = he == string::npos ? "" : req.substr(he + 4);
        string out = handle(method, path, body);

        string response = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " + to_string(out.size()) +
            "\r\nConnection: close\r\n\r\n" + out;

        send(c, response.c_str(), response.size(), 0);
        close(c);
    }

    close(s);
    return 0;
}
