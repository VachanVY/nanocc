#include "include/utils.hpp"
#include "include/parser.hpp"
#include "include/checker.hpp"

TypeCheckerSymbolTable global_type_checker_map;

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

// identifier resolution -- start
void ProgramNode::resolveTypes(IdentifierMap& identifier_map) {
    // both function definitions and declarations are handled here
    for (auto& functions : this->functions) {
        functions->resolveTypes(identifier_map);
    }
}

void DeclarationNode::resolveTypes(IdentifierMap& identifier_map) {
    if (this->func) {
        // only function declarations (not definitions) are allowed inside Blocks/BlockItems
        if (this->func->body) {
            throw std::runtime_error(std::format(
                "Type Error: Defined function '{}' inside a BlockItem, define at top level",
                this->func->func_name->name));
        }
        this->func->resolveTypes(identifier_map);
    } else if (this->var) {
        this->var->resolveTypes(identifier_map);
    }
}

/// @brief resolve variable identifiers `resolveVariableIdentifiers`;
/// resolve the optional initializer expression
void VariableDeclNode::resolveTypes(IdentifierMap& identifier_map) {
    resolveVariableIdentifiers(identifier_map, this->var_identifier);
    if (this->init_expr) {
        this->init_expr->resolveTypes(identifier_map);
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
void FunctionDeclNode::resolveTypes(IdentifierMap& identifier_map) {
    if (identifier_map.contains(this->func_name->name)) {
        auto& prev_entry = identifier_map[this->func_name->name];
        if (prev_entry.from_curr_scope && !prev_entry.external_linkage) {
            throw std::runtime_error(
                std::format("Type Error: Redeclaration of function '{}' with no external linkage",
                            this->func_name->name));
        }
    }

    // external linkage functions don't change their names
    this->func_name->name = this->func_name->name; // simulate no change
    identifier_map[this->func_name->name] =
        (VariableScope){this->func_name->name, true, /*external_linkage=*/true};

    // create new scope for function params and body
    // they will share the same scope therefore below will give error
    // `int foo(int a){ int a = 2; }`
    IdentifierMap new_sym_table =
        copyIdentifierMapForNewScope(identifier_map, /*curr_scope=*/false);
    // resolve parameter identifiers
    for (auto& param : this->parameters) {
        resolveVariableIdentifiers(new_sym_table, param);
    }

    if (this->body) {
        this->body->resolveTypes(new_sym_table);
    }
}

void BlockNode::resolveTypes(IdentifierMap& identifier_map) {
    for (const auto& block_item : this->block_items) {
        block_item->resolveTypes(identifier_map);
    }
}

void BlockItemNode::resolveTypes(IdentifierMap& identifier_map) {
    if (this->declaration) {
        this->declaration->resolveTypes(identifier_map);
    } else if (this->statement) {
        this->statement->resolveTypes(identifier_map);
    }
}

void StatementNode::resolveTypes(IdentifierMap& identifier_map) {
    if (this->return_stmt) {
        this->return_stmt->resolveTypes(identifier_map);
    } else if (this->expression_stmt) {
        this->expression_stmt->resolveTypes(identifier_map);
    } else if (this->ifelse_stmt) {
        this->ifelse_stmt->resolveTypes(identifier_map);
    } else if (this->compound_stmt) {
        this->compound_stmt->resolveTypes(identifier_map);
    } else if (this->break_stmt) {
        this->break_stmt->resolveTypes(identifier_map); // no-op
    } else if (this->continue_stmt) {
        this->continue_stmt->resolveTypes(identifier_map); // no-op
    } else if (this->while_stmt) {
        this->while_stmt->resolveTypes(identifier_map);
    } else if (this->dowhile_stmt) {
        this->dowhile_stmt->resolveTypes(identifier_map);
    } else if (this->for_stmt) {
        this->for_stmt->resolveTypes(identifier_map);
    } else if (this->null_stmt) {
        this->null_stmt->resolveTypes(identifier_map); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void ReturnNode::resolveTypes(IdentifierMap& identifier_map) {
    this->ret_expr->resolveTypes(identifier_map);
}

void ExpressionNode::resolveTypes(IdentifierMap& identifier_map) {
    this->expr->resolveTypes(identifier_map);
}

void IfElseNode::resolveTypes(IdentifierMap& identifier_map) {
    this->condition->resolveTypes(identifier_map);
    this->if_block->resolveTypes(identifier_map);
    if (this->else_block) {
        this->else_block->resolveTypes(identifier_map);
    }
}

/// @brief Create a new scope, i.e create a copy of the current symbol table and
/// mark all variables from the parent scope as false as they are not from current block scope
void CompoundNode::resolveTypes(IdentifierMap& old_sym_table) {
    IdentifierMap new_sym_table = copyIdentifierMapForNewScope(old_sym_table, /*curr_scope=*/false);
    this->block->resolveTypes(new_sym_table);
}

void BreakNode::resolveTypes(IdentifierMap& identifier_map){}; // no-op

void ContinueNode::resolveTypes(IdentifierMap& identifier_map){}; // no-op

void WhileNode::resolveTypes(IdentifierMap& identifier_map) {
    this->condition->resolveTypes(identifier_map);
    this->body->resolveTypes(identifier_map);
}

void DoWhileNode::resolveTypes(IdentifierMap& identifier_map) {
    this->body->resolveTypes(identifier_map);
    this->condition->resolveTypes(identifier_map);
}

void ForNode::resolveTypes(IdentifierMap& identifier_map) {
    // create a new scope for the for-loop
    IdentifierMap new_sym_table =
        copyIdentifierMapForNewScope(identifier_map, /*curr_scope=*/false);
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

void ForInitNode::resolveTypes(IdentifierMap& identifier_map) {
    if (this->declaration) {
        this->declaration->resolveTypes(identifier_map);
    } else if (this->init_expr) {
        this->init_expr->resolveTypes(identifier_map);
    }
}

void NullNode::resolveTypes(IdentifierMap& identifier_map){}; // no-op

void ExprNode::resolveTypes(IdentifierMap& identifier_map) {
    this->left_exprf->resolveTypes(identifier_map);
}

void ExprFactorNode::resolveTypes(IdentifierMap& identifier_map) {
    if (this->var_identifier) {
        this->var_identifier->resolveTypes(identifier_map);
    } else if (this->unary) {
        this->unary->resolveTypes(identifier_map);
    } else if (this->expr) {
        this->expr->resolveTypes(identifier_map);
    } else if (this->constant) {
        this->constant->resolveTypes(identifier_map); // no-op
    } else if (this->func_call) {
        this->func_call->resolveTypes(identifier_map);
    } else {
        throw std::runtime_error("Type Error: Malformed ExprFactorNode");
    }
}

/// @brief Should already be added to the symbol table by `VariableDeclNode`
void VarNode::resolveTypes(IdentifierMap& identifier_map) {
    if (identifier_map.contains(this->var_name->name)) {
        this->var_name->name = identifier_map[this->var_name->name].unique_name;
    } else {
        throw std::runtime_error(
            std::format("Type Error: Undeclared variable '{}'", this->var_name->name));
    }
}

void BinaryNode::resolveTypes(IdentifierMap& identifier_map) {
    this->left_expr->resolveTypes(identifier_map);
    this->right_expr->resolveTypes(identifier_map);
}

/// @brief `AssignmentNode::left_expr` must be a `VarNode` else throw type error
void AssignmentNode::resolveTypes(IdentifierMap& identifier_map) {
    assert(this->left_expr && this->left_expr->left_exprf &&
           "Left expression or its factor is null in `AssignmentNode::resolveTypes`");
    auto left_factor = this->left_expr->left_exprf.get();
    if (!left_factor->var_identifier) {
        throw std::runtime_error("Type Error: Left-hand side of assignment must be a variable");
    }

    this->left_expr->resolveTypes(identifier_map);
    this->right_expr->resolveTypes(identifier_map);
}

void ConditionalNode::resolveTypes(IdentifierMap& identifier_map) {
    this->condition->resolveTypes(identifier_map);
    this->true_expr->resolveTypes(identifier_map);
    this->false_expr->resolveTypes(identifier_map);
}

void FunctionCallNode::resolveTypes(IdentifierMap& identifier_map) {
    // function name must be declared in symbol table by `FunctionDeclNode`
    if (!identifier_map.contains(this->func_identifier->name)) {
        throw std::runtime_error(std::format("Type Error: Calling undeclared function '{}'",
                                             this->func_identifier->name));
    }
    // functions with external linkage will have same name
    // only internal linkage functions will get new unique names
    this->func_identifier->name = identifier_map[this->func_identifier->name].unique_name;
    for (auto& arg : this->arguments) {
        arg->resolveTypes(identifier_map);
    }
}

/*
- Detect errors of type `<unary_op> <exprfactor> = <expr>`
- eg: `!a = 3` ==parsed_as=> `UnaryNode('!', AssignmentNode(VarNode('a'), ConstantNode('3')))`;
*/
void UnaryNode::resolveTypes(IdentifierMap& identifier_map) {
    assert(this->operand && "Operand is null in `UnaryNode::resolveTypes`");
    if (isa<AssignmentNode>(this->operand.get())) {
        throw std::runtime_error("Type Error: Cannot assign to the result of a unary operation");
    }
    this->operand->resolveTypes(identifier_map);
}
// identifier resolution -- end

// check types -- start
void ProgramNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    for (auto& func : this->functions) {
        func->checkTypes(type_checker_map);
    }
}

void DeclarationNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    if (this->func) {
        this->func->checkTypes(type_checker_map);
    } else if (this->var) {
        this->var->checkTypes(type_checker_map);
    }
}

void VariableDeclNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    type_checker_map[this->var_identifier->name] = IntType{};
    if (this->init_expr) {
        this->init_expr->checkTypes(type_checker_map);
    }
}

void FunctionDeclNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    bool has_body = (this->body != nullptr);
    bool already_defined = false;

    // has been declared/defined before
    if (type_checker_map.contains(this->func_name->name)) {
        auto& existing_type = type_checker_map[this->func_name->name];
        if (FuncType* func_type = std::get_if<FuncType>(&existing_type)) {
            already_defined = func_type->defined;
            // if already defined and trying to define again, raise error
            if (has_body && already_defined) {
                throw std::runtime_error(std::format("Type Error: Redefinition of function '{}'",
                                                     this->func_name->name));
            }
            if (func_type->param_types.size() != this->parameters.size()) {
                throw std::runtime_error(std::format("Type Error: Conflicting number of parameters "
                                                     "in declarations for function '{}'",
                                                     this->func_name->name));
            }
        } else { // existing_type is not FuncType
            throw std::runtime_error(std::format("Type Error: Conflicting types for function '{}'",
                                                 this->func_name->name));
        }
    }
    auto param_types = std::vector<std::unique_ptr<Type>>{};
    for (auto& param : this->parameters) {
        // for now, only IntType parameters are supported
        param_types.push_back(std::make_unique<Type>(IntType{}));
    }
    // add/update FuncType in type checker symbol table
    type_checker_map[this->func_name->name] = FuncType{
        .param_types = std::move(param_types),
        .return_type = std::make_unique<Type>(IntType{}),
        .defined = has_body || already_defined,
    };

    if (has_body) {
        for (const auto& param : this->parameters) {
            type_checker_map[param->name] = IntType{};
        }
        this->body->checkTypes(type_checker_map);
    }
}

void BlockNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    for (const auto& block_item : this->block_items) {
        block_item->checkTypes(type_checker_map);
    }
}

void BlockItemNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    if (this->declaration) {
        this->declaration->checkTypes(type_checker_map);
    } else if (this->statement) {
        this->statement->checkTypes(type_checker_map);
    }
}

void StatementNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    if (this->return_stmt) {
        this->return_stmt->checkTypes(type_checker_map);
    } else if (this->expression_stmt) {
        this->expression_stmt->checkTypes(type_checker_map);
    } else if (this->ifelse_stmt) {
        this->ifelse_stmt->checkTypes(type_checker_map);
    } else if (this->compound_stmt) {
        this->compound_stmt->checkTypes(type_checker_map);
    } else if (this->break_stmt) {
        this->break_stmt->checkTypes(type_checker_map); // no-op
    } else if (this->continue_stmt) {
        this->continue_stmt->checkTypes(type_checker_map); // no-op
    } else if (this->while_stmt) {
        this->while_stmt->checkTypes(type_checker_map);
    } else if (this->dowhile_stmt) {
        this->dowhile_stmt->checkTypes(type_checker_map);
    } else if (this->for_stmt) {
        this->for_stmt->checkTypes(type_checker_map);
    } else if (this->null_stmt) {
        this->null_stmt->checkTypes(type_checker_map); // no-op
    } else {
        throw std::runtime_error("Type Error: Malformed StatementNode");
    }
}

void ReturnNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->ret_expr->checkTypes(type_checker_map);
}

void ExpressionNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->expr->checkTypes(type_checker_map);
}

void IfElseNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->condition->checkTypes(type_checker_map);
    this->if_block->checkTypes(type_checker_map);
    if (this->else_block) {
        this->else_block->checkTypes(type_checker_map);
    }
}

void CompoundNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->block->checkTypes(type_checker_map);
}

void BreakNode::checkTypes(TypeCheckerSymbolTable& type_checker_map){};    // no-op
void ContinueNode::checkTypes(TypeCheckerSymbolTable& type_checker_map){}; // no-op

void WhileNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->condition->checkTypes(type_checker_map);
    this->body->checkTypes(type_checker_map);
}

void DoWhileNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->body->checkTypes(type_checker_map);
    this->condition->checkTypes(type_checker_map);
}

void ForNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->init->checkTypes(type_checker_map);
    if (this->condition) {
        this->condition->checkTypes(type_checker_map);
    }
    if (this->post) {
        this->post->checkTypes(type_checker_map);
    }
    this->body->checkTypes(type_checker_map);
}

void ForInitNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    if (this->declaration) {
        this->declaration->checkTypes(type_checker_map);
    } else if (this->init_expr) {
        this->init_expr->checkTypes(type_checker_map);
    }
}

