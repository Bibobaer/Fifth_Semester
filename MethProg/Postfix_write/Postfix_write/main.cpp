#include <iostream> 
#include <vector> 
#include <algorithm> 
#include <string> 
#include <map>
#include <stack>
#include <functional>
#include <cmath>

enum class lexem_t { number, var, op_br, cl_br, op, fun, comma, ternary_q, ternary_c};

std::map<std::string, int> operation_priorities = {
    {"^", 55}, {"fack", 55},
    {"!", 50}, {"uno", 50},
    {"*", 45}, {"/", 45},
    {"+", 40}, {"-", 40},
    {">>", 35}, {"<<", 35},
    {">=", 30}, {"<=", 30}, {">", 30}, {"<", 30},
    {"==", 25}, {"!=", 25},
    {"&", 20},
    {"|", 15},
    {"&&", 10},
    {"||", 5},
    {"?:", 0}
};

std::map<std::string, std::function<void(std::stack<std::string>&)>> functions = {
    {"^", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(std::pow(b, a)));
    }},
    {"fack", [](std::stack<std::string>& stack) {
        int n = std::stoi(stack.top()); stack.pop();
        int i = 1;
        int fack = 1;
        while (i <= n) {
            fack *= i++;
        }
        stack.push(std::to_string(fack));
    }},
    {"!", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(!a));
    }},
    {"uno", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(-a));
    }},
    {"*", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(a * b));
    }},
    {"/", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b / a));
    }},
    {"+", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(a + b));
    }},
    {"-", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b - a));
    }},
    {">>", [](std::stack<std::string>& stack) {
        int a = std::stoi(stack.top()); stack.pop();
        int b = std::stoi(stack.top()); stack.pop();
        stack.push(std::to_string(b >> a));
    }},
    {"<<", [](std::stack<std::string>& stack) {
        int a = std::stoi(stack.top()); stack.pop();
        int b = std::stoi(stack.top()); stack.pop();
        stack.push(std::to_string(b << a));
    }},
    {">=", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b >= a));
    }},
    {"<=", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b <= a));
    }},
    {">", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b > a));
    }},
    {"<", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b < a));
    }},
    {"==", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string((b - a) < 1e-9));
    }},
    {"!=", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string((b - a) >= 1e-9));
    }},
    {"&", [](std::stack<std::string>& stack) {
        int a = std::stoi(stack.top()); stack.pop();
        int b = std::stoi(stack.top()); stack.pop();
        stack.push(std::to_string(b & a));
    }},
    {"|", [](std::stack<std::string>& stack) {
        int a = std::stoi(stack.top()); stack.pop();
        int b = std::stoi(stack.top()); stack.pop();
        stack.push(std::to_string(b | a));
    }},
    {"&&", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b && a));
    }},
    {"||", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b || a));
    }},
    {"?:", [](std::stack<std::string>& stack) {
        double false_op = std::stod(stack.top()); stack.pop();
        double true_op = std::stod(stack.top()); stack.pop();
        bool compare = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(compare ? true_op : false_op));
    }},
    {"sin", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(std::sin(a)));
    }},
    {"cos", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(std::cos(a)));
    }},
    {"exp", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(std::exp(a)));
    }},
    {"func", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(b + 2*a));
    }},
    {"func1", [](std::stack<std::string>& stack) {
        double a = std::stod(stack.top()); stack.pop();
        double b = std::stod(stack.top()); stack.pop();
        stack.push(std::to_string(2*b - a));
    }},

};

//std::map<std::string, std::function<int(std::stack<std::string>&)>> operation_calc = {
//    {"^", [](std::stack<std::string>& stack){ 
//        
//        return 1;
//    }}
//};

struct lexem {
    std::string name;
    lexem_t type;
};

bool is_multisymbol_opertion(const std::string& str, size_t pos) {
    if (pos + 1 >= str.length()) return false;

    std::string multisymbol = str.substr(pos, 2);

    return operation_priorities.contains(multisymbol);
}

