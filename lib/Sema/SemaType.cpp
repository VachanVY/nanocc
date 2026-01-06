#include "nanocc/Utils.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"

#include "SemaHelper.hpp"

namespace sema {
// check types -- start
void programNodeCheckTypes(std::unique_ptr<ProgramNode>& program_node,
                           TypeCheckerSymbolTable& type_checker_map) {
    for (auto& func : program_node->functions) {
        functionDeclNodeCheckTypes(func, type_checker_map);
    }
}

void declarationNodeCheckTypes(std::unique_ptr<DeclarationNode>& declaration_node,
                               TypeCheckerSymbolTable& type_checker_map) {
    if (declaration_node->func) {
        functionDeclNodeCheckTypes(declaration_node->func, type_checker_map);
    } else if (declaration_node->var) {
        variableDeclNodeCheckTypes(declaration_node->var, type_checker_map);
    }
}

void variableDeclNodeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                TypeCheckerSymbolTable& type_checker_map) {
    type_checker_map[variable_decl_node->var_identifier->name] = IntType{};
    if (variable_decl_node->init_expr) {
        exprNodeCheckTypes(variable_decl_node->init_expr, type_checker_map);
    }
}

void functionDeclNodeCheckTypes(std::unique_ptr<FunctionDeclNode>& function_decl_node,
                                TypeCheckerSymbolTable& type_checker_map) {
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
        blockNodeCheckTypes(function_decl_node->body, type_checker_map);
    }
}

void blockNodeCheckTypes(std::unique_ptr<BlockNode>& block_node,
                         TypeCheckerSymbolTable& type_checker_map) {
    for (const auto& block_item : block_node->block_items) {
        blockItemNodeCheckTypes(block_item, type_checker_map);
    }
}

void blockItemNodeCheckTypes(const std::unique_ptr<BlockItemNode>& block_item_node,
                             TypeCheckerSymbolTable& type_checker_map) {
    if (block_item_node->declaration) {
        declarationNodeCheckTypes(block_item_node->declaration, type_checker_map);
    } else if (block_item_node->statement) {
        statementNodeCheckTypes(block_item_node->statement, type_checker_map);
    }
}

