#include <gtest/gtest.h>

#include "functions_and_derivatives.cpp"
//--------------------------------------------------------------------------------



//-----------------------------------global---------------------------------------
Factory factory;
class Func : public testing::Test{};
class Oper : public testing::Test{};
//--------------------------------------------------------------------------------



//------------------------------tests---------------------------------------------
// f(x), f'(x) and to_string(f(x)))
TEST_F(Func, Identity_function)
{
    //SCOPED_TRACE("Identity function test");
    auto f = factory.create("ident");
    ASSERT_DOUBLE_EQ(f->evaluate(1.0), 1.0);
    ASSERT_DOUBLE_EQ(f->derivate(2.0), 1.0);
    ASSERT_EQ(f->to_string(), "x");
}

TEST_F(Func, Const_function)
{
    //SCOPED_TRACE("Constant function test");
    auto f = factory.create("const", 2.0);
    ASSERT_DOUBLE_EQ(f->evaluate(1.0), 2.0);
    ASSERT_DOUBLE_EQ(f->derivate(2.0), 0.0);
    ASSERT_EQ(f->to_string(), std::to_string(2.0));
}

TEST_F(Func, Power_function)
{
    //SCOPED_TRACE("Power function test");
    auto f = factory.create("power", 3.0);
    ASSERT_DOUBLE_EQ(f->evaluate(2.0), 8.0);
    ASSERT_DOUBLE_EQ(f->derivate(2.0), 12.0);
    ASSERT_EQ(f->to_string(), "x^" + std::to_string(3.0));
}

TEST_F(Func, Exponent_function)
{
    //SCOPED_TRACE("Exponent function test");
    auto f = factory.create("exp");
    ASSERT_DOUBLE_EQ(f->evaluate(4.0), std::exp(4.0));
    ASSERT_DOUBLE_EQ(f->derivate(2.0), std::exp(2.0));
    ASSERT_EQ(f->to_string(), "e^x");
}

TEST_F(Func, Polynomial_function)
{
    //SCOPED_TRACE("Polynomial function test");
    auto f = factory.create("polynomial", {7.0, 0.0, 3.0, 15.0});
    ASSERT_DOUBLE_EQ(f->evaluate(10.0), 15307.0);
    ASSERT_DOUBLE_EQ(f->derivate(10.0), 4560.0);
    ASSERT_EQ(f->to_string(), "15.000000x^3 + 3.000000x^2 + 0.000000x^1 + 7.000000");
}


// errors
TEST_F(Func, Power_eval_zero)   // 0 in negative
{
    //SCOPED_TRACE("Zero negative power test");
    auto f = factory.create("power", -2.0);
    ASSERT_THROW(f->evaluate(0.0), std::invalid_argument);
}

TEST_F(Func, Power_deriv_zero)   // 0 in non-positive from derivate
{
    //SCOPED_TRACE("Zero non-positive power derivate test");
    auto f = factory.create("power", 1.0);
    ASSERT_THROW(f->derivate(0.0), std::invalid_argument);
}


// operations
// sum
TEST_F(Oper, Addition_ident_const)
{
    auto f = factory.create("ident");
    auto g = factory.create("const", 1.0);
    auto p = f + g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 4.0);
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 1.0);
    ASSERT_EQ(p->to_string(), "(x) + (" + std::to_string(1.0) + ")");
}

TEST_F(Oper, Addition_power_exp)
{
    auto f = factory.create("power", 2.0);
    auto g = factory.create("exp");
    auto p = f + g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 9.0 + std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(4.0), 8.0 + std::exp(4.0));
    ASSERT_EQ(p->to_string(), "(x^" + std::to_string(2.0) + ") + (e^x)");
}

TEST_F(Oper, Addition_polynom_exp)
{
    auto f = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto g = factory.create("exp");
    auto p = f + g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 4.0 + std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 2.0 + std::exp(2.0));
    ASSERT_EQ(p->to_string(), "(1.000000x^2 + -2.000000x^1 + 1.000000) + (e^x)");
}


// sub
TEST_F(Oper, Subtraction_ident_const)
{
    auto f = factory.create("ident");
    auto g = factory.create("const", 1.0);
    auto p = f - g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 2.0);
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 1.0);
    ASSERT_EQ(p->to_string(), "(x) - (" + std::to_string(1.0) + ")");
}

TEST_F(Oper, Subtraction_power_exp)
{
    auto f = factory.create("power", 2.0);
    auto g = factory.create("exp");
    auto p = f - g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 9.0 - std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(4.0), 8.0 - std::exp(4.0));
    ASSERT_EQ(p->to_string(), "(x^" + std::to_string(2.0) + ") - (e^x)");
}

TEST_F(Oper, Subtraction_polynom_exp)
{
    auto f = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto g = factory.create("exp");
    auto p = f - g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 4.0 - std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 2.0 - std::exp(2.0));
    ASSERT_EQ(p->to_string(), "(1.000000x^2 + -2.000000x^1 + 1.000000) - (e^x)");
}


// mul
TEST_F(Oper, Multiplication_ident_const)
{
    auto f = factory.create("ident");
    auto g = factory.create("const", 1.0);
    auto p = f * g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 3.0);
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 1.0);
    ASSERT_EQ(p->to_string(), "(x) * (" + std::to_string(1.0) + ")");
}

TEST_F(Oper, Multiplication_power_exp)
{
    auto f = factory.create("power", 2.0);
    auto g = factory.create("exp");
    auto p = f * g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 9.0 * std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(4.0), 24.0 * std::exp(4.0));
    ASSERT_EQ(p->to_string(), "(x^" + std::to_string(2.0) + ") * (e^x)");
}

