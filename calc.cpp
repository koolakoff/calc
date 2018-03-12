#include "std_lib_facilities.h"


/* ----------------- Types ----------------- */
class Token
{
public:
    char kind;
    double value;
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

/* ----------------- Global variables ----------------- */
Token_stream ts;

/* -----------------  Constants ----------------- */
const char quit = 'q';
const char print = ';';
const string prompt = "> ";
const string result = "= ";

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
        case '(': case ')': case '-': case '+': case '*': case '/': case '%':
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
        double d = expression();
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
    cout << "  enter ';' to calculate;\n";
    cout << "Enter 'q' to exit\n";
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
