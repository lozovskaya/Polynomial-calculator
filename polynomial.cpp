#include <bits/stdc++.h>

using namespace std;

void err(const string &e = "") {  // print message about error
    cout << "ERR " + e << endl;
}

// polynomial is realized using self-made list
struct Elem {  // element of the list
    long long number;
    long long degree;
    long long k;
    struct Elem *next = nullptr;
};

struct Monomial {  // e.g. 2x^5, k = 2, degree = 5, variable = 'x'
    long long k = 0;
    long long degree = 0;
    char variable = 'A';
};


bool check_monomial(const string &candidate) {  // check whether monomial is correct
    for (const char &symbol : candidate) {  // the string contains correct symbols
        if (symbol >= 'a' && symbol <= 'z') {
            continue;
        }
        if (symbol >= '0' && symbol <= '9') {
            continue;
        }
        if (symbol == '-' || symbol == '+' || symbol == '^') {
            continue;
        }
        return false;
    }
    /* [+/-](0)[100](1){x}(2)[^100](3) - how monomial can look like
     * [...] and {...} - part of monomial
     * (number) - number of part
    */
    int stage = -1;  // on which part of monomial we stay
    for (const char &symbol : candidate) {
        if (symbol == '-' || symbol == '+') {
            if (stage == -1) {
                ++stage;
            } else {
                return false;
            }
            continue;
        }
        if (symbol == '^') {
            if (stage != 2) {
                return false;
            }
            stage = 3;
            continue;
        }
        if (symbol >= '0' && symbol <= '9') {
            if (stage == -1 || stage == 0 || stage == 3) {
                stage = max(stage, 0) + 1;
                continue;
            }
            if (stage == 1 || stage == 4) {
                continue;
            }
            return false; // stage = 2
        }
        if (symbol >= 'a' && symbol <= 'z') {
            if (stage > 1) {
                return false;
            }
            stage = 2;
            continue;
        }
    }
    return stage == 1 || stage == 2 || stage == 4;
}


Monomial return_monomial(const string &m) {  // convert string to monomial
    Monomial new_one;
    int sign = (m[0] == '-' ? -1 : 1);
    int i = ((m[0] == '-' || m[0] == '+') ? 1 : 0);
    while (i < (int) m.size() && m[i] >= '0' && m[i] <= '9') {
        new_one.k *= 10;
        new_one.k += m[i] - '0';
        ++i;
    }
    if (i == 0 || (i == 1 && (m[0] == '+' || m[0] == '-'))) {
        new_one.k = 1;
    }
    if (i < (int) m.size()) {
        new_one.variable = m[i];
        i += 2;
        if (i >= (int) m.size()) {
            new_one.degree = 1;
        }
    }
    while (i < (int) m.size() && m[i] >= '0' && m[i] <= '9') {
        new_one.degree *= 10;
        new_one.degree += m[i] - '0';
        ++i;
    }
    if (new_one.degree == 0 || new_one.k == 0) {
        new_one.variable = 'A';
    }
    new_one.k *= sign;
    return new_one;
}


void add_monomial(Elem *&p, const Monomial &monomial) {
    if (monomial.k == 0 && monomial.degree != 0) {
        return;
    }
    if (!p) {
        p = new Elem;
        p->degree = monomial.degree;
        p->k = monomial.k;
        p->next = nullptr;
        return;
    }
    auto iterator = p;
    while (iterator) {
        if (iterator->degree == monomial.degree) {
            iterator->k += monomial.k;
            if (iterator->k == 0) {
                if (iterator->degree != 0) {
                    auto k = iterator->next;
                    delete iterator;
                    *iterator = *(k);
                }
            }
            return;
        }
        iterator = iterator->next;
    }
    Elem *new_monomial = new Elem;
    new_monomial->degree = monomial.degree;
    new_monomial->k = monomial.k;
    iterator = p;
    if (monomial.degree > iterator->degree) {  // in the start
        new_monomial->next = iterator;
        p = new_monomial;
        return;
    }
    while (iterator) {
        if (iterator->next && iterator->degree > monomial.degree && iterator->next->degree < monomial.degree) {
            new_monomial->next = iterator->next;
            iterator->next = new_monomial;
            return;
        }
        if (!iterator->next) {
            iterator->next = new_monomial;
            new_monomial->next = nullptr;
            return;
        }
        iterator = iterator->next;
    }
}


Elem *return_polynomial(const string &s) { // convert string to polynomial
    int i = 0;
    Elem *polynomial = nullptr;
    string mon;
    if (s[0] == '-') {
        mon = s[0];
        ++i;
    }
    while (i < (int) s.size()) {
        while (i < (int) s.size() && s[i] != '.' && s[i] != '-' && s[i] != '+') {
            mon += s[i];
            ++i;
        }
        add_monomial(polynomial, return_monomial(mon));
        if (i < (int) s.size()) {
            mon = s[i];
        }
        ++i;
    }
    return polynomial;
}

