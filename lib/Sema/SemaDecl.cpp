#include <cassert>

#include "nanocc/Utils.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Sema/Sema.hpp"

#include "SemaHelper.hpp"

namespace { // helper functions
IdentifierMap copyIdentifierMapForNewScope(const IdentifierMap& old_sym_table,
                                           bool curr_scope = false) {
    IdentifierMap new_sym_table = old_sym_table;
    for (const auto& [var_name, var_scope] : old_sym_table) {
        new_sym_table[var_name].from_curr_scope = curr_scope;
    }
    return new_sym_table;
}

/// @brief check for variable redeclaration in the same scope.
/// add to symbol table after giving unique name;
void resolveVariableIdentifiers(IdentifierMap& identifier_map,
                                std::unique_ptr<IdentifierNode>& var_name) {
    if (identifier_map.contains(var_name->name) && identifier_map[var_name->name].from_curr_scope) {
        throw std::runtime_error(
            std::format("Type Error: Redeclaration of parameter '{}'", var_name->name));
    }
    std::string unique_name = getUniqueName(var_name->name);
    identifier_map[var_name->name] =
        (VariableScope){unique_name, /*from_curr_scope=*/true, /*external_linkage=*/false};
    var_name->name = unique_name; // update parameter name too
}
} // namespace

