// ============================================================
// EXPENSE TRACKER - OBJECT ORIENTED PROGRAMMING THROUGH C++
//
// Modules:
// 1. User
// 2. Category
// 3. Expense + Inheritance + Polymorphism
// 4. Budget
// 5. ExpenseManager - CRUD + File Handling
// 6. Report
// 7. Menu
//
// Compatible with C++11 / C++14 / C++17
//
// Compile:
// g++ -std=c++17 -Wall -o expense_tracker TRACKER.cpp
// ============================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


// ============================================================
// UTILITY FUNCTIONS
// ============================================================

namespace util
{
    // --------------------------------------------------------
    // Read a complete line
    // --------------------------------------------------------
    string readLine(const string &prompt)
    {
        string s;
        cout << prompt;
        getline(cin, s);
        return s;
    }


    // --------------------------------------------------------
    // Read integer
    // --------------------------------------------------------
    int readInt(const string &prompt)
    {
        while (true)
        {
            string s = readLine(prompt);

            try
            {
                size_t pos = 0;
                int value = stoi(s, &pos);

                if (pos == s.size())
                    return value;
            }
            catch (...)
            {
            }

            cout << "  Invalid number. Please try again.\n";
        }
    }


    // --------------------------------------------------------
    // Read positive double
    // --------------------------------------------------------
    double readPositiveDouble(const string &prompt)
    {
        while (true)
        {
            string s = readLine(prompt);

            try
            {
                size_t pos = 0;
                double value = stod(s, &pos);

                if (pos == s.size() && value > 0)
                    return value;
            }
            catch (...)
            {
            }

            cout << "  Enter a positive amount.\n";
        }
    }


    // --------------------------------------------------------
    // Validate DD-MM-YYYY
    // --------------------------------------------------------
    bool isValidDate(const string &d)
    {
        if (d.size() != 10)
            return false;

        if (d[2] != '-' || d[5] != '-')
            return false;

        for (size_t i = 0; i < d.size(); i++)
        {
            if (i == 2 || i == 5)
                continue;

            if (!isdigit(static_cast<unsigned char>(d[i])))
                return false;
        }

        try
        {
            int day = stoi(d.substr(0, 2));
            int month = stoi(d.substr(3, 2));
            int year = stoi(d.substr(6, 4));

            if (year < 1900 || year > 2100)
                return false;

            if (month < 1 || month > 12)
                return false;

            int daysInMonth[] =
            {
                0,
                31,
                28,
                31,
                30,
                31,
                30,
                31,
                31,
                30,
                31,
                30,
                31
            };

            // Leap year
            if ((year % 400 == 0) ||
                (year % 4 == 0 && year % 100 != 0))
            {
                daysInMonth[2] = 29;
            }

            if (day < 1 || day > daysInMonth[month])
                return false;
        }
        catch (...)
        {
            return false;
        }

        return true;
    }


    // --------------------------------------------------------
    // Read date
    // --------------------------------------------------------
    string readDate(const string &prompt)
    {
        while (true)
        {
            string d = readLine(prompt);

            if (isValidDate(d))
                return d;

            cout << "  Use format DD-MM-YYYY "
                    "(example 05-09-2026).\n";
        }
    }


    // --------------------------------------------------------
    // Convert DD-MM-YYYY -> MM-YYYY
    // --------------------------------------------------------
    string monthKey(const string &date)
    {
        if (date.size() >= 10)
            return date.substr(3);

        return "";
    }


    // --------------------------------------------------------
    // Replace | because | is our file delimiter
    // --------------------------------------------------------
    string clean(string s)
    {
        replace(s.begin(), s.end(), '|', '/');
        return s;
    }


    // --------------------------------------------------------
    // Split string using delimiter
    // --------------------------------------------------------
    vector<string> split(const string &s, char delim)
    {
        vector<string> parts;
        string item;

        stringstream ss(s);

        while (getline(ss, item, delim))
        {
            parts.push_back(item);
        }

        return parts;
    }
}