bool check_polynomial(vector<Elem *> &v, int n, const string &s) {  // check whether string is polynomial and save it in vector
    int i = 0;
    string mon;
    if (s[0] == '-') {
        mon = s[0];
        ++i;
    }
    char variable = 'A';
    while (i < (int) s.size()) {
        while (i < (int) s.size() && s[i] != '.' && s[i] != '-' && s[i] != '+') {
            mon += s[i];
            ++i;
        }
        if (!check_monomial(mon)) {
            return false;
        }
        char new_let = return_monomial(mon).variable;
        if (variable == 'A') {
            variable = new_let;
        } else {
            if (new_let != variable && new_let != 'A') {
                return false;
            }
        }
        if (i < (int) s.size()) {
            mon = s[i];
        }
        ++i;
    }
    auto ne = return_polynomial(s);
    ne->number = n;
    v.push_back(ne);
    return true;
}


void print_monomial(const Elem *&monomial) {  // print element of the list
    if (monomial->k == -1 && monomial->degree != 0) {
        cout << '-';
    } else {
        if ((monomial->k != 0 && monomial->k != 1) || (abs(monomial->k) == 1 && monomial->degree == 0)) {
            cout << monomial->k;
        }
    }
    if (monomial->degree != 0) {
        cout << "x";
        if (monomial->degree != 1) {
            cout << "^" << monomial->degree;
        }
    }
}

void print_polynomial(const Elem *polynomial) {  // print polynomial
    if (!polynomial) {
        err("polynomial doesn't exist");
        return;
    }
    if (polynomial->degree == 0 && polynomial->k == 0) {
        cout << "0.\n";
        return;
    }
    auto iterator = polynomial;
    print_monomial(iterator);
    iterator = iterator->next;
    while (iterator) {
        if (iterator->k > 0) {
            cout << '+';
        }
        print_monomial(iterator);
        iterator = iterator->next;
    }
    cout << '.' << endl;
}


bool sort_by_numbers(const Elem *a, const Elem *b) {
    return (a->number) < (b->number);
}


bool is_equal(const Elem *p1, const Elem *p2) {  // true if polynomials are equal
    auto iterator1 = p1, iterator2 = p2;
    while (iterator1 && iterator2) {
        if (iterator1->degree != iterator2->degree) {
            return false;
        }
        if (iterator1->k != iterator2->k) {
            return false;
        }
        iterator1 = iterator1->next;
        iterator2 = iterator2->next;
    }
    return !iterator1 && !iterator2;
}


void delete_polynomial(Elem *&iterator) {
    while (iterator) {
        auto k = iterator->next;
        delete iterator;
        iterator = k;
    }
}

// flag = 0 - do nothing, flag = 1 - delete, flag = 2 - print it
bool find_elem(vector<Elem *> &v, int n, int flag = 0) {
    for (int i = 0; i < (int) v.size(); ++i) {
        auto polym = v[i];
        if (polym->number == n) {
            if (flag == 1) {
                delete_polynomial(v[i]);
                v.erase(v.begin() + i);
            }
            if (flag == 2) {
                print_polynomial(polym);
            }
            return true;
        }
    }
    return false;
}


void differentiation(Elem *&polynomial) {
     auto iterator = polynomial;
     while (iterator) {
         if (iterator->next && iterator->next->degree == 0) {
            iterator->next = nullptr;
            break;
         } else {
            iterator->k *= iterator->degree;
            --iterator->degree;
         }
         iterator = iterator->next;
     }
}



int main() {
    ios_base::sync_with_stdio(0);
#ifdef iLOCAL
    freopen("input.txt", "r", stdin);
#endif
    string command;
    vector<Elem *> v;  // all polynomials are here
    while (cin >> command) {
        if (command == "REM") {
            char c;
            while (cin.get(c)) {
                if (c == '\n') {
                    break;
                }
            }
            continue;
        }
        if (command == "READ") {
            int n;
            cin >> n;
            string s;
            cin >> s;
            bool found = find_elem(v, n);
            if (!found) {
                if (check_polynomial(v, n, s)) {
                    cout << "OK\n";
                } else {
                    err("trouble while reading: wrong polynomial");
                }
            } else {
                err("this index is used");
            }
            continue;
        }
        if (command == "DEL") {
            int n;
            cin >> n;
            bool found = find_elem(v, n, 1);
            if (!found) {
                err("can't delete: no string here");
            } else {
                cout << "OK\n";
            }
            continue;
        }
        if (command == "PRINT") {
            int n;
            cin >> n;
            bool found = find_elem(v, n, 2);
            if (!found) {
                err("can't print: no string here");
            }
            continue;
        }
        if (command == "EQ") {
            int n, m;
            cin >> n >> m;
            Elem *p1 = nullptr, *p2 = nullptr;
            for (auto x : v) {
                if (x->number == n) {
                    p1 = x;
                }
                if (x->number == m) {
                    p2 = x;
                }
            }
            if (!p1 || !p2) {
                err("polynomial doesn't exist");
                continue;
            }
            if (is_equal(p1, p2)) {
                cout << "TRUE\n";
            } else {
                cout << "FALSE\n";
            }
            continue;
        }
        if (command == "DIFF") {
            int n;
            cin >> n;
            bool found = find_elem(v, n);
            if (!found) {
                err("no polynomial here");
            } else {
                for (int i = 0; i < (int) v.size(); ++i) {
                    if (v[i]->number == n) {
                        differentiation(v[i]);
                        cout << "OK\n";
                        break;
                    }
                }
            }
            continue;
        }
        if (command == "EXIT") {
            sort(v.begin(), v.end(), sort_by_numbers);
            for (auto x : v) {
                cout << x->number << ": ";
                print_polynomial(x);
            }
            return 0;
        }
    }
    return 0;
}
