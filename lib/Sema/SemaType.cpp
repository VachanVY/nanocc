#include "nanocc/Utils.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"

#include "SemaHelper.hpp"

// check types -- start
void ProgramNodeCheckTypes(std::unique_ptr<ProgramNode>& program_node, TypeCheckerSymbolTable& type_checker_map) {
    for (auto& func : program_node->functions) {
        FunctionDeclNodeCheckTypes(func, type_checker_map);
    }
}

void DeclarationNodeCheckTypes(std::unique_ptr<DeclarationNode>& declaration_node, TypeCheckerSymbolTable& type_checker_map) {
    if (declaration_node->func) {
        FunctionDeclNodeCheckTypes(declaration_node->func, type_checker_map);
    } else if (declaration_node->var) {
        VariableDeclNodeCheckTypes(declaration_node->var, type_checker_map);
    }
}

void VariableDeclNodeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node, TypeCheckerSymbolTable& type_checker_map) {
    type_checker_map[variable_decl_node->var_identifier->name] = IntType{};
    if (variable_decl_node->init_expr) {
        ExprNodeCheckTypes(variable_decl_node->init_expr, type_checker_map);
    }
}

void FunctionDeclNodeCheckTypes(std::unique_ptr<FunctionDeclNode>& function_decl_node, TypeCheckerSymbolTable& type_checker_map) {
    bool has_body = (function_decl_node->body != nullptr);
    bool already_defined = false;

    // has been declared/defined before
    if (type_checker_map.contains(function_decl_node->func_name->name)) {
        auto& existing_type = type_checker_map[function_decl_node->func_name->name];
        if (FuncType* func_type = std::get_if<FuncType>(&existing_type)) {
            already_defined = func_type->defined;
            // if already defined and trying to define again, raise error
            if (has_body && already_defined) {
                throw std::runtime_error(std::format("Type Error: Redefinition of function '{}'",
                                                     function_decl_node->func_name->name));
            }
            if (func_type->param_types.size() != function_decl_node->parameters.size()) {
                throw std::runtime_error(std::format("Type Error: Conflicting number of parameters "
                                                     "in declarations for function '{}'",
                                                     function_decl_node->func_name->name));
            }
        } else { // existing_type is not FuncType
            throw std::runtime_error(std::format("Type Error: Conflicting types for function '{}'",
                                                 function_decl_node->func_name->name));
        }
    }
    auto param_types = std::vector<std::unique_ptr<Type>>{};
    for (auto& param : function_decl_node->parameters) {
        // for now, only IntType parameters are supported
        param_types.push_back(std::make_unique<Type>(IntType{}));
    }
    // add/update FuncType in type checker symbol table
    type_checker_map[function_decl_node->func_name->name] = FuncType{
        .param_types = std::move(param_types),
        .return_type = std::make_unique<Type>(IntType{}),
        .defined = has_body || already_defined,
    };

    if (has_body) {
        for (const auto& param : function_decl_node->parameters) {
            type_checker_map[param->name] = IntType{};
        }
        BlockNodeCheckTypes(function_decl_node->body, type_checker_map);
    }
}

void BlockNodeCheckTypes(std::unique_ptr<BlockNode>& block_node, TypeCheckerSymbolTable& type_checker_map) {
    for (const auto& block_item : block_node->block_items) {
        BlockItemNodeCheckTypes(block_item, type_checker_map);
    }
}

void BlockItemNodeCheckTypes(const std::unique_ptr<BlockItemNode>& block_item_node, TypeCheckerSymbolTable& type_checker_map) {
    if (block_item_node->declaration) {
        DeclarationNodeCheckTypes(block_item_node->declaration, type_checker_map);
    } else if (block_item_node->statement) {
        StatementNodeCheckTypes(block_item_node->statement, type_checker_map);
    }
}