TEST_F(Oper, Multiplication_polynom_exp)
{
    auto f = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto g = factory.create("exp");
    auto p = f * g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 4.0 * std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 3.0 * std::exp(2.0));
    ASSERT_EQ(p->to_string(), "(1.000000x^2 + -2.000000x^1 + 1.000000) * (e^x)");
}


// div
TEST_F(Oper, Division_ident_const)
{
    auto f = factory.create("ident");
    auto g = factory.create("const", 1.0);
    auto p = f / g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 3.0);
    ASSERT_DOUBLE_EQ(p->derivate(2.0), 1.0);
    ASSERT_EQ(p->to_string(), "(x) / (" + std::to_string(1.0) + ")");
}

TEST_F(Oper, Division_power_exp)
{
    auto f = factory.create("power", 2.0);
    auto g = factory.create("exp");
    auto p = f / g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 9.0 / std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(4.0), (-8.0) / std::exp(4.0));
    ASSERT_EQ(p->to_string(), "(x^" + std::to_string(2.0) + ") / (e^x)");
}

TEST_F(Oper, Division_polynom_exp)
{
    auto f = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto g = factory.create("exp");
    auto p = f / g;
    ASSERT_DOUBLE_EQ(p->evaluate(3.0), 4.0 / std::exp(3.0));
    ASSERT_DOUBLE_EQ(p->derivate(2.0), (1.0) / std::exp(2.0));
    ASSERT_EQ(p->to_string(), "(1.000000x^2 + -2.000000x^1 + 1.000000) / (e^x)");
}


// errors with div
TEST_F(Oper, Division_eval_zero)  // division by 0
{
    auto f = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto g = factory.create("ident");
    auto p = f / g;
    ASSERT_THROW(p->evaluate(0.0), std::logic_error);
}


// complex operations
TEST_F(Oper, Addition_complex)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = idn + con + pow + exp + pol;
    ASSERT_DOUBLE_EQ(comp->evaluate(3.0), 19.0 + std::exp(3.0));
    ASSERT_DOUBLE_EQ(comp->derivate(2.0), 7.0 + std::exp(2.0));
    ASSERT_EQ(comp->to_string(), "((((x) + (3.000000)) + (x^2.000000)) + (e^x)) + (1.000000x^2 + -2.000000x^1 + 1.000000)");
}

TEST_F(Oper, Subtraction_complex)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = idn - con - pow - exp - pol;
    ASSERT_DOUBLE_EQ(comp->evaluate(3.0), -13.0 - std::exp(3.0));
    ASSERT_DOUBLE_EQ(comp->derivate(2.0), -5.0 - std::exp(2.0));
    ASSERT_EQ(comp->to_string(), "((((x) - (3.000000)) - (x^2.000000)) - (e^x)) - (1.000000x^2 + -2.000000x^1 + 1.000000)");
}

TEST_F(Oper, Multiplication_complex)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = idn * con * pow * exp * pol;
    ASSERT_DOUBLE_EQ(comp->evaluate(3.0), 324.0 * std::exp(3.0));
    ASSERT_DOUBLE_EQ(comp->derivate(2.0), 108.0 * std::exp(2.0));
    ASSERT_EQ(comp->to_string(), "((((x) * (3.000000)) * (x^2.000000)) * (e^x)) * (1.000000x^2 + -2.000000x^1 + 1.000000)");
}

TEST_F(Oper, Division_complex)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = idn / con / pow / exp / pol;
    ASSERT_DOUBLE_EQ(comp->evaluate(3.0), (1.0 / 36.0) / std::exp(3.0));
    ASSERT_DOUBLE_EQ(comp->derivate(2.0), (-7.0 / 12.0) / std::exp(2.0));
    ASSERT_EQ(comp->to_string(), "((((x) / (3.000000)) / (x^2.000000)) / (e^x)) / (1.000000x^2 + -2.000000x^1 + 1.000000)");
}


// all inclusive
TEST_F(Oper, All_inclusive_first)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = idn / (con * pow) - exp / pol;
    ASSERT_DOUBLE_EQ(comp->evaluate(3.0), (1.0 / 9.0) - (std::exp(3.0) / 4.0));
    ASSERT_DOUBLE_EQ(comp->derivate(2.0), (-1.0 / 12.0) + std::exp(2.0));
    ASSERT_EQ(comp->to_string(), "((x) / ((3.000000) * (x^2.000000))) - ((e^x) / (1.000000x^2 + -2.000000x^1 + 1.000000))");
}

TEST_F(Oper, All_inclusive_second)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = (idn + con - pow) / (exp * pol);
    ASSERT_DOUBLE_EQ(comp->evaluate(3.0), (-3.0 / 4.0) / std::exp(3.0));
    ASSERT_DOUBLE_EQ(comp->derivate(2.0), -6.0 / std::exp(2.0));
    ASSERT_EQ(comp->to_string(), "(((x) + (3.000000)) - (x^2.000000)) / ((e^x) * (1.000000x^2 + -2.000000x^1 + 1.000000))");
}


//finding roots
TEST_F(Oper, Root_equation)
{
    auto idn = factory.create("ident");
    auto con = factory.create("const", 3.0);
    auto pow = factory.create("power", 2.0);
    auto exp = factory.create("exp");
    auto pol = factory.create("polynomial", {1.0, -2.0, 1.0});
    auto comp = (idn + con - pow) / (exp * pol);
    ASSERT_NEAR(find_equation_root(comp), (1.0 - std::pow(13.0, 0.5)) / 2.0, 1e-6);
}
//--------------------------------------------------------------------------------



//------------------------------main----------------------------------------------
int 
main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}