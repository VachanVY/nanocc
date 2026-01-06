#include "nanocc/Utils.hpp"
#include "nanocc/AST/AST.hpp"

#include "SemaHelper.hpp"

namespace sema {
// loop labelling -- start
void programNodeLoopLabelling(std::unique_ptr<ProgramNode>& program_node, std::string& loop_label) {
    for (auto& functions : program_node->functions) {
        functionDeclNodeLoopLabelling(functions, loop_label);
    }
}

void declarationNodeLoopLabelling(std::unique_ptr<DeclarationNode>& declaration_node,
                                  std::string& loop_label) {
    if (declaration_node->func) {
        functionDeclNodeLoopLabelling(declaration_node->func, loop_label);
    } else if (declaration_node->var) {
        variableDeclNodeLoopLabelling(declaration_node->var, loop_label);
    }
}

void variableDeclNodeLoopLabelling(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                   std::string& loop_label) {}; // no-op

void functionDeclNodeLoopLabelling(std::unique_ptr<FunctionDeclNode>& function_decl_node,
                                   std::string& loop_label) {
    if (function_decl_node->body) {
        blockNodeLoopLabelling(function_decl_node->body, loop_label);
    }
}

void blockNodeLoopLabelling(std::unique_ptr<BlockNode>& block_node, std::string& loop_label) {
    for (const auto& block_item : block_node->block_items) {
        blockItemNodeLoopLabelling(block_item, loop_label);
    }
}

void blockItemNodeLoopLabelling(const std::unique_ptr<BlockItemNode>& block_item_node,
                                std::string& loop_label) {
    if (block_item_node->declaration) {
        declarationNodeLoopLabelling(block_item_node->declaration, loop_label); // no-op
    } else if (block_item_node->statement) {
        statementNodeLoopLabelling(block_item_node->statement, loop_label);
    }
}

void statementNodeLoopLabelling(std::unique_ptr<StatementNode>& statement_node,
                                std::string& loop_label) {
    if (statement_node->return_stmt) {
        returnNodeLoopLabelling(statement_node->return_stmt, loop_label); // no-op
    } else if (statement_node->expression_stmt) {
        expressionNodeLoopLabelling(statement_node->expression_stmt, loop_label); // no-op
    } else if (statement_node->ifelse_stmt) {
        ifElseNodeLoopLabelling(statement_node->ifelse_stmt, loop_label); // no-op
    } else if (statement_node->compound_stmt) {
        compoundNodeLoopLabelling(statement_node->compound_stmt, loop_label);
    } else if (statement_node->break_stmt) {
        breakNodeLoopLabelling(statement_node->break_stmt, loop_label);
    } else if (statement_node->continue_stmt) {
        continueNodeLoopLabelling(statement_node->continue_stmt, loop_label);
    } else if (statement_node->while_stmt) {
        whileNodeLoopLabelling(statement_node->while_stmt, loop_label);
    } else if (statement_node->dowhile_stmt) {
        doWhileNodeLoopLabelling(statement_node->dowhile_stmt, loop_label);
    } else if (statement_node->for_stmt) {
        forNodeLoopLabelling(statement_node->for_stmt, loop_label);
    } else if (statement_node->null_stmt) {
        nullNodeLoopLabelling(statement_node->null_stmt, loop_label); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void returnNodeLoopLabelling(std::unique_ptr<ReturnNode>& return_node, std::string& loop_label) {
}; // no-op
void expressionNodeLoopLabelling(std::unique_ptr<ExpressionNode>& expression_node,
                                 std::string& loop_label) {}; // no-op

void ifElseNodeLoopLabelling(std::unique_ptr<IfElseNode>& ifelse_node, std::string& loop_label) {
    statementNodeLoopLabelling(ifelse_node->if_block, loop_label);
    if (ifelse_node->else_block) {
        statementNodeLoopLabelling(ifelse_node->else_block, loop_label);
    }
};

void compoundNodeLoopLabelling(std::unique_ptr<CompoundNode>& compound_node,
                               std::string& loop_label) {
    blockNodeLoopLabelling(compound_node->block, loop_label);
};

void breakNodeLoopLabelling(std::unique_ptr<BreakNode>& break_node, std::string& loop_label) {
    if (loop_label.empty()) {
        throw std::runtime_error("Label Error: 'break' used outside of a loop");
    }
    break_node->label = std::make_unique<IdentifierNode>();
    break_node->label->name = loop_label;
}

void continueNodeLoopLabelling(std::unique_ptr<ContinueNode>& continue_node,
                               std::string& loop_label) {
    if (loop_label.empty()) {
        throw std::runtime_error("Label Error: 'continue' used outside of a loop");
    }
    continue_node->label = std::make_unique<IdentifierNode>();
    continue_node->label->name = loop_label;
}

void whileNodeLoopLabelling(std::unique_ptr<WhileNode>& while_node, std::string& loop_label) {
    std::string new_label = getLabelName("while");
    while_node->label = std::make_unique<IdentifierNode>();
    while_node->label->name = new_label;
    statementNodeLoopLabelling(while_node->body, new_label);
}

void doWhileNodeLoopLabelling(std::unique_ptr<DoWhileNode>& dowhile_node, std::string& loop_label) {
    std::string new_label = getLabelName("do_while");
    dowhile_node->label = std::make_unique<IdentifierNode>();
    dowhile_node->label->name = new_label;
    statementNodeLoopLabelling(dowhile_node->body, new_label);
}

void forNodeLoopLabelling(std::unique_ptr<ForNode>& for_node, std::string& loop_label) {
    std::string new_label = getLabelName("for");
    for_node->label = std::make_unique<IdentifierNode>();
    for_node->label->name = new_label;
    statementNodeLoopLabelling(for_node->body, new_label);
}

void nullNodeLoopLabelling(std::unique_ptr<NullNode>& null_node, std::string& loop_label) {
}; // no-op
// loop labelling -- end
} // namespace sema

namespace nanocc {
void semaLoopLabelling(std::unique_ptr<ProgramNode>& ast) {
    std::string loop_label = "";
    sema::programNodeLoopLabelling(ast, loop_label);
}
} // namespace nanocc