// ============================================================
// USER MODULE
// ============================================================

class User
{
private:

    string name;
    string userId;


public:

    User()
    {
    }


    User(const string &n, const string &id)
        : name(n), userId(id)
    {
    }


    const string &getName() const
    {
        return name;
    }


    const string &getId() const
    {
        return userId;
    }


    bool isRegistered() const
    {
        return !name.empty();
    }


    // --------------------------------------------------------
    // Register user
    // --------------------------------------------------------
    void registerUser()
    {
        name = util::clean(
            util::readLine("Enter your name: ")
        );

        userId = util::clean(
            util::readLine("Enter a user ID: ")
        );

        cout << "\nWelcome, " << name << "!\n";
    }


    // --------------------------------------------------------
    // Display user
    // --------------------------------------------------------
    void display() const
    {
        cout << "User: " << name
             << "  (ID: " << userId << ")\n";
    }


    // --------------------------------------------------------
    // Save user
    // --------------------------------------------------------
    bool saveToFile(const string &file) const
    {
        ofstream out(file.c_str());

        if (!out)
            return false;

        out << name << "|" << userId << "\n";

        return true;
    }


    // --------------------------------------------------------
    // Load user
    // --------------------------------------------------------
    bool loadFromFile(const string &file)
    {
        ifstream in(file.c_str());

        string line;

        if (!in || !getline(in, line))
            return false;

        vector<string> p = util::split(line, '|');

        if (p.size() < 2)
            return false;

        name = p[0];
        userId = p[1];

        return true;
    }
};


// ============================================================
// CATEGORY MODULE
// ============================================================

class Category
{
public:

    static const vector<string> &all()
    {
        static const vector<string> names =
        {
            "Food",
            "Travel",
            "Shopping",
            "Bills"
        };

        return names;
    }


    static void showMenu()
    {
        const vector<string> &categories = all();

        for (size_t i = 0; i < categories.size(); i++)
        {
            cout << "  "
                 << i + 1
                 << ". "
                 << categories[i]
                 << "\n";
        }
    }


    static bool isValid(const string &c)
    {
        const vector<string> &categories = all();

        return find(
            categories.begin(),
            categories.end(),
            c
        ) != categories.end();
    }
};


// ============================================================
// BASE EXPENSE CLASS
// ============================================================

class Expense
{
protected:

    int id;
    double amount;
    string date;
    string description;


public:

    Expense(
        int id,
        double amount,
        const string &date,
        const string &desc
    )
        : id(id),
          amount(amount),
          date(date),
          description(desc)
    {
    }


    virtual ~Expense()
    {
    }


    // --------------------------------------------------------
    // Pure virtual functions
    // --------------------------------------------------------

    virtual string getCategory() const = 0;

    virtual string getExtraLabel() const = 0;

    virtual string getExtra() const = 0;

    virtual void setExtra(const string &value) = 0;


    // --------------------------------------------------------
    // Display expense
    // --------------------------------------------------------

    virtual void display() const
    {
        cout
            << left
            << setw(5)
            << id

            << setw(12)
            << date

            << setw(10)
            << getCategory()

            << right
            << setw(10)
            << fixed
            << setprecision(2)
            << amount

            << "  "

            << left
            << setw(20)
            << description

            << getExtraLabel()
            << ": "
            << getExtra()

            << "\n";
    }


    // --------------------------------------------------------
    // Convert expense to file format
    // --------------------------------------------------------

    string serialize() const
    {
        ostringstream os;

        os
            << id
            << "|"
            << getCategory()
            << "|"
            << amount
            << "|"
            << date
            << "|"
            << description
            << "|"
            << getExtra();

        return os.str();
    }


    // --------------------------------------------------------
    // Getters
    // --------------------------------------------------------

    int getId() const
    {
        return id;
    }


    double getAmount() const
    {
        return amount;
    }