void NullNode::checkTypes(TypeCheckerSymbolTable& type_checker_map){}; // no-op

void ExprNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->left_exprf->checkTypes(type_checker_map);
}

void ExprFactorNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    if (this->var_identifier) {
        this->var_identifier->checkTypes(type_checker_map);
    } else if (this->unary) {
        this->unary->checkTypes(type_checker_map);
    } else if (this->expr) {
        this->expr->checkTypes(type_checker_map);
    } else if (this->constant) {
        this->constant->checkTypes(type_checker_map); // no-op ig??
    } else if (this->func_call) {
        this->func_call->checkTypes(type_checker_map);
    } else {
        throw std::runtime_error("Type Error: Malformed ExprFactorNode");
    }
}

void VarNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    if (!std::holds_alternative<IntType>(type_checker_map[this->var_name->name])) {
        throw std::runtime_error(
            std::format("Type Error: Variable '{}' is not of type 'int'", this->var_name->name));
    }
}

void ConstantNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    // constants are always of type int for now
}

void UnaryNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->operand->checkTypes(type_checker_map);
}

void BinaryNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->left_expr->checkTypes(type_checker_map);
    this->right_expr->checkTypes(type_checker_map);
}

void AssignmentNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->left_expr->checkTypes(type_checker_map);
    this->right_expr->checkTypes(type_checker_map);
}

void FunctionCallNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    Type& caller_type = type_checker_map[this->func_identifier->name];
    if (std::holds_alternative<IntType>(caller_type)) {
        throw std::runtime_error(
            std::format("Type Error: Attempting to call non-function of type 'int' '{}'",
                        this->func_identifier->name));
    } else if (FuncType* func_type = std::get_if<FuncType>(&caller_type)) {
        if (func_type->param_types.size() != this->arguments.size()) {
            throw std::runtime_error(
                std::format("Type Error: Function '{}' expects {} arguments but {} were provided",
                            this->func_identifier->name, func_type->param_types.size(),
                            this->arguments.size()));
        }
        // for now, only IntType parameters are supported
        for (const auto& arg : this->arguments) {
            arg->checkTypes(type_checker_map);
        }
    } else {
        throw std::runtime_error(
            std::format("Type Error: Unknown type for function '{}'", this->func_identifier->name));
    }
}

void ConditionalNode::checkTypes(TypeCheckerSymbolTable& type_checker_map) {
    this->condition->checkTypes(type_checker_map);
    this->true_expr->checkTypes(type_checker_map);
    this->false_expr->checkTypes(type_checker_map);
}
// check types -- end

// loop labelling -- start
void ProgramNode::loopLabelling(std::string& loop_label) {
    for (auto& functions : this->functions) {
        functions->loopLabelling(loop_label);
    }
}

void DeclarationNode::loopLabelling(std::string& loop_label) {
    if (func) {
        func->loopLabelling(loop_label);
    } else if (var) {
        var->loopLabelling(loop_label);
    }
}

void VariableDeclNode::loopLabelling(std::string& loop_label){}; // no-op

void FunctionDeclNode::loopLabelling(std::string& loop_label) {
    if (body) {
        body->loopLabelling(loop_label);
    }
}

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

    IdentifierMap global_sym_table;
    ast->resolveTypes(global_sym_table);
    if (true) {
        std::println("----- Identifier Resolution -----");
        ast->dump();
        std::println("---------------------------------");
    }
    ast->checkTypes(global_type_checker_map);
    if (true) {
        std::println("----- Type Checking -----");
        ast->dump();
        std::println("-------------------------");
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