#pragma once

#include "token.hpp"
#include <variant>
#include <memory>

struct Assign;
struct Binary;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;

using Expr = std::variant<Assign, Binary, Grouping, Literal, Unary, Variable>;

struct Assign {
	Token name;
	std::shared_ptr<Expr> value;

	Assign(Token name, std::shared_ptr<Expr> value) : name(name), value(value) {}
};

struct Binary {
	std::shared_ptr<Expr> left;
	Token op;
	std::shared_ptr<Expr> right;

	Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right) : left(left), op(op), right(right) {}
};

struct Grouping {
	std::shared_ptr<Expr> expression;

	Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}
};

struct Literal {
	LiteralObject value;

	Literal(LiteralObject value) : value(value) {}
};

struct Unary {
	Token op;
	std::shared_ptr<Expr> right;

	Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}
};

struct Variable {
	Token name;

	Variable(Token name) : name(name) {}
};

