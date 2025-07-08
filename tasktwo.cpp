#include <bits/stdc++.h>
using namespace std;
#define LOG(x) cout << "Quantum book store: " << x << endl
class Book {
protected:
    string isbn, title, author;
    int year;
    double price;

public:
    Book(string i, string t, string a, int y, double p)
        : isbn(i), title(t), author(a), year(y), price(p) {}

    virtual bool canBuy(int quantity) const = 0;
    virtual double buy(int quantity, string email, string address) = 0;
    virtual void show() const = 0;
    virtual bool isOld(int currentYear, int maxAge) const {
        return (currentYear - year) > maxAge;
    }
    string getISBN() const { return isbn; }
    virtual ~Book() = default;
};

class PaperBook : public Book {
    int stock;

public:
    PaperBook(string i, string t, string a, int y, double p, int s)
        : Book(i, t, a, y, p), stock(s) {}

    bool canBuy(int quantity) const override {
        return stock >= quantity;
    }

    double buy(int quantity, string email, string address) override {
        if (!canBuy(quantity)) {
            throw runtime_error("Not enough stock for ISBN: " + isbn);
        }
        stock -= quantity;
        LOG("Shipping " + to_string(quantity) + " copy(ies) of " + title + " to " + address);
        return price * quantity;
    }

    void show() const override {
        LOG("Book: " + title + " | Author: " + author + " | ISBN: " + isbn +
           " | Year: " + to_string(year) + " | Price: $" + to_string(price));
        LOG("Type: Paper | Stock: " + to_string(stock));
    }
};

class EBook : public Book {
    string filetype;

public:
    EBook(string i, string t, string a, int y, double p, string f)
        : Book(i, t, a, y, p), filetype(f) {}

    bool canBuy(int quantity) const override {
        return true; 
    }

    double buy(int quantity, string email, string address) override {
        LOG("Sending " + to_string(quantity) + " copy(ies) of " + title +
           " (" + filetype + ") to " + email);
        return price * quantity;
    }

    void show() const override {
        LOG("Book: " + title + " | Author: " + author + " | ISBN: " + isbn +
           " | Year: " + to_string(year) + " | Price: $" + to_string(price));
        LOG("Type: EBook | Filetype: " + filetype);
    }
};

class ShowcaseBook : public Book {
public:
    ShowcaseBook(string i, string t, string a, int y)
        : Book(i, t, a, y, 0) {}

    bool canBuy(int quantity) const override {
        return false;
    }

    double buy(int quantity, string email, string address) override {
        throw runtime_error("Showcase book (ISBN: " + isbn + ") is not for sale!");
    }

    void show() const override {
        LOG("Book: " + title + " | Author: " + author + " | ISBN: " + isbn +
           " | Year: " + to_string(year));
        LOG("Type: Showcase | Not for sale");
    }
};

class Bookstore {
    vector<unique_ptr<Book>> inventory;

public:
    void add(unique_ptr<Book> book) {
        inventory.push_back(move(book));
    }

    void list() const {
        if (inventory.empty()) {
            LOG("Inventory is empty");
            return;
        }
        for (const auto& book : inventory) {
            book->show();
        }
    }

    void removeOldBooks(int maxAge) {
        time_t now = time(nullptr);
        int currentYear = 1900 + localtime(&now)->tm_year;

        vector<unique_ptr<Book>> updated;
        for (auto& book : inventory) {
            if (book->isOld(currentYear, maxAge)) {
                LOG("Removed outdated book: " + book->getISBN() +
                   " (Published " + to_string(currentYear - book->isOld(currentYear, 0)) + " years ago)");
            } else {
                updated.push_back(move(book));
            }
        }
        inventory = move(updated);
    }

    double buy(string isbn, int quantity, string email, string address) {
        for (auto& book : inventory) {
            if (book->getISBN() == isbn) {
                if (!book->canBuy(quantity)) {
                    throw runtime_error("Cannot buy " + to_string(quantity) +
                                      " of ISBN: " + isbn);
                }
                return book->buy(quantity, email, address);
            }
        }
        throw runtime_error("Book not found with ISBN: " + isbn);
    }
};

class QuantumBookstoreFullTest {
public:
    static void run() {
        Bookstore store;

       store.add(make_unique<PaperBook>("111", "C++ Made Easy", "Ali", 2012, 150.0, 5));
        store.add(make_unique<EBook>("222", "Learn Java", "Sara", 2019, 70.0, "PDF"));
        store.add(make_unique<ShowcaseBook>("333", "Old Secrets", "John", 1980));
        store.add(make_unique<PaperBook>("444", "Python Basics", "Ahmed", 2020, 90.0, 0)); // Out of stock

        LOG("Initial inventory:");
        store.list();

        testPurchase(store, "111", 2, "user1@mail.com", "Cairo, Egypt"); // Valid purchase
        testPurchase(store, "222", 1, "user2@mail.com", ""); // EBook
        testPurchase(store, "333", 1, "", ""); // Showcase book (should fail)
        testPurchase(store, "444", 1, "user3@mail.com", "Alexandria"); // Out of stock (should fail)
        testPurchase(store, "999", 1, "", ""); // Non-existent book (should fail)

        // Remove books older than 20 years (2023 - 20 = 2003)
        LOG("\nRemoving books older than 20 years...");
        store.removeOldBooks(20);

        LOG("\nFinal inventory:");
        store.list();
    }

private:
    static void testPurchase(Bookstore& store, string isbn, int quantity,
                           string email, string address) {
        try {
            LOG("\nAttempting to buy " + to_string(quantity) + " of ISBN: " + isbn);
            double total = store.buy(isbn, quantity, email, address);
            LOG("Total paid: $" + to_string(total));
        } catch (const exception& e) {
            LOG("Error: " + string(e.what()));
        }
    }
};

int main() {
    QuantumBookstoreFullTest::run();
    return 0;
}