    const string &getDate() const
    {
        return date;
    }


    const string &getDescription() const
    {
        return description;
    }


    // --------------------------------------------------------
    // Setters
    // --------------------------------------------------------

    void setAmount(double a)
    {
        amount = a;
    }


    void setDate(const string &d)
    {
        date = d;
    }


    void setDescription(const string &d)
    {
        description = d;
    }
};


// ============================================================
// FOOD EXPENSE
// ============================================================

class FoodExpense : public Expense
{
private:

    string mealType;


public:

    FoodExpense(
        int id,
        double amount,
        const string &date,
        const string &description,
        const string &meal
    )
        : Expense(id, amount, date, description),
          mealType(meal)
    {
    }


    string getCategory() const override
    {
        return "Food";
    }


    string getExtraLabel() const override
    {
        return "Meal";
    }


    string getExtra() const override
    {
        return mealType;
    }


    void setExtra(const string &value) override
    {
        mealType = value;
    }
};


// ============================================================
// TRAVEL EXPENSE
// ============================================================

class TravelExpense : public Expense
{
private:

    string mode;


public:

    TravelExpense(
        int id,
        double amount,
        const string &date,
        const string &description,
        const string &transportMode
    )
        : Expense(id, amount, date, description),
          mode(transportMode)
    {
    }


    string getCategory() const override
    {
        return "Travel";
    }


    string getExtraLabel() const override
    {
        return "Mode";
    }


    string getExtra() const override
    {
        return mode;
    }


    void setExtra(const string &value) override
    {
        mode = value;
    }
};


// ============================================================
// SHOPPING EXPENSE
// ============================================================

class ShoppingExpense : public Expense
{
private:

    string shop;


public:

    ShoppingExpense(
        int id,
        double amount,
        const string &date,
        const string &description,
        const string &shopName
    )
        : Expense(id, amount, date, description),
          shop(shopName)
    {
    }


    string getCategory() const override
    {
        return "Shopping";
    }


    string getExtraLabel() const override
    {
        return "Shop";
    }


    string getExtra() const override
    {
        return shop;
    }


    void setExtra(const string &value) override
    {
        shop = value;
    }
};


// ============================================================
// BILL EXPENSE
// ============================================================

class BillExpense : public Expense
{
private:

    string billType;


public:

    BillExpense(
        int id,
        double amount,
        const string &date,
        const string &description,
        const string &bill
    )
        : Expense(id, amount, date, description),
          billType(bill)
    {
    }


    string getCategory() const override
    {
        return "Bills";
    }


    string getExtraLabel() const override
    {
        return "Bill";
    }


    string getExtra() const override
    {
        return billType;
    }


    void setExtra(const string &value) override
    {
        billType = value;
    }
};


// ============================================================
// EXPENSE FACTORY
// ============================================================
//
// IMPORTANT:
// We are NOT using make_unique() here.
//
// This works even with older compilers:
//
// unique_ptr<Expense>(new FoodExpense(...))
//
// ============================================================

unique_ptr<Expense> createExpense(
    const string &cat,
    int id,
    double amount,
    const string &date,
    const string &description,
    const string &extra
)
{
    if (cat == "Food")
    {
        return unique_ptr<Expense>(
            new FoodExpense(
                id,
                amount,
                date,
                description,
                extra
            )
        );
    }


    if (cat == "Travel")
    {
        return unique_ptr<Expense>(
            new TravelExpense(
                id,
                amount,
                date,
                description,
                extra
            )
        );
    }


    if (cat == "Shopping")
    {
        return unique_ptr<Expense>(
            new ShoppingExpense(
                id,
                amount,
                date,
                description,
                extra
            )
        );
    }


    if (cat == "Bills")
    {
        return unique_ptr<Expense>(
            new BillExpense(
                id,
                amount,
                date,
                description,
                extra
            )
        );
    }


    return unique_ptr<Expense>();
}


