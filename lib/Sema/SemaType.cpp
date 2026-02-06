#include "nanocc/Utils.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"

#include "SemaHelper.hpp"

namespace Sema {
// check types -- start
void programNodeCheckTypes(std::unique_ptr<ProgramNode>& program_node,
                           TypeCheckerSymbolTable& type_checker_map) {
    for (auto& decl : program_node->declarations) {
        declarationNodeCheckTypes(decl, type_checker_map);
    }
}

void declarationNodeCheckTypes(std::unique_ptr<DeclarationNode>& declaration_node,
                               TypeCheckerSymbolTable& type_checker_map) {
    if (declaration_node->func) {
        functionDeclNodeCheckTypes(declaration_node->func, type_checker_map);
    } else if (declaration_node->var) {
        variableDeclNodeFileScopeCheckTypes(declaration_node->var, type_checker_map);
    }
}

static std::string isConstInitExpr(const VariableDeclNode& node) {
    if (node.init_expr && node.init_expr->left_exprf && node.init_expr->left_exprf->constant)
        return node.init_expr->left_exprf->constant->val;
    return "";
};

void variableDeclNodeFileScopeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                         TypeCheckerSymbolTable& type_checker_map) {
    InitValue init_value;
    auto const_val = isConstInitExpr(*variable_decl_node);
    if (!const_val.empty()) {
        // int b = 2;
        init_value = Initial{.value = const_val};
    } else if (!variable_decl_node->init_expr) {
        if (variable_decl_node->storage_class == StorageClass::Extern) {
            // extern int x;
            init_value = NoIntializer{};
        } else {
            // int x; // StorageClass::None
            init_value = Tentative{};
        }
    } else {
        throw std::runtime_error(
            std::format("Type Error: File scope variable '{}' must have a constant initializer or "
                        "be declared as extern or tentative",
                        variable_decl_node->var_identifier->name));
    }

    bool global = variable_decl_node->storage_class != StorageClass::Static;

    if (type_checker_map.contains(variable_decl_node->var_identifier->name)) {
        auto& prev_decl_entry = type_checker_map[variable_decl_node->var_identifier->name];
        auto prev_decl_attrs = std::get<StaticAttr>(prev_decl_entry.attrs);
        if (!std::holds_alternative<IntType>(prev_decl_entry.type)) { // not IntType
            throw std::runtime_error(std::format("Type Error: Conflicting types for variable '{}'",
                                                 variable_decl_node->var_identifier->name));
        }
        if (variable_decl_node->storage_class == StorageClass::Extern) {
            /*
            static int x;            extern int x;
            // previous linkage (i.e of `static int x`) is assigned to this

            *//*
            extern int x;            extern int x;
            */
            global = prev_decl_attrs.global;
        } else if (prev_decl_attrs.global != global) {
            /*
            > int x;               static int x;
            > static int x;        int x;
            > extern int x;        static int x;
            */
            throw std::runtime_error(
                std::format("Type Error: Conflicting linkage for variable '{}'",
                            variable_decl_node->var_identifier->name));
        }

        Initial* curr_init = std::get_if<Initial>(&init_value);
        if (Initial* prev_init = std::get_if<Initial>(&prev_decl_attrs.init)) {
            if (curr_init) {
                /*
                int x = 5;          int x = 10;
                */
                throw std::runtime_error(std::format("Type Error: Redefinition of variable '{}'",
                                                     variable_decl_node->var_identifier->name));
            } else {
                /*
                int x = 5;          int x;
                */
                init_value = *prev_init;
            }
        } else if (std::holds_alternative<Tentative>(prev_decl_attrs.init) && !curr_init) {
            /*
            int x;              int x;
            int x;              extern int x;
            */
            init_value = Tentative{};
        }
    }

    type_checker_map[variable_decl_node->var_identifier->name] =
        SymbolTableEntry{.type = IntType{},
                         .attrs = StaticAttr{
                             .init = init_value,
                             .global = global,
                         }};
}

void variableDeclNodeBlockScopeCheckTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                          TypeCheckerSymbolTable& type_checker_map) {
    //
    if (variable_decl_node->storage_class == StorageClass::Extern) {
        if (variable_decl_node->init_expr) {
            // extern int x = 5; // error, extern variables cannot have initializers
            throw std::runtime_error(std::format("Type Error: Block scope variable '{}' declared "
                                                 "as extern cannot have an initializer",
                                                 variable_decl_node->var_identifier->name));
        }
        if (type_checker_map.contains(variable_decl_node->var_identifier->name)) {
            auto& existing_entry = type_checker_map[variable_decl_node->var_identifier->name];
            if (!std::holds_alternative<IntType>(existing_entry.type)) {
                throw std::runtime_error(
                    std::format("Type Error: Conflicting types for variable '{}'",
                                variable_decl_node->var_identifier->name));
            }
        } else {
            // if not previously declared, add to symbol table with type IntType and no initializer
            type_checker_map[variable_decl_node->var_identifier->name] = {
                .type = IntType{},
                .attrs = StaticAttr{
                    .init = NoIntializer{},
                    .global = true,
                }};
        }
    } else if (variable_decl_node->storage_class == StorageClass::Static) {
        auto const_val = isConstInitExpr(*variable_decl_node);
        Initial init_value;
        if (!const_val.empty()) {
            /*
            { static int x = 5; }
            */
            init_value = Initial{.value = const_val};
        } else if (!variable_decl_node->init_expr) {
            /*
            { static int x; } // block scope static variables initialized to 0 by default
            */
            init_value = Initial{.value = "0"};
        } else {
            throw std::runtime_error(
                std::format("Type Error: Block scope variable '{}' declared as static must have a "
                            "constant initializer or no initializer",
                            variable_decl_node->var_identifier->name));
        }
        type_checker_map[variable_decl_node->var_identifier->name] = {.type = IntType{},
                                                                      .attrs = StaticAttr{
                                                                          .init = init_value,
                                                                          .global = false,
                                                                      }};
    } else {
        type_checker_map[variable_decl_node->var_identifier->name].type = IntType{};
        if (variable_decl_node->init_expr) {
            exprNodeCheckTypes(variable_decl_node->init_expr, type_checker_map);
        }
    }
}

void functionDeclNodeCheckTypes(std::unique_ptr<FunctionDeclNode>& function_decl_node,
                                TypeCheckerSymbolTable& type_checker_map) {
    bool has_body = (function_decl_node->body != nullptr);
    bool already_defined = false;
    bool global = function_decl_node->storage_class != StorageClass::Static;

    // has been declared/defined before
    if (type_checker_map.contains(function_decl_node->func_name->name)) {
        auto& existing_entry = type_checker_map[function_decl_node->func_name->name];
        if (FuncType* func_type = std::get_if<FuncType>(&existing_entry.type)) {
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
            /* functions can never change linkage.
            ```
            int foo(void); // external linkage
            static int foo(void) { ... } // error, trying to go from external to internal
            ```
            ```
            static int foo(void); // internal linkage
            int foo(void) { ... } // gets internal linkage from prior decl
            ```
            ```
            static int foo(void); // internal linkage
            extern int foo(void); // gets internal linkage from prior decl
            ```
            */
            bool existing_global = std::get<FuncAttr>(existing_entry.attrs).global;

            if (function_decl_node->storage_class == StorageClass::Static) {
                if (existing_global) {
                    // trying to change from external to internal - error
                    throw std::runtime_error(
                        std::format("Type Error: Conflicting linkage for function '{}'",
                                    function_decl_node->func_name->name));
                }
                global = false;
            } else {
                // 'extern' or no storage class - adopt existing linkage
                global = existing_global;
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
    type_checker_map[function_decl_node->func_name->name] = {
        .type =
            FuncType{
                .param_types = std::move(param_types),
                .return_type = std::make_unique<Type>(IntType{}),
                .defined = has_body || already_defined,
            },
        .attrs = FuncAttr{
            .defined = has_body || already_defined,
            .global = global,
        }};

    if (has_body) {
        for (const auto& param : function_decl_node->parameters) {
            type_checker_map[param->name].type = IntType{};
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
        if (block_item_node->declaration->func) {
            functionDeclNodeCheckTypes(block_item_node->declaration->func, type_checker_map);
        } else if (block_item_node->declaration->var) {
            variableDeclNodeBlockScopeCheckTypes(block_item_node->declaration->var,
                                                 type_checker_map);
        }
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
                         TypeCheckerSymbolTable& type_checker_map){}; // no-op
void continueNodeCheckTypes(std::unique_ptr<ContinueNode>& continue_node,
                            TypeCheckerSymbolTable& type_checker_map){}; // no-op

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
        auto& variable_decl_node = for_init_node->declaration;
        /* ERROR cases:
        for (extern int x; i < 10; i++) { ... }
        for (static int x = 5; i < 10; i++) { ... }
        */
        if (variable_decl_node->storage_class != StorageClass::None) {
            throw std::runtime_error(std::format("Type Error: For loop initializer variable '{}' "
                                                 "cannot have storage class specifier",
                                                 variable_decl_node->var_identifier->name));
        }
        variableDeclNodeBlockScopeCheckTypes(variable_decl_node, type_checker_map);
    } else if (for_init_node->init_expr) {
        exprNodeCheckTypes(for_init_node->init_expr, type_checker_map);
    }
}

void nullNodeCheckTypes(std::unique_ptr<NullNode>& null_node,
                        TypeCheckerSymbolTable& type_checker_map){}; // no-op

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
    if (!std::holds_alternative<IntType>(type_checker_map[var_node->var_name->name].type)) {
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
    Type& caller_type = type_checker_map[function_call_node->func_identifier->name].type;
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
} // namespace Sema

namespace nanocc {
void semaCheckTypes(std::unique_ptr<ProgramNode>& ast,
                    TypeCheckerSymbolTable& global_type_checker_map) {
    Sema::programNodeCheckTypes(ast, global_type_checker_map);
}
} // namespace nanocc
