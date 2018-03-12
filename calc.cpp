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
private:
    bool full {false};
    Token buffer;
};

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
        case ';':         // calculate
        case 'q':         // exit
        case '(': case ')': case '-': case '+': case '*': case '/':
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

/* ----------------- Global variables ----------------- */
Token_stream ts;

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
        default:
            error ("Missing initial expression");
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

/* ----------------- main ----------------- */
int main()
{
    cout << "Welcom to the Calc\n";
    cout << "Please enter expression using digits and operators +-*/ and braces()\n";
    cout << "  enter ';' to calculate;\n";
    cout << "Enter 'q' to exit\n";
    try
    {
        double val = 0;
        while (cin)
        {
            Token t = ts.get();
            if (t.kind == 'q') break;
            if (t.kind == ';')
            {
                cout << " = " << val << "\n";
            }
            else
                ts.putback(t);
            val = expression();
        }
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