// ============================================================
// BUDGET MANAGEMENT
// ============================================================

class Budget
{
private:

    double monthlyLimit;


public:

    Budget()
        : monthlyLimit(0)
    {
    }


    bool isSet() const
    {
        return monthlyLimit > 0;
    }


    double getLimit() const
    {
        return monthlyLimit;
    }


    void setLimit(double limit)
    {
        monthlyLimit = limit;
    }


    // --------------------------------------------------------
    // Check budget status
    // --------------------------------------------------------

    void checkStatus(
        double monthTotal,
        const string &month
    ) const
    {
        if (!isSet())
            return;

        double percentage =
            monthTotal / monthlyLimit * 100.0;


        if (monthTotal > monthlyLimit)
        {
            cout
                << "\n!! BUDGET EXCEEDED !!\n"
                << "Month   : " << month << "\n"
                << "Spent   : " << fixed << setprecision(2)
                << monthTotal << "\n"
                << "Budget  : " << monthlyLimit << "\n";
        }
        else if (percentage >= 80.0)
        {
            cout
                << "\n! Budget Warning !\n"
                << fixed
                << setprecision(1)
                << percentage
                << "% of the budget for "
                << month
                << " is used.\n";
        }
    }


    // --------------------------------------------------------
    // Display budget
    // --------------------------------------------------------

    void display(
        double monthTotal,
        const string &month
    ) const
    {
        if (!isSet())
        {
            cout << "No budget set yet.\n";
            return;
        }


        cout
            << "\n===== BUDGET STATUS =====\n"

            << "Budget limit : "
            << fixed
            << setprecision(2)
            << monthlyLimit

            << "\nSpent ("
            << month
            << "): "
            << monthTotal

            << "\nRemaining    : "
            << monthlyLimit - monthTotal
            << "\n";


        checkStatus(monthTotal, month);
    }


    // --------------------------------------------------------
    // Save budget
    // --------------------------------------------------------

    bool saveToFile(const string &file) const
    {
        ofstream out(file.c_str());

        if (!out)
            return false;

        out << monthlyLimit << "\n";

        return true;
    }


    // --------------------------------------------------------
    // Load budget
    // --------------------------------------------------------

    void loadFromFile(const string &file)
    {
        ifstream in(file.c_str());

        if (in)
            in >> monthlyLimit;
    }
};


// ============================================================
// EXPENSE MANAGER
// ============================================================

class ExpenseManager
{
private:

    vector<unique_ptr<Expense> > expenses;

    int nextId;

    string dataFile;


    // --------------------------------------------------------
    // Find expense by ID
    // --------------------------------------------------------

    Expense *findById(int id)
    {
        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (expenses[i]->getId() == id)
                return expenses[i].get();
        }

        return NULL;
    }


    // --------------------------------------------------------
    // Print table header
    // --------------------------------------------------------

    static void printHeader()
    {
        cout
            << left
            << setw(5)
            << "ID"

            << setw(12)
            << "Date"

            << setw(10)
            << "Category"

            << right
            << setw(10)
            << "Amount"

            << "  "

            << left
            << setw(20)
            << "Description"

            << "Details\n";

        cout
            << string(80, '-')
            << "\n";
    }


