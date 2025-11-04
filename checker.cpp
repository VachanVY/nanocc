#include "utils.hpp"
#include "parser.hpp"
#include "checker.hpp"

void ProgramNode::resolveTypes(SymbolTable& sym_table) { this->func->resolveTypes(sym_table); }

void FunctionNode::resolveTypes(SymbolTable& sym_table) {
    for (const auto& block : this->body) {
        block->resolveTypes(sym_table);
    }
}

void BlockNode::resolveTypes(SymbolTable& sym_table) {
    if (this->declaration) {
        this->declaration->resolveTypes(sym_table);
    } else if (this->statement) {
        this->statement->resolveTypes(sym_table);
    }
}

void StatementNode::resolveTypes(SymbolTable& sym_table) {
    if (this->return_stmt) {
        this->return_stmt->resolveTypes(sym_table);
    } else if (this->expression_stmt) {
        this->expression_stmt->resolveTypes(sym_table);
    } else if (this->ifelse_stmt) {
        this->ifelse_stmt->resolveTypes(sym_table);
    } else if (this->null_stmt) {
        this->null_stmt->resolveTypes(sym_table); // no-op
    }
}

void ReturnNode::resolveTypes(SymbolTable& sym_table) { this->ret_expr->resolveTypes(sym_table); }

void ExpressionNode::resolveTypes(SymbolTable& sym_table) { this->expr->resolveTypes(sym_table); }

void IfElseNode::resolveTypes(SymbolTable& sym_table) {
    this->condition->resolveTypes(sym_table);
    this->if_block->resolveTypes(sym_table);
    if (this->else_block) {
        this->else_block->resolveTypes(sym_table);
    }
}

/// @brief Check for variable redeclaration; Add to symbol table after giving unique name;
void DeclarationNode::resolveTypes(SymbolTable& sym_table) {
    if (sym_table.contains(this->var_identifier->name)) {
        throw std::runtime_error(
            std::format("Type Error: Redeclaration of variable '{}'", this->var_identifier->name));
    }

    std::string unique_name = getUniqueName(this->var_identifier->name);
    sym_table[this->var_identifier->name] = unique_name;
    this->var_identifier->name = unique_name; // update the declaration's name too
    if (this->init_expr) {
        this->init_expr->resolveTypes(sym_table);
    }
}

void ExprNode::resolveTypes(SymbolTable& sym_table) { this->left_exprf->resolveTypes(sym_table); }

void ExprFactorNode::resolveTypes(SymbolTable& sym_table) {
    if (this->var_identifier) {
        this->var_identifier->resolveTypes(sym_table);
    } else if (this->unary) {
        this->unary->resolveTypes(sym_table);
    } else if (this->expr) {
        this->expr->resolveTypes(sym_table);
    } else if (this->constant) {
        this->constant->resolveTypes(sym_table); // no-op
    }
}

/// @brief Should already be added to the symbol table by `DeclarationNode`
void VarNode::resolveTypes(SymbolTable& sym_table) {
    if (sym_table.contains(this->var_name->name)) {
        this->var_name->name = sym_table[this->var_name->name];
    } else {
        throw std::runtime_error(
            std::format("Type Error: Undeclared variable '{}'", this->var_name->name));
    }
}

/// @brief left_expr of a `AssignmentNode` must be a `VarNode`
void AssignmentNode::resolveTypes(SymbolTable& sym_table) {
    assert(this->left_expr && this->left_expr->left_exprf &&
           "Left expression or its factor is null in `AssignmentNode::resolveTypes`");
    auto left_factor = this->left_expr->left_exprf.get();
    if (!left_factor->var_identifier) {
        throw std::runtime_error("Type Error: Left-hand side of assignment must be a variable");
    }

    this->left_expr->resolveTypes(sym_table);
    this->right_expr->resolveTypes(sym_table);
}

void ConditionalNode::resolveTypes(SymbolTable& sym_table) {
    this->condition->resolveTypes(sym_table);
    this->true_expr->resolveTypes(sym_table);
    this->false_expr->resolveTypes(sym_table);
}

void BinaryNode::resolveTypes(SymbolTable& sym_table) {
    this->left_expr->resolveTypes(sym_table);
    this->right_expr->resolveTypes(sym_table);
}

/*
- Detect errors of type `<unary_op> <exprfactor> = <expr>`
- eg: !a = 3 ==parsed_as=> UnaryNode('!', AssignmentNode(VarNode('a'), ConstantNode('3')));
*/
void UnaryNode::resolveTypes(SymbolTable& sym_table) {
    assert(this->operand && "Operand is null in `UnaryNode::resolveTypes`");
    if (isa<AssignmentNode>(this->operand.get())) {
        throw std::runtime_error("Type Error: Cannot assign to the result of a unary operation");
    }
    this->operand->resolveTypes(sym_table);
}

/*
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr,
                     "Usage: {} [--lex|--parse|--validate|--tacky|--codegen]"
                     "<source_file>",
                     argv[0]);
        return 1;
    }

    // Find the source file (non-flag argument)
    std::string filename;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (!arg.starts_with("--")) {
            filename = arg;
            break;
        }
    }

    if (filename.empty()) {
        std::println(stderr, "Error: No source file specified");
        return 1;
    }
    auto contents = getFileContents(filename);
    auto tokens = lexer(contents);
    size_t i = 0;
    for (const auto& [token_type, lexeme] : tokens) {
        std::println("{}, {}, {}", i++, tokenTypeToString(token_type), lexeme);
    }
    auto ast = parse(tokens);
    ast->dump();
    SymbolTable sym_table;
    ast->resolveTypes(sym_table);
    ast->dump();
    return 0;
}
// */