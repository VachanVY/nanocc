#include "nanocc/Utils.hpp"
#include "nanocc/AST/AST.hpp"

#include "SemaHelper.hpp"

// loop labelling -- start
void ProgramNodeLoopLabelling(std::unique_ptr<ProgramNode>& program_node, std::string& loop_label) {
    for (auto& functions : program_node->functions) {
        FunctionDeclNodeLoopLabelling(functions, loop_label);
    }
}

void DeclarationNodeLoopLabelling(std::unique_ptr<DeclarationNode>& declaration_node, std::string& loop_label) {
    if (declaration_node->func) {
        FunctionDeclNodeLoopLabelling(declaration_node->func, loop_label);
    } else if (declaration_node->var) {
        VariableDeclNodeLoopLabelling(declaration_node->var, loop_label);
    }
}

void VariableDeclNodeLoopLabelling(std::unique_ptr<VariableDeclNode>& variable_decl_node, std::string& loop_label){}; // no-op

void FunctionDeclNodeLoopLabelling(std::unique_ptr<FunctionDeclNode>& function_decl_node, std::string& loop_label) {
    if (function_decl_node->body) {
        BlockNodeLoopLabelling(function_decl_node->body, loop_label);
    }
}

void BlockNodeLoopLabelling(std::unique_ptr<BlockNode>& block_node, std::string& loop_label) {
    for (const auto& block_item : block_node->block_items) {
        BlockItemNodeLoopLabelling(block_item, loop_label);
    }
}

void BlockItemNodeLoopLabelling(const std::unique_ptr<BlockItemNode>& block_item_node, std::string& loop_label) {
    if (block_item_node->declaration) {
        DeclarationNodeLoopLabelling(block_item_node->declaration, loop_label); // no-op
    } else if (block_item_node->statement) {
        StatementNodeLoopLabelling(block_item_node->statement, loop_label);
    }
}

void StatementNodeLoopLabelling(std::unique_ptr<StatementNode>& statement_node, std::string& loop_label) {
    if (statement_node->return_stmt) {
        ReturnNodeLoopLabelling(statement_node->return_stmt, loop_label); // no-op
    } else if (statement_node->expression_stmt) {
        ExpressionNodeLoopLabelling(statement_node->expression_stmt, loop_label); // no-op
    } else if (statement_node->ifelse_stmt) {
        IfElseNodeLoopLabelling(statement_node->ifelse_stmt, loop_label); // no-op
    } else if (statement_node->compound_stmt) {
        CompoundNodeLoopLabelling(statement_node->compound_stmt, loop_label);
    } else if (statement_node->break_stmt) {
        BreakNodeLoopLabelling(statement_node->break_stmt, loop_label);
    } else if (statement_node->continue_stmt) {
        ContinueNodeLoopLabelling(statement_node->continue_stmt, loop_label);
    } else if (statement_node->while_stmt) {
        WhileNodeLoopLabelling(statement_node->while_stmt, loop_label);
    } else if (statement_node->dowhile_stmt) {
        DoWhileNodeLoopLabelling(statement_node->dowhile_stmt, loop_label);
    } else if (statement_node->for_stmt) {
        ForNodeLoopLabelling(statement_node->for_stmt, loop_label);
    } else if (statement_node->null_stmt) {
        NullNodeLoopLabelling(statement_node->null_stmt, loop_label); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void ReturnNodeLoopLabelling(std::unique_ptr<ReturnNode>& return_node, std::string& loop_label){};     // no-op
void ExpressionNodeLoopLabelling(std::unique_ptr<ExpressionNode>& expression_node, std::string& loop_label){}; // no-op

void IfElseNodeLoopLabelling(std::unique_ptr<IfElseNode>& ifelse_node, std::string& loop_label) {
    StatementNodeLoopLabelling(ifelse_node->if_block, loop_label);
    if (ifelse_node->else_block) {
        StatementNodeLoopLabelling(ifelse_node->else_block, loop_label);
    }
};

void CompoundNodeLoopLabelling(std::unique_ptr<CompoundNode>& compound_node, std::string& loop_label) { BlockNodeLoopLabelling(compound_node->block, loop_label); };

void BreakNodeLoopLabelling(std::unique_ptr<BreakNode>& break_node, std::string& loop_label) {
    if (loop_label.empty()) {
        throw std::runtime_error("Label Error: 'break' used outside of a loop");
    }
    break_node->label = std::make_unique<IdentifierNode>();
    break_node->label->name = loop_label;
}

void ContinueNodeLoopLabelling(std::unique_ptr<ContinueNode>& continue_node, std::string& loop_label) {
    if (loop_label.empty()) {
        throw std::runtime_error("Label Error: 'continue' used outside of a loop");
    }
    continue_node->label = std::make_unique<IdentifierNode>();
    continue_node->label->name = loop_label;
}

void WhileNodeLoopLabelling(std::unique_ptr<WhileNode>& while_node, std::string& loop_label) {
    std::string new_label = getLabelName("while");
    while_node->label = std::make_unique<IdentifierNode>();
    while_node->label->name = new_label;
    StatementNodeLoopLabelling(while_node->body, new_label);
}

void DoWhileNodeLoopLabelling(std::unique_ptr<DoWhileNode>& dowhile_node, std::string& loop_label) {
    std::string new_label = getLabelName("do_while");
    dowhile_node->label = std::make_unique<IdentifierNode>();
    dowhile_node->label->name = new_label;
    StatementNodeLoopLabelling(dowhile_node->body, new_label);
}

void ForNodeLoopLabelling(std::unique_ptr<ForNode>& for_node, std::string& loop_label) {
    std::string new_label = getLabelName("for");
    for_node->label = std::make_unique<IdentifierNode>();
    for_node->label->name = new_label;
    StatementNodeLoopLabelling(for_node->body, new_label);
}

void NullNodeLoopLabelling(std::unique_ptr<NullNode>& null_node, std::string& loop_label){}; // no-op
// loop labelling -- end

namespace nanocc {
    void semaLoopLabelling(std::unique_ptr<ProgramNode>& ast) {
        std::string loop_label = "";
        ProgramNodeLoopLabelling(ast, loop_label);
    }
} // namespace nanocc