public:

    explicit ExpenseManager(const string &file)
        : nextId(1),
          dataFile(file)
    {
    }


    // --------------------------------------------------------
    // Get all expenses
    // --------------------------------------------------------

    const vector<unique_ptr<Expense> > &getAll() const
    {
        return expenses;
    }


    // ========================================================
    // ADD EXPENSE
    // ========================================================

    Expense *addExpense(
        const string &category,
        double amount,
        const string &date,
        const string &description,
        const string &extra
    )
    {
        unique_ptr<Expense> expense =
            createExpense(
                category,
                nextId,
                amount,
                date,
                util::clean(description),
                util::clean(extra)
            );


        if (!expense)
            return NULL;


        nextId++;

        expenses.push_back(
            move(expense)
        );


        return expenses.back().get();
    }


    // ========================================================
    // UPDATE EXPENSE
    // ========================================================

    bool updateExpense(
        int id,
        double amount,
        const string &date,
        const string &description,
        const string &extra
    )
    {
        Expense *expense = findById(id);


        if (!expense)
            return false;


        expense->setAmount(amount);

        expense->setDate(date);

        expense->setDescription(
            util::clean(description)
        );

        expense->setExtra(
            util::clean(extra)
        );


        return true;
    }


    // ========================================================
    // DELETE EXPENSE
    // ========================================================

    bool deleteExpense(int id)
    {
        vector<unique_ptr<Expense> >::iterator it =
            find_if(
                expenses.begin(),
                expenses.end(),

                [id](const unique_ptr<Expense> &expense)
                {
                    return expense->getId() == id;
                }
            );


        if (it == expenses.end())
            return false;


        expenses.erase(it);

        return true;
    }


    // ========================================================
    // VIEW ALL
    // ========================================================

    void viewAll() const
    {
        if (expenses.empty())
        {
            cout << "No expenses recorded.\n";
            return;
        }


        printHeader();


        for (size_t i = 0; i < expenses.size(); i++)
        {
            expenses[i]->display();
        }
    }


    // ========================================================
    // SEARCH BY CATEGORY
    // ========================================================

    void searchByCategory(const string &category) const
    {
        search(
            [category](const Expense &expense)
            {
                return expense.getCategory() == category;
            }
        );
    }


    // ========================================================
    // SEARCH BY DATE
    // ========================================================

    void searchByDate(const string &date) const
    {
        search(
            [date](const Expense &expense)
            {
                return expense.getDate() == date;
            }
        );
    }


    // ========================================================
    // SEARCH BY KEYWORD
    // ========================================================

    void searchByKeyword(const string &keyword) const
    {
        string key = keyword;


        transform(
            key.begin(),
            key.end(),
            key.begin(),

            [](unsigned char c)
            {
                return static_cast<char>(tolower(c));
            }
        );


        search(
            [key](const Expense &expense)
            {
                string description =
                    expense.getDescription();


                transform(
                    description.begin(),
                    description.end(),
                    description.begin(),

                    [](unsigned char c)
                    {
                        return static_cast<char>(tolower(c));
                    }
                );


                return description.find(key)
                       != string::npos;
            }
        );
    }


    // ========================================================
    // GENERIC SEARCH
    // ========================================================

    template <typename Predicate>

    void search(Predicate match) const
    {
        bool found = false;


        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (match(*expenses[i]))
            {
                if (!found)
                    printHeader();


                expenses[i]->display();

                found = true;
            }
        }


        if (!found)
            cout << "No matching expenses found.\n";
    }


    // ========================================================
    // TOTAL EXPENSE
    // ========================================================

    double totalExpense() const
    {
        double total = 0;


        for (size_t i = 0; i < expenses.size(); i++)
        {
            total += expenses[i]->getAmount();
        }


        return total;
    }


    // ========================================================
    // MONTH TOTAL
    // ========================================================

    double monthTotal(const string &month) const
    {
        double total = 0;


        for (size_t i = 0; i < expenses.size(); i++)
        {
            if (
                util::monthKey(
                    expenses[i]->getDate()
                ) == month
            )
            {
                total += expenses[i]->getAmount();
            }
        }


        return total;
    }


    // ========================================================
    // CATEGORY TOTALS
    // ========================================================

    map<string, double> categoryTotals() const
    {
        map<string, double> totals;


        const vector<string> &categories =
            Category::all();


        for (size_t i = 0; i < categories.size(); i++)
        {
            totals[categories[i]] = 0;
        }


        for (size_t i = 0; i < expenses.size(); i++)
        {
            totals[
                expenses[i]->getCategory()
            ] += expenses[i]->getAmount();
        }


        return totals;
    }


    // ========================================================
    // MONTHLY TOTALS
    // ========================================================

    map<string, double> monthlyTotals() const
    {
        map<string, double> totals;


        for (size_t i = 0; i < expenses.size(); i++)
        {
            totals[
                util::monthKey(
                    expenses[i]->getDate()
                )
            ] += expenses[i]->getAmount();
        }


        return totals;
    }


    // ========================================================
    // SAVE TO FILE
    // ========================================================

    bool saveToFile() const
    {
        ofstream out(dataFile.c_str());


        if (!out)
            return false;


        for (size_t i = 0; i < expenses.size(); i++)
        {
            out
                << expenses[i]->serialize()
                << "\n";
        }


        return true;
    }


    // ========================================================
    // LOAD FROM FILE
    // ========================================================

    void loadFromFile()
    {
        ifstream in(dataFile.c_str());


        if (!in)
            return;


        string line;


        while (getline(in, line))
        {
            if (line.empty())
                continue;


            vector<string> p =
                util::split(line, '|');


            if (p.size() < 5)
                continue;


            try
            {
                int id = stoi(p[0]);

                double amount =
                    stod(p[2]);


                string extra = "";


                if (p.size() > 5)
                    extra = p[5];


                unique_ptr<Expense> expense =
                    createExpense(
                        p[1],
                        id,
                        amount,
                        p[3],
                        p[4],
                        extra
                    );


                if (!expense)
                    continue;


                if (id >= nextId)
                    nextId = id + 1;


                expenses.push_back(
                    move(expense)
                );
            }
            catch (...)
            {
                // Ignore corrupted records
                continue;
            }
        }
    }
};