std::vector<lexem> parse_expr_to_lexems(const std::string& expr) {
    std::vector<lexem> res;
    auto it = expr.begin();
    while (it != expr.end()) {
        if (std::isspace(*it)) { ++it; continue; }

        auto pos = it - expr.begin();
        if (is_multisymbol_opertion(expr, pos)) {
            res.push_back({expr.substr(pos, 2), lexem_t::op});
            it += 2;
            continue;
        }
        if (*it == '?') {
            res.push_back({"?:", lexem_t::ternary_q});
            it++;
            continue;
        }
        if (*it == ':') {
            res.push_back({ ":", lexem_t::ternary_c });
            it++;
            continue;
        }

        auto last = std::find_if(it, expr.end(), [](auto c) {return !std::isalnum(c) and c != '.'; });
        if (it == last) ++last;
        res.push_back({ std::string(it, last), lexem_t::op });
        it = last;
    }
    for (size_t i = 0; i < res.size(); i++) {
        auto c = res[i].name[0];
        if (std::isdigit(c)) res[i].type = lexem_t::number;
        else if (c == '(') res[i].type = lexem_t::op_br;
        else if (c == ')') res[i].type = lexem_t::cl_br;
        else if (c == ',') res[i].type = lexem_t::comma;
        else if (std::isalpha(c)) {
            if (i + 1 < res.size() and res[i + 1].name == "(") res[i].type = lexem_t::fun;
            else res[i].type = lexem_t::var;
        }
        else if (c == '-') {
            if (i == 0 || res[i-1].type == lexem_t::op_br) res[i].name = "uno";
        }
        else if (c == '!') {
            if (i > 0 && (res[i].name.size() != 2) && (res[i - 1].type == lexem_t::number || res[i - 1].type == lexem_t::var || res[i - 1].type == lexem_t::cl_br)) res[i].name = "fack";
        }
    }
    return res;
}

std::vector<lexem> create_postfix_write(const std::vector<lexem>& terms) {
    std::vector<lexem> out;
    std::stack<lexem> S;
    
    for (auto& term : terms) {
        switch (term.type) {
            case lexem_t::number:
            case lexem_t::var:
                out.push_back(term);
                break;
            case lexem_t::cl_br:
                while (S.size() && S.top().type != lexem_t::op_br) {
                    out.push_back(std::move(S.top()));
                    S.pop();
                }
                S.pop();
                break;
            case lexem_t::op:
                while (S.size() && (S.top().type == lexem_t::fun || 
                        (S.top().type == lexem_t::op && 
                         operation_priorities[S.top().name] >= operation_priorities[term.name]))) {
                    out.push_back(std::move(S.top()));
                    S.pop();
                }
                S.push(term);
                break;
                break;
            case lexem_t::comma:
                while (S.size() && S.top().type != lexem_t::op_br) {
                    out.push_back(std::move(S.top()));
                    S.pop();
                }
                break;
            case lexem_t::ternary_q:
                S.push(term);
                break;
            case lexem_t::ternary_c:
                while (S.size() && S.top().type != lexem_t::ternary_q ) {
                    out.push_back(std::move(S.top()));
                    S.pop();
                }
                break;
            case lexem_t::op_br:
            case lexem_t::fun:
                S.push(term);
                break;
            default:
                break;
        }
    }
    while (S.size()) {
        out.push_back(std::move(S.top()));
        S.pop();
    }

    return out;
}

double calc_postfix(const std::vector<lexem>& postfix, std::map<std::string, double>& variables) {
    std::stack<std::string> S;

    for (auto& term : postfix) {
        switch (term.type) {
            case lexem_t::number:
                S.push(term.name);
                break;
            case lexem_t::var: {
                double value = variables[term.name];
                S.push(std::to_string(value));
                break;
            }
            case lexem_t::ternary_q:
            case lexem_t::op:
            case lexem_t::fun:
                functions[term.name](S);
                break;
        }
    }
    return std::stod(S.top());
}

/*\
    TODO:
    вычисление постификсной,
    много симнольные операции,
    тернарная операция,
    класс функтор,
    корректность самого выражения,

*/

int main() {
    std::map<std::string, double> variables = {
        {"x", 1.0},
        {"y", 2.0},
        {"z", 3.0},
        {"z1", 4.0}
    };

    std::vector<std::string> lexem_str{ "number", "var", "op_br", "cl_br", "op", "fun", "comma", "ternary_q", "ternary_c"};
    auto a1 = "x/y/z^(sin(1/(1-a^c/b) + cos(a-b-c)))";
    auto a2 = parse_expr_to_lexems(a1);
    auto a3 = create_postfix_write(a2);

    for (auto& el : a3) {
        std::cout << el.name << " ";
    }

    /*auto v = parse_expr_to_lexems("3*x*2+sin(x!)+func(func1((y == 1) ? 1 << 2 : 2 ,z), z1)");
    for (auto& e : v) {
        std::cout << e.name << " " << lexem_str[(int)e.type] << std::endl;
    }

    auto post = create_postfix_write(v);
    std::cout << "\n";
    for (auto& t : post) {
        std::cout << "{" << t.name << ", " << lexem_str[(int)t.type] << " }" << "\n";
    }
    std::cout << "\n";

    auto res = calc_postfix(post, variables);
    std::cout << res;*/

    /*auto v = parse_expr_to_lexems("(y != 1) ? 3.6 : 2");
    auto post = create_postfix_write(v);
    auto res = calc_postfix(post, variables);
    std::cout << res;*/
    return 1;
}