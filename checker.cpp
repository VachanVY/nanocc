#include "utils.hpp"
#include "parser.hpp"
#include "checker.hpp"

namespace {
SymbolTable copySymbolTableForNewScope(const SymbolTable& old_sym_table, bool curr_scope = false) {
    SymbolTable new_sym_table = old_sym_table;
    for (const auto& [var_name, var_scope] : old_sym_table) {
        new_sym_table[var_name].from_curr_scope = curr_scope;
    }
    return new_sym_table;
}
} // namespace

// identifier resolution -- start
void ProgramNode::resolveTypes(SymbolTable& sym_table) { this->func->resolveTypes(sym_table); }

void FunctionNode::resolveTypes(SymbolTable& sym_table) { body->resolveTypes(sym_table); }

void BlockNode::resolveTypes(SymbolTable& sym_table) {
    for (const auto& block_item : this->block_items) {
        block_item->resolveTypes(sym_table);
    }
}

void BlockItemNode::resolveTypes(SymbolTable& sym_table) {
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
    } else if (this->compound_stmt) {
        this->compound_stmt->resolveTypes(sym_table);
    } else if (this->break_stmt) {
        this->break_stmt->resolveTypes(sym_table); // no-op
    } else if (this->continue_stmt) {
        this->continue_stmt->resolveTypes(sym_table); // no-op
    } else if (this->while_stmt) {
        this->while_stmt->resolveTypes(sym_table);
    } else if (this->dowhile_stmt) {
        this->dowhile_stmt->resolveTypes(sym_table);
    } else if (this->for_stmt) {
        this->for_stmt->resolveTypes(sym_table);
    } else if (this->null_stmt) {
        this->null_stmt->resolveTypes(sym_table); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
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

/// @brief Create a new scope, i.e create a copy of the current symbol table and
/// mark all variables from the parent scope as false as they are not from current block scope
void CompoundNode::resolveTypes(SymbolTable& old_sym_table) {
    SymbolTable new_sym_table = copySymbolTableForNewScope(old_sym_table, /*curr_scope=*/false);
    this->block->resolveTypes(new_sym_table);
}

void BreakNode::resolveTypes(SymbolTable& sym_table){}; // no-op

void ContinueNode::resolveTypes(SymbolTable& sym_table){}; // no-op

void WhileNode::resolveTypes(SymbolTable& sym_table) {
    this->condition->resolveTypes(sym_table);
    this->body->resolveTypes(sym_table);
}

void DoWhileNode::resolveTypes(SymbolTable& sym_table) {
    this->body->resolveTypes(sym_table);
    this->condition->resolveTypes(sym_table);
}

void ForNode::resolveTypes(SymbolTable& sym_table) {
    // create a new scope for the for-loop
    SymbolTable new_sym_table = copySymbolTableForNewScope(sym_table, /*curr_scope=*/false);
    this->init->resolveTypes(new_sym_table);
    if (this->condition) {
        this->condition->resolveTypes(new_sym_table);
    }
    if (this->post) {
        this->post->resolveTypes(new_sym_table);
    }
    // `StatementNode::resolveTypes` =calls=> `CompoundNode::resolveTypes`
    // => creates another new scope for the body
    // no need to do anything here for that
    this->body->resolveTypes(new_sym_table);
}

void ForInitNode::resolveTypes(SymbolTable& sym_table) {
    if (this->declaration) {
        this->declaration->resolveTypes(sym_table);
    } else if (this->init_expr) {
        this->init_expr->resolveTypes(sym_table);
    }
}

void NullNode::resolveTypes(SymbolTable& sym_table){}; // no-op

/// @brief Check for variable redeclaration in the same scope;
/// Add to symbol table after giving unique name;
void DeclarationNode::resolveTypes(SymbolTable& sym_table) {
    if (sym_table.contains(this->var_identifier->name) &&
        sym_table[this->var_identifier->name].from_curr_scope) {
        throw std::runtime_error(
            std::format("Type Error: Redeclaration of variable '{}'", this->var_identifier->name));
    }

    std::string unique_name = getUniqueName(this->var_identifier->name);
    sym_table[this->var_identifier->name] = (VariableScope){unique_name, true};
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
        this->var_name->name = sym_table[this->var_name->name].unique_name;
    } else {
        throw std::runtime_error(
            std::format("Type Error: Undeclared variable '{}'", this->var_name->name));
    }
}