// ============================================================
// REPORT MODULE
// ============================================================

class Report
{
private:

    const ExpenseManager &manager;


public:

    explicit Report(const ExpenseManager &m)
        : manager(m)
    {
    }


    // ========================================================
    // SUMMARY
    // ========================================================

    void summary() const
    {
        cout
            << "\n===== EXPENSE SUMMARY =====\n"

            << "Total records : "
            << manager.getAll().size()

            << "\nTotal spending: "
            << fixed
            << setprecision(2)
            << manager.totalExpense()

            << "\n";
    }


    // ========================================================
    // CATEGORY-WISE REPORT
    // ========================================================

    void categoryWise() const
    {
        double total =
            manager.totalExpense();


        cout
            << "\n===== CATEGORY-WISE REPORT =====\n";


        string highestCategory;

        double highestAmount = -1;


        map<string, double> totals =
            manager.categoryTotals();


        map<string, double>::const_iterator it;


        for (
            it = totals.begin();
            it != totals.end();
            ++it
        )
        {
            string category = it->first;

            double amount = it->second;


            double percentage = 0;


            if (total > 0)
            {
                percentage =
                    amount / total * 100.0;
            }


            cout
                << left
                << setw(10)
                << category

                << right
                << setw(10)
                << fixed
                << setprecision(2)
                << amount

                << "  ("
                << fixed
                << setprecision(1)
                << percentage
                << "%)\n";


            if (amount > highestAmount)
            {
                highestAmount = amount;
                highestCategory = category;
            }
        }


        if (total > 0)
        {
            cout
                << "\nHighest spending category: "
                << highestCategory
                << "\n";
        }
    }


    // ========================================================
    // MONTHLY REPORT
    // ========================================================

    void monthly() const
    {
        cout
            << "\n===== MONTHLY REPORT =====\n";


        map<string, double> totals =
            manager.monthlyTotals();


        if (totals.empty())
        {
            cout << "No data.\n";
            return;
        }


        map<string, double>::const_iterator it;


        for (
            it = totals.begin();
            it != totals.end();
            ++it
        )
        {
            cout
                << it->first
                << "  "
                << fixed
                << setprecision(2)
                << it->second
                << "\n";
        }
    }
};


