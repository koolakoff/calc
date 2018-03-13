#include "std_lib_facilities.h"


/* ----------------- Types ----------------- */
class Token
{
public:
    char kind;
    double value;
    string name;

    Token() {}
    Token(char ch): kind{ch} {}
    Token(char ch, double val): kind{ch}, value{val} {}
    Token(char ch, string str): kind{ch}, name{str} {}
};

class Token_stream
{
public:
    Token get();
    void putback(Token t);
    void ignore(char c);
private:
    bool full {false};
    Token buffer;
};

class Variable
{
public:
    string name;
    double value;
};

/* ----------------- Global variables ----------------- */
Token_stream ts;
vector<Variable> var_table;

/* -----------------  Constants ----------------- */
const char quit = 'q';
const char print = ';';
const char let = 'L';
const char name = 'a';
const string prompt = "> ";
const string result = "= ";
const string declkey = "let";

/* ----------------- Class Methods ----------------- */
///////  Token_stream::get
Token Token_stream::get()
{
    if (full)
    {
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;

    switch (ch)
    {
        case print:         // calculate
        case quit:          // exit
        case '(': case ')': case '-': case '+': case '*': case '/': case '%': case '=':
            return Token{ch};
        case '.': case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        {
            cin.putback(ch);
            double val;
            cin >> val;
            return Token{'8', val};
        }
        default:
            if (isalpha(ch))
            {
                string s = string{ch};
                while (cin.get(ch) &&
                    (isalpha(ch) || isdigit(ch))) s+=ch;
                cin.putback(ch);
                if (s == declkey)
                    return Token(let);
                return Token(name,s);
            }
            error ("wrong token " + string{ch});
    }
    return Token{'8', 1};
}

///////  Token_stream::putback
void Token_stream::putback(Token t)
{
    if (full) error("Buffer full");
    buffer = t;
    full = true;
}

///////  Token_stream::ignore
// clears all symbols up to specified (including specified)
void Token_stream::ignore (char c)
{
    if (full && c==buffer.kind)
    {
        full = false;
        return;
    }
    full = false;

    char ch = 0;
    while (cin >> ch)
        if (ch==c) return;
}

/* ----------------- Functions ----------------- */
double expression();

//    get_value
double get_value(string s)
{
    for (const Variable& v: var_table)
        if (v.name == s) return v.value;
    error("get: unknown variable ", s);
}

//    set_value
void set_value(string s, double val)
{
    for (Variable& v: var_table)
        if (v.name == s)
        {
            v.value = val;
            return;
        }
    error("set: unknown variable ", s);
}

//    is_declared
bool is_declared (string var)
{
    for (const Variable& v: var_table)
        if (v.name == var) return true;
    return false;
}

//   define_name
double define_name (string var, double val)
{
    // overwrite value if variable exists
    if (is_declared(var)) set_value (var, val);
    var_table.push_back(Variable{var,val});
    return val;
}

//   primary
double primary()
{
    Token t = ts.get();
    switch (t.kind)
    {
        case '(':
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')')
            {
                error ("Missing ')'");
            }
            return d;
        }
        case '8':
            return t.value;
        case '-':
            return -primary();
        case '+':
            return primary();
        case name:
            return get_value(t.name);
        default:
            error ("Missing initial expression, t=" + string{t.kind});
    }
}

//   term
double term ()
{
    double left = primary();
    Token t = ts.get();
    while (true)
    {
        switch (t.kind)
        {
            case '*':
                left *= primary();
                t = ts.get();
                break;
            case '/':
            {
                double d = primary();
                if (d == 0)
                {
                    error ("Division by 0");
                }
                left /= d;
                t = ts.get();
                break;
            }
            case '%':
            {
                double d = primary();
                if (d == 0)
                {
                    error ("Division by 0");
                }
                left = fmod(left,d);
                t = ts.get();
                break;
            }
            default:
                ts.putback(t);
                return left;
        }
    }
}

//   expression
double expression()
{
    double left = term();
    Token t = ts.get();
    while (true)
    {
        switch (t.kind)
        {
            case '+':
                left += term();
                t = ts.get();
                break;
            case '-':
                left -= term();
                t = ts.get();
                break;
            default:
                ts.putback(t);
                return left;
        }
    }
}

double declaration()
{
    // Suppose keyword 'let' is already parsed
    // handle 'name = expression'
    Token t = ts.get();
    if (t.kind != name)
        error ("variable name expected");
    string var_name = t.name;

    Token t2 = ts.get();
    if (t2.kind != '=')
        error ("expected '='");

    double d = expression();
    define_name (var_name, d);
    return d;
}

//   statement
double statement()
{
    Token t = ts.get();
    switch (t.kind)
    {
    case let:
        return declaration();
    default:
        ts.putback(t);
        return expression();
    }
}

//   calculate
void calculate ()
{
    while (cin)
    try
    {
        cout << prompt;
        Token t = ts.get();
        while (t.kind == print) t=ts.get(); // ignore all `;`
        if (t.kind == quit) return;
        ts.putback(t);
        double d = statement();
        cout << result << d << "\n";
    }
    catch (exception& e)
    {
        cerr << e.what() << "\n";
        ts.ignore(print);
    }
}

/* ----------------- main ----------------- */
int main()
{
    cout << "Welcom to the Calc\n";
    cout << "Please enter expression using digits and operators +-*/% and braces()\n";
    cout << "  or enter variable using 'let varname = expression'\n";
    cout << "  enter '" << print << "' to calculate;\n";
    cout << "Enter '" << quit << "' to exit\n";
    try
    {
        calculate();
    }
    catch (exception& e)
    {
        cerr << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        cerr << "unknown exception\n";
        return 1;
    }
    return 0;
}