/// @brief `AssignmentNode::left_expr` must be a `VarNode` else throw type error
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
// identifier resolution -- end

// loop labelling -- start
void ProgramNode::loopLabelling(std::string& loop_label) { this->func->loopLabelling(loop_label); }

void FunctionNode::loopLabelling(std::string& loop_label) { body->loopLabelling(loop_label); }

void BlockNode::loopLabelling(std::string& loop_label) {
    for (const auto& block_item : this->block_items) {
        block_item->loopLabelling(loop_label);
    }
}

void BlockItemNode::loopLabelling(std::string& loop_label) {
    if (this->declaration) {
        this->declaration->loopLabelling(loop_label); // no-op
    } else if (this->statement) {
        this->statement->loopLabelling(loop_label);
    }
}

void StatementNode::loopLabelling(std::string& loop_label) {
    if (this->return_stmt) {
        this->return_stmt->loopLabelling(loop_label); // no-op
    } else if (this->expression_stmt) {
        this->expression_stmt->loopLabelling(loop_label); // no-op
    } else if (this->ifelse_stmt) {
        this->ifelse_stmt->loopLabelling(loop_label); // no-op
    } else if (this->compound_stmt) {
        this->compound_stmt->loopLabelling(loop_label);
    } else if (this->break_stmt) {
        this->break_stmt->loopLabelling(loop_label);
    } else if (this->continue_stmt) {
        this->continue_stmt->loopLabelling(loop_label);
    } else if (this->while_stmt) {
        this->while_stmt->loopLabelling(loop_label);
    } else if (this->dowhile_stmt) {
        this->dowhile_stmt->loopLabelling(loop_label);
    } else if (this->for_stmt) {
        this->for_stmt->loopLabelling(loop_label);
    } else if (this->null_stmt) {
        this->null_stmt->loopLabelling(loop_label); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void ReturnNode::loopLabelling(std::string& loop_label){};     // no-op
void ExpressionNode::loopLabelling(std::string& loop_label){}; // no-op

void IfElseNode::loopLabelling(std::string& loop_label) {
    this->if_block->loopLabelling(loop_label);
    if (this->else_block) {
        this->else_block->loopLabelling(loop_label);
    }
};

void CompoundNode::loopLabelling(std::string& loop_label) { block->loopLabelling(loop_label); };

void BreakNode::loopLabelling(std::string& loop_label) {
    if (loop_label.empty()) {
        throw std::runtime_error("Label Error: 'break' used outside of a loop");
    }
    this->label = std::make_unique<IdentifierNode>();
    this->label->name = loop_label;
}

void ContinueNode::loopLabelling(std::string& loop_label) {
    if (loop_label.empty()) {
        throw std::runtime_error("Label Error: 'continue' used outside of a loop");
    }
    this->label = std::make_unique<IdentifierNode>();
    this->label->name = loop_label;
}

void WhileNode::loopLabelling(std::string& loop_label) {
    std::string new_label = getLabelName("while");
    this->label = std::make_unique<IdentifierNode>();
    this->label->name = new_label;
    this->body->loopLabelling(new_label);
}

void DoWhileNode::loopLabelling(std::string& loop_label) {
    std::string new_label = getLabelName("do_while");
    this->label = std::make_unique<IdentifierNode>();
    this->label->name = new_label;
    this->body->loopLabelling(new_label);
}

void ForNode::loopLabelling(std::string& loop_label) {
    std::string new_label = getLabelName("for");
    this->label = std::make_unique<IdentifierNode>();
    this->label->name = new_label;
    this->body->loopLabelling(new_label);
}

void NullNode::loopLabelling(std::string& loop_label){}; // no-op

void DeclarationNode::loopLabelling(std::string& loop_label){}; // no-op

// loop labelling -- end

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
    auto tokens = lexer(contents, true);
    auto ast = parse(tokens, true);

    SymbolTable global_sym_table;
    ast->resolveTypes(global_sym_table);
    if (true) {
        std::println("----- Identifier Resolution -----");
        ast->dump();
        std::println("---------------------------------");
    }

    std::string loop_label = "";
    ast->loopLabelling(loop_label);
    if (true) {
        std::println("----- Loop Labelling -----");
        ast->dump();
        std::println("--------------------------");
    }
    return 0;
}
// */