void statementNodeCheckTypes(std::unique_ptr<StatementNode>& statement_node,
                             TypeCheckerSymbolTable& type_checker_map) {
    if (statement_node->return_stmt) {
        returnNodeCheckTypes(statement_node->return_stmt, type_checker_map);
    } else if (statement_node->expression_stmt) {
        expressionNodeCheckTypes(statement_node->expression_stmt, type_checker_map);
    } else if (statement_node->ifelse_stmt) {
        ifElseNodeCheckTypes(statement_node->ifelse_stmt, type_checker_map);
    } else if (statement_node->compound_stmt) {
        compoundNodeCheckTypes(statement_node->compound_stmt, type_checker_map);
    } else if (statement_node->break_stmt) {
        breakNodeCheckTypes(statement_node->break_stmt, type_checker_map); // no-op
    } else if (statement_node->continue_stmt) {
        continueNodeCheckTypes(statement_node->continue_stmt, type_checker_map); // no-op
    } else if (statement_node->while_stmt) {
        whileNodeCheckTypes(statement_node->while_stmt, type_checker_map);
    } else if (statement_node->dowhile_stmt) {
        doWhileNodeCheckTypes(statement_node->dowhile_stmt, type_checker_map);
    } else if (statement_node->for_stmt) {
        forNodeCheckTypes(statement_node->for_stmt, type_checker_map);
    } else if (statement_node->null_stmt) {
        nullNodeCheckTypes(statement_node->null_stmt, type_checker_map); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void returnNodeCheckTypes(std::unique_ptr<ReturnNode>& return_node,
                          TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(return_node->ret_expr, type_checker_map);
}

void expressionNodeCheckTypes(std::unique_ptr<ExpressionNode>& expression_node,
                              TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(expression_node->expr, type_checker_map);
}

void ifElseNodeCheckTypes(std::unique_ptr<IfElseNode>& ifelse_node,
                          TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(ifelse_node->condition, type_checker_map);
    statementNodeCheckTypes(ifelse_node->if_block, type_checker_map);
    if (ifelse_node->else_block) {
        statementNodeCheckTypes(ifelse_node->else_block, type_checker_map);
    }
}

void compoundNodeCheckTypes(std::unique_ptr<CompoundNode>& compound_node,
                            TypeCheckerSymbolTable& type_checker_map) {
    blockNodeCheckTypes(compound_node->block, type_checker_map);
}

void breakNodeCheckTypes(std::unique_ptr<BreakNode>& break_node,
                         TypeCheckerSymbolTable& type_checker_map) {}; // no-op
void continueNodeCheckTypes(std::unique_ptr<ContinueNode>& continue_node,
                            TypeCheckerSymbolTable& type_checker_map) {}; // no-op

void whileNodeCheckTypes(std::unique_ptr<WhileNode>& while_node,
                         TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(while_node->condition, type_checker_map);
    statementNodeCheckTypes(while_node->body, type_checker_map);
}

void doWhileNodeCheckTypes(std::unique_ptr<DoWhileNode>& dowhile_node,
                           TypeCheckerSymbolTable& type_checker_map) {
    statementNodeCheckTypes(dowhile_node->body, type_checker_map);
    exprNodeCheckTypes(dowhile_node->condition, type_checker_map);
}

void forNodeCheckTypes(std::unique_ptr<ForNode>& for_node,
                       TypeCheckerSymbolTable& type_checker_map) {
    forInitNodeCheckTypes(for_node->init, type_checker_map);
    if (for_node->condition) {
        exprNodeCheckTypes(for_node->condition, type_checker_map);
    }
    if (for_node->post) {
        exprNodeCheckTypes(for_node->post, type_checker_map);
    }
    statementNodeCheckTypes(for_node->body, type_checker_map);
}

void forInitNodeCheckTypes(std::unique_ptr<ForInitNode>& for_init_node,
                           TypeCheckerSymbolTable& type_checker_map) {
    if (for_init_node->declaration) {
        variableDeclNodeCheckTypes(for_init_node->declaration, type_checker_map);
    } else if (for_init_node->init_expr) {
        exprNodeCheckTypes(for_init_node->init_expr, type_checker_map);
    }
}

void nullNodeCheckTypes(std::unique_ptr<NullNode>& null_node,
                        TypeCheckerSymbolTable& type_checker_map) {}; // no-op

void exprNodeCheckTypes(std::unique_ptr<ExprNode>& expr_node,
                        TypeCheckerSymbolTable& type_checker_map) {
    exprFactorNodeCheckTypes(expr_node->left_exprf, type_checker_map);
}

void exprFactorNodeCheckTypes(std::unique_ptr<ExprFactorNode>& expr_factor_node,
                              TypeCheckerSymbolTable& type_checker_map) {
    if (expr_factor_node->var_identifier) {
        varNodeCheckTypes(expr_factor_node->var_identifier, type_checker_map);
    } else if (expr_factor_node->unary) {
        unaryNodeCheckTypes(expr_factor_node->unary, type_checker_map);
    } else if (expr_factor_node->expr) {
        exprNodeCheckTypes(expr_factor_node->expr, type_checker_map);
    } else if (expr_factor_node->constant) {
        constantNodeCheckTypes(expr_factor_node->constant, type_checker_map); // no-op for now??
    } else if (expr_factor_node->func_call) {
        functionCallNodeCheckTypes(expr_factor_node->func_call, type_checker_map);
    } else if (auto* binary = dyn_cast<BinaryNode>(expr_factor_node.get())) {
        binaryNodeCheckTypes(reinterpret_cast<std::unique_ptr<BinaryNode>&>(expr_factor_node),
                             type_checker_map);
    } else if (auto* assignment = dyn_cast<AssignmentNode>(expr_factor_node.get())) {
        assignmentNodeCheckTypes(
            reinterpret_cast<std::unique_ptr<AssignmentNode>&>(expr_factor_node), type_checker_map);
    } else if (auto* conditional = dyn_cast<ConditionalNode>(expr_factor_node.get())) {
        conditionalNodeCheckTypes(
            reinterpret_cast<std::unique_ptr<ConditionalNode>&>(expr_factor_node),
            type_checker_map);
    } else {
        throw std::runtime_error("Type Error: Malformed ExprFactorNode");
    }
}

void varNodeCheckTypes(std::unique_ptr<VarNode>& var_node,
                       TypeCheckerSymbolTable& type_checker_map) {
    if (!std::holds_alternative<IntType>(type_checker_map[var_node->var_name->name])) {
        throw std::runtime_error(std::format("Type Error: Variable '{}' is not of type 'int'",
                                             var_node->var_name->name));
    }
}

void constantNodeCheckTypes(std::unique_ptr<ConstantNode>& constant_node,
                            TypeCheckerSymbolTable& type_checker_map) {
    // constants are always of type int for now
}

void unaryNodeCheckTypes(std::unique_ptr<UnaryNode>& unary_node,
                         TypeCheckerSymbolTable& type_checker_map) {
    exprFactorNodeCheckTypes(unary_node->operand, type_checker_map);
}

void binaryNodeCheckTypes(std::unique_ptr<BinaryNode>& binary_node,
                          TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(binary_node->left_expr, type_checker_map);
    exprNodeCheckTypes(binary_node->right_expr, type_checker_map);
}

void assignmentNodeCheckTypes(std::unique_ptr<AssignmentNode>& assignment_node,
                              TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(assignment_node->left_expr, type_checker_map);
    exprNodeCheckTypes(assignment_node->right_expr, type_checker_map);
}

void functionCallNodeCheckTypes(std::unique_ptr<FunctionCallNode>& function_call_node,
                                TypeCheckerSymbolTable& type_checker_map) {
    Type& caller_type = type_checker_map[function_call_node->func_identifier->name];
    if (std::holds_alternative<IntType>(caller_type)) {
        throw std::runtime_error(
            std::format("Type Error: Attempting to call non-function of type 'int' '{}'",
                        function_call_node->func_identifier->name));
    } else if (FuncType* func_type = std::get_if<FuncType>(&caller_type)) {
        if (func_type->param_types.size() != function_call_node->arguments.size()) {
            throw std::runtime_error(
                std::format("Type Error: Function '{}' expects {} arguments but {} were provided",
                            function_call_node->func_identifier->name,
                            func_type->param_types.size(), function_call_node->arguments.size()));
        }
        // for now, only IntType parameters are supported
        for (auto& arg : function_call_node->arguments) {
            exprNodeCheckTypes(arg, type_checker_map);
        }
    } else {
        throw std::runtime_error(std::format("Type Error: Unknown type for function '{}'",
                                             function_call_node->func_identifier->name));
    }
}

void conditionalNodeCheckTypes(std::unique_ptr<ConditionalNode>& conditional_node,
                               TypeCheckerSymbolTable& type_checker_map) {
    exprNodeCheckTypes(conditional_node->condition, type_checker_map);
    exprNodeCheckTypes(conditional_node->true_expr, type_checker_map);
    exprNodeCheckTypes(conditional_node->false_expr, type_checker_map);
}
// check types -- end
} // namespace sema

namespace nanocc {
void semaCheckTypes(std::unique_ptr<ProgramNode>& ast,
                    TypeCheckerSymbolTable& global_type_checker_map) {
    sema::programNodeCheckTypes(ast, global_type_checker_map);
}
} // namespace nanocc