// ============================================================
// MAIN APPLICATION
// ============================================================

class ExpenseTrackerApp
{
private:

    User user;

    Budget budget;

    ExpenseManager manager;

    Report report;


    // --------------------------------------------------------
    // Select category
    // --------------------------------------------------------

    string pickCategory() const
    {
        while (true)
        {
            cout
                << "\nSelect category:\n";


            Category::showMenu();


            int choice =
                util::readInt("Choice: ");


            if (
                choice >= 1 &&
                choice <=
                static_cast<int>(
                    Category::all().size()
                )
            )
            {
                return Category::all()[
                    choice - 1
                ];
            }


            cout
                << "  Invalid choice.\n";
        }
    }


    // --------------------------------------------------------
    // Extra information prompt
    // --------------------------------------------------------

    static string extraPrompt(
        const string &category
    )
    {
        if (category == "Food")
        {
            return
                "Meal type "
                "(Breakfast/Lunch/Dinner/Snack): ";
        }


        if (category == "Travel")
        {
            return
                "Mode of transport: ";
        }


        if (category == "Shopping")
        {
            return
                "Shop name: ";
        }


        return
            "Bill type "
            "(Electricity/Water/Phone/...): ";
    }


    // ========================================================
    // ADD EXPENSE
    // ========================================================

    void addExpense()
    {
        string category =
            pickCategory();


        double amount =
            util::readPositiveDouble(
                "Amount: "
            );


        string date =
            util::readDate(
                "Date (DD-MM-YYYY): "
            );


        string description =
            util::readLine(
                "Description: "
            );


        string extra =
            util::readLine(
                extraPrompt(category)
            );


        Expense *expense =
            manager.addExpense(
                category,
                amount,
                date,
                description,
                extra
            );


        if (expense)
        {
            cout
                << "\nExpense added successfully!\n";


            string month =
                util::monthKey(date);


            budget.checkStatus(
                manager.monthTotal(month),
                month
            );
        }
        else
        {
            cout
                << "Failed to add expense.\n";
        }
    }


    // ========================================================
    // UPDATE EXPENSE
    // ========================================================

    void updateExpense()
    {
        int id =
            util::readInt(
                "Enter expense ID to update: "
            );


        double amount =
            util::readPositiveDouble(
                "New amount: "
            );


        string date =
            util::readDate(
                "New date (DD-MM-YYYY): "
            );


        string description =
            util::readLine(
                "New description: "
            );


        string extra =
            util::readLine(
                "New detail "
                "(meal/mode/shop/bill): "
            );


        bool success =
            manager.updateExpense(
                id,
                amount,
                date,
                description,
                extra
            );


        if (success)
        {
            cout
                << "Expense updated successfully.\n";
        }
        else
        {
            cout
                << "ID not found.\n";
        }
    }


    // ========================================================
    // DELETE EXPENSE
    // ========================================================

    void deleteExpense()
    {
        int id =
            util::readInt(
                "Enter expense ID to delete: "
            );


        bool success =
            manager.deleteExpense(id);


        if (success)
        {
            cout
                << "Expense deleted successfully.\n";
        }
        else
        {
            cout
                << "ID not found.\n";
        }
    }


    // ========================================================
    // SEARCH MENU
    // ========================================================

    void searchMenu()
    {
        cout
            << "\n===== SEARCH EXPENSES =====\n"

            << "1. Search by Category\n"
            << "2. Search by Date\n"
            << "3. Search by Keyword\n";


        int choice =
            util::readInt("Choice: ");


        if (choice == 1)
        {
            manager.searchByCategory(
                pickCategory()
            );
        }
        else if (choice == 2)
        {
            string date =
                util::readDate(
                    "Date (DD-MM-YYYY): "
                );


            manager.searchByDate(date);
        }
        else if (choice == 3)
        {
            string keyword =
                util::readLine(
                    "Keyword: "
                );


            manager.searchByKeyword(
                keyword
            );
        }
        else
        {
            cout
                << "Invalid choice.\n";
        }
    }


