#pragma once

#include "token.hpp"
#include <variant>
#include <vector>
#include <memory>

struct Assign;
struct Logical;
struct Binary;
struct Call;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;

using Expr = std::variant<Assign, Logical, Binary, Call, Grouping, Literal, Unary, Variable>;

struct Assign {
	Token name;
	std::shared_ptr<Expr> value;

	Assign(Token name, std::shared_ptr<Expr> value) : name(name), value(value) {}
};

struct Logical {
	std::shared_ptr<Expr> left;
	Token op;
	std::shared_ptr<Expr> right;

	Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right) : left(left), op(op), right(right) {}
};

struct Binary {
	std::shared_ptr<Expr> left;
	Token op;
	std::shared_ptr<Expr> right;

	Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right) : left(left), op(op), right(right) {}
};

struct Call {
	std::shared_ptr<Expr> callee;
	Token paren;
	std::vector<std::shared_ptr<Expr>> args;

	Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> args) : callee(callee), paren(paren), args(args) {}
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

