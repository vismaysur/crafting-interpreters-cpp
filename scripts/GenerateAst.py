import sys
from typing import Iterable


def defineType(f, class_name: str, fields_str: str):
    f.write(f"struct {class_name} {{\n")
    fields = fields_str.split(", ")

    # data members
    for field in fields:
        f.write(f"\t{field};\n")
    f.write("\n")

    # constructor with member initializer list
    f.write(f"\t{class_name}({fields_str}) : ")

    # initializer list
    inits = []
    for field in fields:
        name = field.split(" ")[1]
        inits.append(f"{name}({name})")
    f.write(", ".join(inits))
    f.write(" {}\n")

    f.write("};\n\n")


def defineAst(output_dir: str, base_name: str, types: Iterable[str]):
    path = output_dir + "/" + base_name.lower() + ".hpp"
    with open(path, "w") as f:
        f.write("#pragma once\n\n")
        f.write('#include "token.hpp"\n')
        f.write("#include <variant>\n")
        f.write("#include <vector>\n")
        f.write("#include <memory>\n\n")

        # Forward declarations for node types
        for type in types:
            class_name = type.split("=")[0].strip()
            f.write(f"struct {class_name};\n")
        f.write("\n")

        # Define Expr alias as std::variant of all node types
        variants = ", ".join([type.split("=")[0].strip() for type in types])
        f.write(f"using {base_name} = std::variant<{variants}>;\n\n")

        # Define types as structs
        for type in types:
            class_name = type.split("=")[0].strip()
            fields = type.split("=")[1].strip()
            defineType(f, class_name, fields)


def main():
    args = sys.argv[1:]

    if len(args) != 1:
        print("Usage: python GenerateAst.py [output_dir]")
        sys.exit(1)

    output_dir = args[0]

    defineAst(
        output_dir,
        "Expr",
        [
            "Assign= Token name, std::shared_ptr<Expr> value",
            "Logical= std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right",
            "Binary= std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right",
            "Call= std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> args",
            "Grouping= std::shared_ptr<Expr> expression",
            "Literal= LiteralObject value",
            "Unary= Token op, std::shared_ptr<Expr> right",
            "Variable= Token name",
        ],
    )

    defineAst(
        output_dir,
        "Stmt",
        [
            "Block= std::vector<std::shared_ptr<Stmt>> statements",
            "Expression= std::shared_ptr<Expr> expr",
            "Func= Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body",
            "If= std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch",
            "Print= std::shared_ptr<Expr> expr",
            "Return= Token keyword, std::shared_ptr<Expr> value",
            "Var= Token name, std::shared_ptr<Expr> initializer",
            "While= std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body",
        ],
    )


if __name__ == "__main__":
    main()