    // ========================================================
    // BUDGET MENU
    // ========================================================

    void budgetMenu()
    {
        cout
            << "\n===== BUDGET MANAGEMENT =====\n"

            << "1. Set/Change monthly budget\n"
            << "2. View budget status\n";


        int choice =
            util::readInt("Choice: ");


        if (choice == 1)
        {
            double limit =
                util::readPositiveDouble(
                    "Monthly budget limit: "
                );


            budget.setLimit(limit);


            cout
                << "Budget saved successfully.\n";
        }
        else if (choice == 2)
        {
            string month =
                util::readLine(
                    "Month (MM-YYYY): "
                );


            budget.display(
                manager.monthTotal(month),
                month
            );
        }
        else
        {
            cout
                << "Invalid choice.\n";
        }
    }


    // ========================================================
    // REPORT MENU
    // ========================================================

    void reportMenu()
    {
        cout
            << "\n===== REPORTS =====\n"

            << "1. Summary\n"
            << "2. Category-wise\n"
            << "3. Monthly\n"
            << "4. All Reports\n";


        int choice =
            util::readInt("Choice: ");


        if (choice == 1 || choice == 4)
        {
            report.summary();
        }


        if (choice == 2 || choice == 4)
        {
            report.categoryWise();
        }


        if (choice == 3 || choice == 4)
        {
            report.monthly();
        }


        if (
            choice < 1 ||
            choice > 4
        )
        {
            cout
                << "Invalid choice.\n";
        }
    }


    // ========================================================
    // SAVE EVERYTHING
    // ========================================================

    void saveAll() const
    {
        manager.saveToFile();

        budget.saveToFile(
            "budget.txt"
        );

        user.saveToFile(
            "user.txt"
        );
    }


public:

    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    ExpenseTrackerApp()
        : manager("expenses.txt"),
          report(manager)
    {
    }


    // ========================================================
    // RUN APPLICATION
    // ========================================================

    void run()
    {
        // Load user
        if (!user.loadFromFile("user.txt"))
        {
            cout
                << "\n===== USER REGISTRATION =====\n";

            user.registerUser();
        }


        // Load budget
        budget.loadFromFile(
            "budget.txt"
        );


        // Load expenses
        manager.loadFromFile();


        // ----------------------------------------------------
        // Main application
        // ----------------------------------------------------

        cout
            << "\n====================================\n"
            << "        EXPENSE TRACKER\n"
            << "====================================\n";


        user.display();


        while (true)
        {
            cout
                << "\n====================================\n"
                << "             MAIN MENU\n"
                << "====================================\n"

                << "1. Add expense\n"
                << "2. View all expenses\n"
                << "3. Update expense\n"
                << "4. Delete expense\n"
                << "5. Search expenses\n"
                << "6. Budget management\n"
                << "7. Reports\n"
                << "0. Save & Exit\n"

                << "====================================\n";


            int choice =
                util::readInt(
                    "Enter choice: "
                );


            switch (choice)
            {
                case 1:
                    addExpense();
                    break;


                case 2:
                    manager.viewAll();
                    break;


                case 3:
                    updateExpense();
                    break;


                case 4:
                    deleteExpense();
                    break;


                case 5:
                    searchMenu();
                    break;


                case 6:
                    budgetMenu();
                    break;


                case 7:
                    reportMenu();
                    break;


                case 0:

                    saveAll();

                    cout
                        << "\nData saved successfully.\n"
                        << "Thank you for using Expense Tracker!\n";

                    return;


                default:

                    cout
                        << "Invalid choice. "
                        << "Please try again.\n";
            }
        }
    }
};


// ============================================================
// MAIN FUNCTION
// ============================================================

int main()
{
    ExpenseTrackerApp app;

    app.run();

    return 0;
}

