#include "nanocc/AST/AST.hpp"
#include "nanocc/Utils/Utils.hpp"

#include "SemaHelper.hpp"

namespace Sema {
// loop labelling -- start
void programNodeLoopLabelling(ProgramNode& program_node,
                              std::string& loop_label) {
  for (auto& decl : program_node.declarations) {
    declarationNodeLoopLabelling(*decl, loop_label);
  }
}

void declarationNodeLoopLabelling(DeclarationNode& declaration_node,
                                  std::string& loop_label) {
  if (declaration_node.func) {
    functionDeclNodeLoopLabelling(*declaration_node.func, loop_label);
  } else if (declaration_node.var) {
    variableDeclNodeLoopLabelling(*declaration_node.var, loop_label);
  }
}

void variableDeclNodeLoopLabelling(VariableDeclNode& variable_decl_node,
                                   std::string& loop_label){}; // no-op

void functionDeclNodeLoopLabelling(FunctionDeclNode& function_decl_node,
                                   std::string& loop_label) {
  if (function_decl_node.body) {
    blockNodeLoopLabelling(*function_decl_node.body, loop_label);
  }
}

void blockNodeLoopLabelling(BlockNode& block_node, std::string& loop_label) {
  for (const auto& block_item : block_node.block_items) {
    blockItemNodeLoopLabelling(*block_item, loop_label);
  }
}

void blockItemNodeLoopLabelling(const BlockItemNode& block_item_node,
                                std::string& loop_label) {
  if (block_item_node.declaration) {
    declarationNodeLoopLabelling(*block_item_node.declaration,
                                 loop_label); // no-op
  } else if (block_item_node.statement) {
    statementNodeLoopLabelling(*block_item_node.statement, loop_label);
  }
}

void statementNodeLoopLabelling(StatementNode& statement_node,
                                std::string& loop_label) {
  if (statement_node.return_stmt) {
    returnNodeLoopLabelling(*statement_node.return_stmt, loop_label); // no-op
  } else if (statement_node.expression_stmt) {
    expressionNodeLoopLabelling(*statement_node.expression_stmt,
                                loop_label); // no-op
  } else if (statement_node.ifelse_stmt) {
    ifElseNodeLoopLabelling(*statement_node.ifelse_stmt, loop_label); // no-op
  } else if (statement_node.compound_stmt) {
    compoundNodeLoopLabelling(*statement_node.compound_stmt, loop_label);
  } else if (statement_node.break_stmt) {
    breakNodeLoopLabelling(*statement_node.break_stmt, loop_label);
  } else if (statement_node.continue_stmt) {
    continueNodeLoopLabelling(*statement_node.continue_stmt, loop_label);
  } else if (statement_node.while_stmt) {
    whileNodeLoopLabelling(*statement_node.while_stmt, loop_label);
  } else if (statement_node.dowhile_stmt) {
    doWhileNodeLoopLabelling(*statement_node.dowhile_stmt, loop_label);
  } else if (statement_node.for_stmt) {
    forNodeLoopLabelling(*statement_node.for_stmt, loop_label);
  } else if (statement_node.null_stmt) {
    nullNodeLoopLabelling(*statement_node.null_stmt, loop_label); // no-op
  } else {
    throw std::runtime_error("Loop Labeling Error: Malformed StatementNode");
  }
}

void returnNodeLoopLabelling(ReturnNode& return_node,
                             std::string& loop_label){}; // no-op
void expressionNodeLoopLabelling(ExpressionNode& expression_node,
                                 std::string& loop_label){}; // no-op

void ifElseNodeLoopLabelling(IfElseNode& ifelse_node, std::string& loop_label) {
  statementNodeLoopLabelling(*ifelse_node.if_block, loop_label);
  if (ifelse_node.else_block) {
    statementNodeLoopLabelling(*ifelse_node.else_block, loop_label);
  }
};

void compoundNodeLoopLabelling(CompoundNode& compound_node,
                               std::string& loop_label) {
  blockNodeLoopLabelling(*compound_node.block, loop_label);
};

void breakNodeLoopLabelling(BreakNode& break_node, std::string& loop_label) {
  if (loop_label.empty()) {
    nanocc::raiseError(break_node.location.filename, break_node.location.line,
                       break_node.location.column, STAGE,
                       "'break' used outside of a loop");
  }
  break_node.label = std::make_unique<IdentifierNode>();
  break_node.label->name = loop_label;
}

void continueNodeLoopLabelling(ContinueNode& continue_node,
                               std::string& loop_label) {
  if (loop_label.empty()) {
    nanocc::raiseError(continue_node.location.filename,
                       continue_node.location.line,
                       continue_node.location.column, STAGE,
                       "'continue' used outside of a loop");
  }
  continue_node.label = std::make_unique<IdentifierNode>();
  continue_node.label->name = loop_label;
}

void whileNodeLoopLabelling(WhileNode& while_node, std::string& loop_label) {
  std::string new_label = getLabelName("while");
  while_node.label = std::make_unique<IdentifierNode>();
  while_node.label->name = new_label;
  statementNodeLoopLabelling(*while_node.body, new_label);
}

void doWhileNodeLoopLabelling(DoWhileNode& dowhile_node,
                              std::string& loop_label) {
  std::string new_label = getLabelName("do_while");
  dowhile_node.label = std::make_unique<IdentifierNode>();
  dowhile_node.label->name = new_label;
  statementNodeLoopLabelling(*dowhile_node.body, new_label);
}

void forNodeLoopLabelling(ForNode& for_node, std::string& loop_label) {
  std::string new_label = getLabelName("for");
  for_node.label = std::make_unique<IdentifierNode>();
  for_node.label->name = new_label;
  statementNodeLoopLabelling(*for_node.body, new_label);
}

void nullNodeLoopLabelling(NullNode& null_node,
                           std::string& loop_label){}; // no-op
// loop labelling -- end
} // namespace Sema

namespace nanocc {
void semaLoopLabelling(ProgramNode& ast) {
  std::string loop_label = "";
  Sema::programNodeLoopLabelling(ast, loop_label);
}
} // namespace nanocc
