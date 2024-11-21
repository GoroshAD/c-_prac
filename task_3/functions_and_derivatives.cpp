#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <iostream>
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
class TFunction;
class Identity;
class Const;
class Power;
class Exponent;
class Polynomial;
class Factory;
using TFunction_ptr = std::shared_ptr<TFunction>;
std::unordered_map<std::string, int> type2int = {{"ident", 0},
                                                 {"const", 1},
                                                 {"power", 2},
                                                 {"exp", 3},
                                                 {"polynomial", 4},
                                                };
//----------------------------------------------------------------------------



//----------------------------classes-----------------------------------------
class TFunction {
public:
    virtual ~TFunction() = default;
    virtual double evaluate(double x) const = 0;
    virtual double derivate(double x) const = 0;
    virtual std::string to_string() const = 0;
    
    double operator()(double x) const {
        return evaluate(x);
    }
    
    static TFunction_ptr create(const std::string& type, double param=0.0, const std::vector<double>& coefs={});
};

class Identity : public TFunction {
public:
    double evaluate(double x) const override {
        return x;
    }

    double derivate(double x) const override {
        return 1.0;
    }

    std::string to_string() const override {
        return "x";
    }
};

class Const : public TFunction {
private:
    double val;
public:
    explicit Const(double v) : val(v) {}

    double evaluate(double x) const override {
        return val;
    }
    double derivate(double x) const override {
        return 0.0;
    }

    std::string to_string() const override {
        return std::to_string(val);
    }
};

class Power : public TFunction {
private:
    double power;
public:
    explicit Power(double p) : power(p) {}

    double evaluate(double x) const override {
        if (!x && power <= 0) {
            throw std::invalid_argument("Division by 0.");
        }
        return std::pow(x, power);
    }

    double derivate(double x) const override {
        if (!x && power - 1 <= 0) {
            throw std::invalid_argument("Division by 0.");
        }
        return power * std::pow(x, power - 1);
    }

    std::string to_string() const override {
        return "x^" + std::to_string(power);
    }
};

class Exponent : public TFunction {
public:
    explicit Exponent() {}

    double evaluate(double x) const override {
        return std::exp(x);
    }

    double derivate(double x) const override {
        return std::exp(x);
    }

    std::string to_string() const override {
        return "e^x";
    }
};

class Polynomial : public TFunction {
private:
    std::vector<double> coefs;
public:
    explicit Polynomial(const std::vector<double> &vec) : coefs(vec) {}

    double evaluate(double x) const override {
        double res = 0.0;
        int power = 0;
        for (auto &i : coefs) {
            res += i * std::pow(x, power);
            ++power;
        }
        return res;
    }

    double derivate(double x) const override {
        double res = 0.0;
        int power = 0;
        for (auto &i : coefs) {
            res += (power != 0 ? i * power * std::pow(x, power - 1) : 0);
            ++power;
        }
        return res;
    }

    std::string to_string() const override {
        if (coefs.size() <= 1) {   // if polynomial was like a const
            return "0.0";
        }
        std::string res = "";
        int power = coefs.size() - 1;
        for (auto i = coefs.rbegin(); i != coefs.rend(); ++i) {
            res += (power != 0 ? " + " + std::to_string(*i) + "x^" + std::to_string(power) : " + " + std::to_string(*i));
            power -= 1;
        }
        res.erase(0, 3);   // delete first " + "
        return res;
    }
};

TFunction_ptr TFunction::create(const std::string& type, double param, const std::vector<double>& coefs) {
    switch (type2int[type]) {
        case 0:
            return std::make_shared<Identity>();
        case 1:
            return std::make_shared<Const>(param);
        case 2:
            return std::make_shared<Power>(param);
        case 3:
            return std::make_shared<Exponent>();
        case 4:
            return std::make_shared<Polynomial>(coefs);
        default:
            throw std::logic_error("Unknown function.");
    }
}

class Addition : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Addition(TFunction_ptr a, TFunction_ptr b) : a(a), b(b) {}
 
    double evaluate(double x) const override {
        return a->evaluate(x) + b->evaluate(x);
    }

    double derivate(double x) const override {
        return a->derivate(x) + b->derivate(x);
    }

    std::string to_string() const override {
        return "(" + a->to_string() + ") + (" + b->to_string() + ")";
    }
};

class Subtraction : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Subtraction(TFunction_ptr a, TFunction_ptr b) : a(a), b(b) {}
 
    double evaluate(double x) const override {
        return a->evaluate(x) - b->evaluate(x);
    }

    double derivate(double x) const override {
        return a->derivate(x) - b->derivate(x);
    }

    std::string to_string() const override {
        return "(" + a->to_string() + ") - (" + b->to_string() + ")";
    }
};

class Multiplication : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Multiplication(TFunction_ptr a, TFunction_ptr b) : a(a), b(b) {}
 
    double evaluate(double x) const override {
        return a->evaluate(x) * b->evaluate(x);
    }

    double derivate(double x) const override {
        return a->derivate(x) * b->evaluate(x) + b->derivate(x) * a->evaluate(x);   // a'b + b'a
    }

    std::string to_string() const override {
        return "(" + a->to_string() + ") * (" + b->to_string() + ")";
    }
};

class Division : public TFunction {
private:
    TFunction_ptr a;
    TFunction_ptr b;
public:
    explicit Division(TFunction_ptr a, TFunction_ptr b) : a(a), b(b) {}
 
    double evaluate(double x) const override {
        if (b->evaluate(x) == 0) {
            throw std::invalid_argument("Division by 0.");
        }
        return a->evaluate(x) / b->evaluate(x);
    }

    double derivate(double x) const override {
        if (b->evaluate(x) == 0) {
            throw std::invalid_argument("Division by 0.");
        }
        return (a->derivate(x) * b->evaluate(x) - b->derivate(x) * a->evaluate(x)) / std::pow(b->evaluate(x), 2);   // (a'b - b'a) / b^2
    }

    std::string to_string() const override {
        return "("+  a->to_string() + ") / (" + b->to_string() + ")";
    }
};

TFunction_ptr operator+(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Addition>(a, b);
}

TFunction_ptr operator-(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Subtraction>(a, b);
}

TFunction_ptr operator*(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Multiplication>(a, b);
}

TFunction_ptr operator/(TFunction_ptr a, TFunction_ptr b) {
    return std::make_shared<Division>(a, b);
}

class Factory {
public:
    TFunction_ptr create(const std::string& type, double param=0) const {
        return TFunction::create(type, param);
    }

    TFunction_ptr create(const std::string& type, const std::vector<double>& coefs) const {
        return TFunction::create(type, 0, coefs);
	}
};
//----------------------------------------------------------------------------



//--------------------------functions-----------------------------------------
double find_equation_root(TFunction_ptr &eq, double x=0, double error=1e-6, double step=0.01)
{
    for (int i = 0; i < 100000; ++i) {
        if (std::abs(eq->evaluate(x)) < error) {
            return x;
        }
        if (std::abs(eq->derivate(x)) < 1e-10) {   // too small
            return x;
        }
        x = x - step * eq->evaluate(x) / eq->derivate(x);
    }
    return x;
}
//----------------------------------------------------------------------------
