/*
 * Visitor Pattern Proof of Concept Demo
 * 
 * This file demonstrates the visitor pattern working with AST nodes.
 * It shows how visitors can traverse and process AST nodes without
 * tightly coupling the node classes to specific operations.
 * 
 * Usage: Build and run ./visitor_demo <file.c>
 */

#include <print>
#include "helper.hpp"
#include "example_visitors.hpp"

void demonstrateVisitorPattern(const std::string& filename) {
    std::println("\n========================================");
    std::println("Visitor Pattern Proof of Concept Demo");
    std::println("========================================\n");
    
    // Parse the file
    auto contents = getFileContents(filename);
    auto tokens = lexer(contents, false);
    auto ast = parse(tokens, false);
    
    std::println("1. Demonstrating ExamplePrintVisitor:");
    std::println("--------------------------------------");
    
    // Create a visitor that prints node information
    ExamplePrintVisitor printVisitor;
    
    // For demo purposes, manually visit some expression nodes if they exist
    // In a full implementation, you would traverse the entire AST
    for (auto& function : ast->functions) {
        if (!function->body) continue;
        
        for (auto& block_item : function->body->block_items) {
            if (block_item->statement && block_item->statement->return_stmt) {
                auto& ret_node = block_item->statement->return_stmt;
                std::println("\nFound a return statement, visiting it:");
                ret_node->accept(printVisitor);
                
                // If the return has an expression, visit its components
                if (ret_node->ret_expr && ret_node->ret_expr->left_exprf) {
                    auto& expr_factor = ret_node->ret_expr->left_exprf;
                    
                    if (expr_factor->constant) {
                        expr_factor->constant->accept(printVisitor);
                    }
                    if (expr_factor->var_identifier) {
                        expr_factor->var_identifier->accept(printVisitor);
                    }
                    if (expr_factor->unary) {
                        expr_factor->unary->accept(printVisitor);
                    }
                    // Note: BinaryNode, AssignmentNode, and ConditionalNode are ExprFactorNode subclasses
                    // They can be accessed if expr_factor points to one of those derived types
                    auto* binary = dynamic_cast<BinaryNode*>(expr_factor.get());
                    if (binary) {
                        binary->accept(printVisitor);
                    }
                }
            }
            
            if (block_item->statement && block_item->statement->expression_stmt) {
                std::println("\nFound an expression statement, visiting it:");
                block_item->statement->expression_stmt->accept(printVisitor);
            }
        }
    }
    
    std::println("\n2. Demonstrating SemanticAnalysisVisitor:");
    std::println("------------------------------------------");
    std::println("This visitor delegates to existing semantic analysis methods.");
    std::println("It shows how the visitor pattern can wrap existing functionality.\n");
    
    // Re-parse for a clean slate
    ast = parse(tokens, false);
    
    IdentifierMap sym_table;
    TypeCheckerSymbolTable type_map;
    SemanticAnalysisVisitor semanticVisitor(sym_table, type_map);
    
    // Visit nodes using the semantic analysis visitor
    for (auto& function : ast->functions) {
        if (!function->body) continue;
        
        for (auto& block_item : function->body->block_items) {
            if (block_item->statement && block_item->statement->return_stmt) {
                auto& ret_node = block_item->statement->return_stmt;
                
                if (ret_node->ret_expr && ret_node->ret_expr->left_exprf) {
                    auto& expr_factor = ret_node->ret_expr->left_exprf;
                    
                    if (expr_factor->constant) {
                        expr_factor->constant->accept(semanticVisitor);
                        std::println("Visited constant node with semantic analysis");
                    }
                }
            }
        }
    }
    
    std::println("\n========================================");
    std::println("Benefits of Visitor Pattern:");
    std::println("========================================");
    std::println("✓ Separation of Concerns: Node classes focus on structure");
    std::println("✓ Extensibility: Easy to add new operations without modifying nodes");
    std::println("✓ Testability: Each visitor can be tested independently");
    std::println("✓ Flexibility: Multiple visitors can implement different traversals");
    std::println("");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr, "Usage: {} <source_file.c>", argv[0]);
        std::println(stderr, "");
        std::println(stderr, "This demo shows the visitor pattern proof of concept.");
        std::println(stderr, "It demonstrates how visitors can process AST nodes.");
        return 1;
    }
    
    std::string filename = argv[1];
    
    try {
        // Run the visitor pattern demonstration
        demonstrateVisitorPattern(filename);
        
        // Also run the normal compilation to show everything still works
        std::println("\n========================================");
        std::println("Normal Compilation (existing functionality):");
        std::println("========================================\n");
        std::string asm_output = getAsmOutputFromCFile(filename, true);
        std::println("Compilation successful! Assembly generated.");
        
    } catch (const std::exception& e) {
        std::println(stderr, "Error: {}", e.what());
        return 1;
    }
    
    return 0;
}