namespace sema {
// identifier resolution -- start
void programNodeResolveTypes(std::unique_ptr<ProgramNode>& program_node,
                             IdentifierMap& identifier_map) {
    // both function definitions and declarations are handled here
    for (auto& function : program_node->functions) {
        functionDeclNodeResolveTypes(function, identifier_map);
    }
}

void declarationNodeResolveTypes(std::unique_ptr<DeclarationNode>& declaration_node,
                                 IdentifierMap& identifier_map) {
    if (declaration_node->func) {
        // only function declarations (not definitions) are allowed inside Blocks/BlockItems
        if (declaration_node->func->body) {
            throw std::runtime_error(std::format(
                "Type Error: Defined function '{}' inside a BlockItem, define at top level",
                declaration_node->func->func_name->name));
        }
        functionDeclNodeResolveTypes(declaration_node->func, identifier_map);
    } else if (declaration_node->var) {
        variableDeclNodeResolveTypes(declaration_node->var, identifier_map);
    }
}

/// @brief resolve variable identifiers `resolveVariableIdentifiers`;
/// resolve the optional initializer expression
void variableDeclNodeResolveTypes(std::unique_ptr<VariableDeclNode>& variable_decl_node,
                                  IdentifierMap& identifier_map) {
    resolveVariableIdentifiers(identifier_map, variable_decl_node->var_identifier);
    if (variable_decl_node->init_expr) {
        exprNodeResolveTypes(variable_decl_node->init_expr, identifier_map);
    }
}

/*```
// no other declarations/definitions of foo in the same scope
// no issues
int foo(void);
int main(void) {
    return foo();
}
```
```
// redeclarations in different scopes is okay
// new declaration shall shadow previous one
int foo(void);
int main(void) {
    int foo(void);
    return foo();
}
```
```
// if redeclarations in the same scope is an error UNLESS they have external linkage
int main(void) {
    int foo(void);
    int foo(void);
    return foo();
}
// that's why the below program is INVALID
int main(void) {
    static int foo(void); // although static not added yet... just giving an example
    int foo(void); // even if you delete this line, still INVALID
    return foo();
}
```*/
void functionDeclNodeResolveTypes(std::unique_ptr<FunctionDeclNode>& function_decl_node,
                                  IdentifierMap& identifier_map) {
    if (identifier_map.contains(function_decl_node->func_name->name)) {
        auto& prev_entry = identifier_map[function_decl_node->func_name->name];
        if (prev_entry.from_curr_scope && !prev_entry.external_linkage) {
            throw std::runtime_error(
                std::format("Type Error: Redeclaration of function '{}' with no external linkage",
                            function_decl_node->func_name->name));
        }
    }

    // external linkage functions don't change their names
    function_decl_node->func_name->name = function_decl_node->func_name->name; // simulate no change
    identifier_map[function_decl_node->func_name->name] =
        (VariableScope){function_decl_node->func_name->name, true, /*external_linkage=*/true};

    // create new scope for function params and body
    // they will share the same scope therefore below will give error
    // `int foo(int a){ int a = 2; }`
    IdentifierMap new_sym_table =
        copyIdentifierMapForNewScope(identifier_map, /*curr_scope=*/false);
    // resolve parameter identifiers
    for (auto& param : function_decl_node->parameters) {
        resolveVariableIdentifiers(new_sym_table, param);
    }

    if (function_decl_node->body) {
        blockNodeResolveTypes(function_decl_node->body, new_sym_table);
    }
}

void blockNodeResolveTypes(std::unique_ptr<BlockNode>& block_node, IdentifierMap& identifier_map) {
    for (const auto& block_item : block_node->block_items) {
        blockItemNodeResolveTypes(block_item, identifier_map);
    }
}

void blockItemNodeResolveTypes(const std::unique_ptr<BlockItemNode>& block_item_node,
                               IdentifierMap& identifier_map) {
    if (block_item_node->declaration) {
        declarationNodeResolveTypes(block_item_node->declaration, identifier_map);
    } else if (block_item_node->statement) {
        statementNodeResolveTypes(block_item_node->statement, identifier_map);
    }
}

void statementNodeResolveTypes(std::unique_ptr<StatementNode>& statement_node,
                               IdentifierMap& identifier_map) {
    if (statement_node->return_stmt) {
        returnNodeResolveTypes(statement_node->return_stmt, identifier_map);
    } else if (statement_node->expression_stmt) {
        expressionNodeResolveTypes(statement_node->expression_stmt, identifier_map);
    } else if (statement_node->ifelse_stmt) {
        ifElseNodeResolveTypes(statement_node->ifelse_stmt, identifier_map);
    } else if (statement_node->compound_stmt) {
        compoundNodeResolveTypes(statement_node->compound_stmt, identifier_map);
    } else if (statement_node->break_stmt) {
        breakNodeResolveTypes(statement_node->break_stmt, identifier_map); // no-op
    } else if (statement_node->continue_stmt) {
        continueNodeResolveTypes(statement_node->continue_stmt, identifier_map); // no-op
    } else if (statement_node->while_stmt) {
        whileNodeResolveTypes(statement_node->while_stmt, identifier_map);
    } else if (statement_node->dowhile_stmt) {
        doWhileNodeResolveTypes(statement_node->dowhile_stmt, identifier_map);
    } else if (statement_node->for_stmt) {
        forNodeResolveTypes(statement_node->for_stmt, identifier_map);
    } else if (statement_node->null_stmt) {
        nullNodeResolveTypes(statement_node->null_stmt, identifier_map); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void returnNodeResolveTypes(std::unique_ptr<ReturnNode>& return_node,
                            IdentifierMap& identifier_map) {
    exprNodeResolveTypes(return_node->ret_expr, identifier_map);
}

void expressionNodeResolveTypes(std::unique_ptr<ExpressionNode>& expression_node,
                                IdentifierMap& identifier_map) {
    exprNodeResolveTypes(expression_node->expr, identifier_map);
}

void ifElseNodeResolveTypes(std::unique_ptr<IfElseNode>& ifelse_node,
                            IdentifierMap& identifier_map) {
    exprNodeResolveTypes(ifelse_node->condition, identifier_map);
    statementNodeResolveTypes(ifelse_node->if_block, identifier_map);
    if (ifelse_node->else_block) {
        statementNodeResolveTypes(ifelse_node->else_block, identifier_map);
    }
}

/// @brief Create a new scope, i.e create a copy of the current symbol table and
/// mark all variables from the parent scope as false as they are not from current block scope
void compoundNodeResolveTypes(std::unique_ptr<CompoundNode>& compound_node,
                              IdentifierMap& old_sym_table) {
    IdentifierMap new_sym_table = copyIdentifierMapForNewScope(old_sym_table, /*curr_scope=*/false);
    blockNodeResolveTypes(compound_node->block, new_sym_table);
}

void breakNodeResolveTypes(std::unique_ptr<BreakNode>& break_node,
                           IdentifierMap& identifier_map){}; // no-op

void continueNodeResolveTypes(std::unique_ptr<ContinueNode>& continue_node,
                              IdentifierMap& identifier_map){}; // no-op

void whileNodeResolveTypes(std::unique_ptr<WhileNode>& while_node, IdentifierMap& identifier_map) {
    exprNodeResolveTypes(while_node->condition, identifier_map);
    statementNodeResolveTypes(while_node->body, identifier_map);
}

void doWhileNodeResolveTypes(std::unique_ptr<DoWhileNode>& dowhile_node,
                             IdentifierMap& identifier_map) {
    statementNodeResolveTypes(dowhile_node->body, identifier_map);
    exprNodeResolveTypes(dowhile_node->condition, identifier_map);
}

void forNodeResolveTypes(std::unique_ptr<ForNode>& for_node, IdentifierMap& identifier_map) {
    // create a new scope for the for-loop
    IdentifierMap new_sym_table =
        copyIdentifierMapForNewScope(identifier_map, /*curr_scope=*/false);
    forInitNodeResolveTypes(for_node->init, new_sym_table);
    if (for_node->condition) {
        exprNodeResolveTypes(for_node->condition, new_sym_table);
    }
    if (for_node->post) {
        exprNodeResolveTypes(for_node->post, new_sym_table);
    }
    // `statementNodeResolveTypes` =calls=> `compoundNodeResolveTypes`
    // => creates another new scope for the body
    // no need to do anything here for that
    statementNodeResolveTypes(for_node->body, new_sym_table);
}

void forInitNodeResolveTypes(std::unique_ptr<ForInitNode>& for_init_node,
                             IdentifierMap& identifier_map) {
    if (for_init_node->declaration) {
        variableDeclNodeResolveTypes(for_init_node->declaration, identifier_map);
    } else if (for_init_node->init_expr) {
        exprNodeResolveTypes(for_init_node->init_expr, identifier_map);
    }
}

void nullNodeResolveTypes(std::unique_ptr<NullNode>& null_node,
                          IdentifierMap& identifier_map){}; // no-op

void exprNodeResolveTypes(std::unique_ptr<ExprNode>& expr_node, IdentifierMap& identifier_map) {
    exprFactorNodeResolveTypes(expr_node->left_exprf, identifier_map);
}

void exprFactorNodeResolveTypes(std::unique_ptr<ExprFactorNode>& expr_factor_node,
                                IdentifierMap& identifier_map) {
    if (expr_factor_node->var_identifier) {
        varNodeResolveTypes(expr_factor_node->var_identifier, identifier_map);
    } else if (expr_factor_node->unary) {
        unaryNodeResolveTypes(expr_factor_node->unary, identifier_map);
    } else if (expr_factor_node->expr) {
        exprNodeResolveTypes(expr_factor_node->expr, identifier_map);
    } else if (expr_factor_node->constant) {
        constantNodeResolveTypes(expr_factor_node->constant, identifier_map); // no-op
    } else if (expr_factor_node->func_call) {
        functionCallNodeResolveTypes(expr_factor_node->func_call, identifier_map);
    } else if (isa<BinaryNode>(expr_factor_node.get())) {
        binaryNodeResolveTypes(reinterpret_cast<std::unique_ptr<BinaryNode>&>(expr_factor_node),
                               identifier_map);
    } else if (isa<AssignmentNode>(expr_factor_node.get())) {
        assignmentNodeResolveTypes(
            reinterpret_cast<std::unique_ptr<AssignmentNode>&>(expr_factor_node), identifier_map);
    } else if (isa<ConditionalNode>(expr_factor_node.get())) {
        conditionalNodeResolveTypes(
            reinterpret_cast<std::unique_ptr<ConditionalNode>&>(expr_factor_node), identifier_map);
    } else {
        throw std::runtime_error("Type Error: Malformed ExprFactorNode");
    }
}

/// @brief Should already be added to the symbol table by `variableDeclNodeResolveTypes`
void varNodeResolveTypes(std::unique_ptr<VarNode>& var_node, IdentifierMap& identifier_map) {
    if (identifier_map.contains(var_node->var_name->name)) {
        var_node->var_name->name = identifier_map[var_node->var_name->name].unique_name;
    } else {
        throw std::runtime_error(
            std::format("Type Error: Undeclared variable '{}'", var_node->var_name->name));
    }
}

void constantNodeResolveTypes(std::unique_ptr<ConstantNode>& constant_node,
                              IdentifierMap& identifier_map){}; // no-op

void binaryNodeResolveTypes(std::unique_ptr<BinaryNode>& binary_node,
                            IdentifierMap& identifier_map) {
    exprNodeResolveTypes(binary_node->left_expr, identifier_map);
    exprNodeResolveTypes(binary_node->right_expr, identifier_map);
}

/// @brief `AssignmentNode::left_expr` must be a `VarNode` else throw type error
void assignmentNodeResolveTypes(std::unique_ptr<AssignmentNode>& assignment_node,
                                IdentifierMap& identifier_map) {
    assert(assignment_node->left_expr && assignment_node->left_expr->left_exprf &&
           "Left expression or its factor is null in `assignmentNodeResolveTypes`");
    auto left_factor = assignment_node->left_expr->left_exprf.get();
    if (!left_factor->var_identifier) {
        throw std::runtime_error("Type Error: Left-hand side of assignment must be a variable");
    }

    exprNodeResolveTypes(assignment_node->left_expr, identifier_map);
    exprNodeResolveTypes(assignment_node->right_expr, identifier_map);
}

void conditionalNodeResolveTypes(std::unique_ptr<ConditionalNode>& conditional_node,
                                 IdentifierMap& identifier_map) {
    exprNodeResolveTypes(conditional_node->condition, identifier_map);
    exprNodeResolveTypes(conditional_node->true_expr, identifier_map);
    exprNodeResolveTypes(conditional_node->false_expr, identifier_map);
}

void functionCallNodeResolveTypes(std::unique_ptr<FunctionCallNode>& function_call_node,
                                  IdentifierMap& identifier_map) {
    // function name must be declared in symbol table by `functionDeclNodeResolveTypes`
    if (!identifier_map.contains(function_call_node->func_identifier->name)) {
        throw std::runtime_error(std::format("Type Error: Calling undeclared function '{}'",
                                             function_call_node->func_identifier->name));
    }
    // functions with external linkage will have same name
    // only internal linkage functions will get new unique names
    function_call_node->func_identifier->name =
        identifier_map[function_call_node->func_identifier->name].unique_name;
    for (auto& arg : function_call_node->arguments) {
        exprNodeResolveTypes(arg, identifier_map);
    }
}

/*
- detect errors of type `<unary_op> <exprfactor> = <expr>`
- eg: `ar x /usr/lib/x86_64-linux-gnu/libc.a putchar.o = 3` ==parsed_as=> `UnaryNode('!',
AssignmentNode(VarNode('a'), ConstantNode('3')))`;
*/
void unaryNodeResolveTypes(std::unique_ptr<UnaryNode>& unary_node, IdentifierMap& identifier_map) {
    assert(unary_node->operand && "Operand is null in `unaryNodeResolveTypes`");
    if (isa<AssignmentNode>(unary_node->operand.get())) {
        throw std::runtime_error("Type Error: Cannot assign to the result of a unary operation");
    }
    exprFactorNodeResolveTypes(unary_node->operand, identifier_map);
}
// identifier resolution -- end
} // namespace sema

namespace nanocc {
void semaIdentifierResolution(std::unique_ptr<ProgramNode>& program_node) {
    IdentifierMap identifier_map;
    sema::programNodeResolveTypes(program_node, identifier_map);
}
} // namespace nanocc