void StatementNodeCheckTypes(std::unique_ptr<StatementNode>& statement_node, TypeCheckerSymbolTable& type_checker_map) {
    if (statement_node->return_stmt) {
        ReturnNodeCheckTypes(statement_node->return_stmt, type_checker_map);
    } else if (statement_node->expression_stmt) {
        ExpressionNodeCheckTypes(statement_node->expression_stmt, type_checker_map);
    } else if (statement_node->ifelse_stmt) {
        IfElseNodeCheckTypes(statement_node->ifelse_stmt, type_checker_map);
    } else if (statement_node->compound_stmt) {
        CompoundNodeCheckTypes(statement_node->compound_stmt, type_checker_map);
    } else if (statement_node->break_stmt) {
        BreakNodeCheckTypes(statement_node->break_stmt, type_checker_map); // no-op
    } else if (statement_node->continue_stmt) {
        ContinueNodeCheckTypes(statement_node->continue_stmt, type_checker_map); // no-op
    } else if (statement_node->while_stmt) {
        WhileNodeCheckTypes(statement_node->while_stmt, type_checker_map);
    } else if (statement_node->dowhile_stmt) {
        DoWhileNodeCheckTypes(statement_node->dowhile_stmt, type_checker_map);
    } else if (statement_node->for_stmt) {
        ForNodeCheckTypes(statement_node->for_stmt, type_checker_map);
    } else if (statement_node->null_stmt) {
        NullNodeCheckTypes(statement_node->null_stmt, type_checker_map); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void ReturnNodeCheckTypes(std::unique_ptr<ReturnNode>& return_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(return_node->ret_expr, type_checker_map);
}

void ExpressionNodeCheckTypes(std::unique_ptr<ExpressionNode>& expression_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(expression_node->expr, type_checker_map);
}

void IfElseNodeCheckTypes(std::unique_ptr<IfElseNode>& ifelse_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(ifelse_node->condition, type_checker_map);
    StatementNodeCheckTypes(ifelse_node->if_block, type_checker_map);
    if (ifelse_node->else_block) {
        StatementNodeCheckTypes(ifelse_node->else_block, type_checker_map);
    }
}

void CompoundNodeCheckTypes(std::unique_ptr<CompoundNode>& compound_node, TypeCheckerSymbolTable& type_checker_map) {
    BlockNodeCheckTypes(compound_node->block, type_checker_map);
}

void BreakNodeCheckTypes(std::unique_ptr<BreakNode>& break_node, TypeCheckerSymbolTable& type_checker_map){};    // no-op
void ContinueNodeCheckTypes(std::unique_ptr<ContinueNode>& continue_node, TypeCheckerSymbolTable& type_checker_map){}; // no-op

void WhileNodeCheckTypes(std::unique_ptr<WhileNode>& while_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(while_node->condition, type_checker_map);
    StatementNodeCheckTypes(while_node->body, type_checker_map);
}

void DoWhileNodeCheckTypes(std::unique_ptr<DoWhileNode>& dowhile_node, TypeCheckerSymbolTable& type_checker_map) {
    StatementNodeCheckTypes(dowhile_node->body, type_checker_map);
    ExprNodeCheckTypes(dowhile_node->condition, type_checker_map);
}

void ForNodeCheckTypes(std::unique_ptr<ForNode>& for_node, TypeCheckerSymbolTable& type_checker_map) {
    ForInitNodeCheckTypes(for_node->init, type_checker_map);
    if (for_node->condition) {
        ExprNodeCheckTypes(for_node->condition, type_checker_map);
    }
    if (for_node->post) {
        ExprNodeCheckTypes(for_node->post, type_checker_map);
    }
    StatementNodeCheckTypes(for_node->body, type_checker_map);
}

void ForInitNodeCheckTypes(std::unique_ptr<ForInitNode>& for_init_node, TypeCheckerSymbolTable& type_checker_map) {
    if (for_init_node->declaration) {
        VariableDeclNodeCheckTypes(for_init_node->declaration, type_checker_map);
    } else if (for_init_node->init_expr) {
        ExprNodeCheckTypes(for_init_node->init_expr, type_checker_map);
    }
}

void NullNodeCheckTypes(std::unique_ptr<NullNode>& null_node, TypeCheckerSymbolTable& type_checker_map){}; // no-op

void ExprNodeCheckTypes(std::unique_ptr<ExprNode>& expr_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprFactorNodeCheckTypes(expr_node->left_exprf, type_checker_map);
}

void ExprFactorNodeCheckTypes(std::unique_ptr<ExprFactorNode>& expr_factor_node, TypeCheckerSymbolTable& type_checker_map) {
    if (expr_factor_node->var_identifier) {
        VarNodeCheckTypes(expr_factor_node->var_identifier, type_checker_map);
    } else if (expr_factor_node->unary) {
        UnaryNodeCheckTypes(expr_factor_node->unary, type_checker_map);
    } else if (expr_factor_node->expr) {
        ExprNodeCheckTypes(expr_factor_node->expr, type_checker_map);
    } else if (expr_factor_node->constant) {
        ConstantNodeCheckTypes(expr_factor_node->constant, type_checker_map); // no-op for now??
    } else if (expr_factor_node->func_call) {
        FunctionCallNodeCheckTypes(expr_factor_node->func_call, type_checker_map);
    } else if (auto* binary = dyn_cast<BinaryNode>(expr_factor_node.get())) {
        BinaryNodeCheckTypes(reinterpret_cast<std::unique_ptr<BinaryNode>&>(expr_factor_node), type_checker_map);
    } else if (auto* assignment = dyn_cast<AssignmentNode>(expr_factor_node.get())) {
        AssignmentNodeCheckTypes(reinterpret_cast<std::unique_ptr<AssignmentNode>&>(expr_factor_node), type_checker_map);
    } else if (auto* conditional = dyn_cast<ConditionalNode>(expr_factor_node.get())) {
        ConditionalNodeCheckTypes(reinterpret_cast<std::unique_ptr<ConditionalNode>&>(expr_factor_node), type_checker_map);
    } else {
        throw std::runtime_error("Type Error: Malformed ExprFactorNode");
    }
}

void VarNodeCheckTypes(std::unique_ptr<VarNode>& var_node, TypeCheckerSymbolTable& type_checker_map) {
    if (!std::holds_alternative<IntType>(type_checker_map[var_node->var_name->name])) {
        throw std::runtime_error(
            std::format("Type Error: Variable '{}' is not of type 'int'", var_node->var_name->name));
    }
}

void ConstantNodeCheckTypes(std::unique_ptr<ConstantNode>& constant_node, TypeCheckerSymbolTable& type_checker_map) {
    // constants are always of type int for now
}

void UnaryNodeCheckTypes(std::unique_ptr<UnaryNode>& unary_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprFactorNodeCheckTypes(unary_node->operand, type_checker_map);
}

void BinaryNodeCheckTypes(std::unique_ptr<BinaryNode>& binary_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(binary_node->left_expr, type_checker_map);
    ExprNodeCheckTypes(binary_node->right_expr, type_checker_map);
}

void AssignmentNodeCheckTypes(std::unique_ptr<AssignmentNode>& assignment_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(assignment_node->left_expr, type_checker_map);
    ExprNodeCheckTypes(assignment_node->right_expr, type_checker_map);
}

void FunctionCallNodeCheckTypes(std::unique_ptr<FunctionCallNode>& function_call_node, TypeCheckerSymbolTable& type_checker_map) {
    Type& caller_type = type_checker_map[function_call_node->func_identifier->name];
    if (std::holds_alternative<IntType>(caller_type)) {
        throw std::runtime_error(
            std::format("Type Error: Attempting to call non-function of type 'int' '{}'",
                        function_call_node->func_identifier->name));
    } else if (FuncType* func_type = std::get_if<FuncType>(&caller_type)) {
        if (func_type->param_types.size() != function_call_node->arguments.size()) {
            throw std::runtime_error(
                std::format("Type Error: Function '{}' expects {} arguments but {} were provided",
                            function_call_node->func_identifier->name, func_type->param_types.size(),
                            function_call_node->arguments.size()));
        }
        // for now, only IntType parameters are supported
        for (auto& arg : function_call_node->arguments) {
            ExprNodeCheckTypes(arg, type_checker_map);
        }
    } else {
        throw std::runtime_error(
            std::format("Type Error: Unknown type for function '{}'", function_call_node->func_identifier->name));
    }
}

void ConditionalNodeCheckTypes(std::unique_ptr<ConditionalNode>& conditional_node, TypeCheckerSymbolTable& type_checker_map) {
    ExprNodeCheckTypes(conditional_node->condition, type_checker_map);
    ExprNodeCheckTypes(conditional_node->true_expr, type_checker_map);
    ExprNodeCheckTypes(conditional_node->false_expr, type_checker_map);
}
// check types -- end

namespace nanocc {
    void semaCheckTypes(std::unique_ptr<ProgramNode> &ast, TypeCheckerSymbolTable& global_type_checker_map) {
        ProgramNodeCheckTypes(ast, global_type_checker_map);
    }
} // namespace nanocc
