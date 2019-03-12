/*
GRAMMAR OF THE PROGRAM

	program:
		END							// END is end-of-input
		expr_list END

	expr_list:
		expression PRINT			// PRINT is semicolon
		expression PRINT expr_list

	expression:
		expression + term
		expression - term
		term

	term:
		term / primary
		term * primary
		primary

	primary:
		NUMBER
		NAME
		NAME = expression
		- primary
		( expression )
*/

#include <string>
#include <map>
#include <iostream>

enum Token_value {
	NAME,			NUMBER,			END,
	PLUS = '+',		MINUS = '-',	MUL = '*',	DIV = '/',
	PRINT = ';',	ASSIGN = '=',	LP = '(',	RP = ')'
};

Token_value curr_tok = PRINT;
double number_value;
std::string string_value;
std::map<std::string, double> table;
int no_of_errors;

double term(bool);
double prim(bool);
Token_value get_token();

double error(const std::string& s)
{
	no_of_errors++;
	std::cerr << "error: " << s << std::endl;
	return 1;
}

double expr(bool get)	// add and subtract
{
	double left = term(get);

	for (;;)	// forever
		switch (curr_tok) {
		case PLUS:
			left += term(true);
			break;
		case MINUS:
			left -= term(true);
			break;
		default:
			return left;
		}
}

double term(bool get)	// multiply and divide
{
	double left = prim(get);

	for (;;)	//forever
		switch (curr_tok) {
		case MUL:
			left *= term(true);
			break;
		case DIV:
			if (double d = prim(true)) {
				left /= d;
				break;
			}
			return error("divide by 0");
		default:
			return left;
		}
}

double prim(bool get)	// handle primaries
{
	if (get)
		get_token();

	switch (curr_tok) {
	case NUMBER:
	{
		double v = number_value;
		get_token();
		return v;
	}
	case NAME:
	{
		double& v = table[string_value];
		if (get_token() == ASSIGN)
			v = expr(true);
		return v;
	}
	case MINUS:		// unary minus
		return -prim(true);
	case LP:
	{
		double e = expr(true);
		if (curr_tok != RP)
			return error(") expected");
		get_token();	// eat ')'
		return e;
	}
	default:
		return error("primary expected");
	}
}

Token_value get_token()
{
	char ch = 0;
	
	do {	// skip whitespace except '\n'
		if (!std::cin.get(ch))
			return curr_tok = END;
	} while (ch != '\n' && isspace(ch));

	switch (ch) {
	case 0:
		return curr_tok = END;
	case ';':
	case '\n':
		return curr_tok = PRINT;
	case '*':
	case '/':
	case '+':
	case '-':
	case '(':
	case ')':
	case '=':
		return curr_tok = Token_value(ch);
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.':
		std::cin.putback(ch);
		std::cin >> number_value;
		return curr_tok = NUMBER;
	default:		// NAME, NAME =, or error
		if (isalpha(ch)) {
			string_value = ch;
			while (std::cin.get(ch) && isalnum(ch))
				string_value.push_back(ch);
			std::cin.putback(ch);
			return curr_tok = NAME;
		}
		error("bad token");
		return curr_tok = PRINT;
	}
}

int main()
{
	table["pi"] = 3.1415926535897935385;	// insert predefined names
	table["e"] = 2.7182818284590452354;

	while (std::cin) {
		get_token();
		if (curr_tok == END)
			break;
		if (curr_tok == PRINT)
			continue;
		std::cout << expr(false) << std::endl;
	}

	std::cin.get();
	return no_of_errors;
}