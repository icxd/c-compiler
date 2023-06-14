#include <stdlib.h>
#include <stdio.h>
#include "include/sv.h"
#include "include/types.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/tokenizer.h"


struct parser_t* parser_new(struct tokenizer_t* t) {
    struct parser_t* p = malloc(sizeof(struct parser_t));
    p->tokenizer = t;
    p->ast = NULL;
    p->token = tokenizer_next(t);
    return p;
}

void parser_free(struct parser_t* p) {
    free(p);
}

struct ast_t* parser_parse(struct parser_t* p) {
    struct ast_t* ast = malloc(sizeof(struct ast_t));
    ast->type = AST_PROGRAM;
    ast->data.program.statements = NULL;
    ast->next = NULL;

    struct ast_t* current = ast;

    while (p->token.type != TK_EOF) {
        if (p->token.type == TK_UNKNOWN) {
            fprintf(stderr, "Unknown token: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(p->token.value), p->token.line, p->token.column);
            break;
        } else if (p->token.type == TK_ERROR) {
            fprintf(stderr, "Error: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(p->token.value), p->token.line, p->token.column);
            break;
        }

        if (p->token.type == TK_COMMENT) {
            p->token = tokenizer_next(p->tokenizer);
            continue;
        }

        struct ast_t* statement = parser_parse_statement(p);
        if (statement == NULL) {
            fprintf(stderr, "Error: Unexpected token: \""SV_ARG"\" (%d) at line %d, column %d\n", SV_FMT(p->token.value), p->token.type, p->token.line, p->token.column);
            break;
        }

        current->next = statement;
        current = current->next;
    }

    return ast;
}

struct ast_t* parser_parse_statement(struct parser_t* p) {
    struct ast_t* statement = malloc(sizeof(struct ast_t));
    statement->next = NULL;

    if (p->token.type == TK_HASH) {
        statement->type = AST_TOP_LEVEL_ATTRIBUTE;
        p->token = tokenizer_next(p->tokenizer);
        string identifier = p->token.value;
        if (sv_compare(identifier, SV("foreign"))) {
            statement->data.top_level_attribute.name = identifier;
            statement->data.top_level_attribute.value = NULL;
            p->token = tokenizer_next(p->tokenizer);

            if (p->token.type == TK_STRING) {
                statement->data.top_level_attribute.value = malloc(sizeof(struct ast_t));
                statement->data.top_level_attribute.value->type = AST_STRING;
                statement->data.top_level_attribute.value->data.string_.value = p->token.value;
                statement->data.top_level_attribute.value->next = NULL;
                p->token = tokenizer_next(p->tokenizer);
            } else {
                fprintf(stderr, "Error: Expected string literal at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
        } else {
            fprintf(stderr, "Error: Unknown top-level attribute: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(identifier), p->token.line, p->token.column);
            return NULL;
        }
    } else if (p->token.type == TK_IDENTIFIER) {
        string identifier = p->token.value;
        p->token = tokenizer_next(p->tokenizer);

        if (p->token.type == TK_COLON) {
            p->token = tokenizer_next(p->tokenizer);

            struct type_t* type = NULL;
            if (p->token.type != TK_COLON) {
                type = parser_parse_type(p);                
            }
            
            statement->type = AST_CONSTANT_DECLARATION;

            if (p->token.type == TK_SEMICOLON) {
                p->token = tokenizer_next(p->tokenizer);
                statement->data.const_decl.name = identifier;
                statement->data.const_decl.type = type;
                statement->data.const_decl.value = NULL;
                return statement;    
            }

            p->token = tokenizer_next(p->tokenizer);

            statement->data.const_decl.name = identifier;
            statement->data.const_decl.type = type;
            statement->data.const_decl.value = parser_parse_expression(p);

            if (p->token.type != TK_SEMICOLON) {
                fprintf(stderr, "Error: Expected semicolon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            p->token = tokenizer_next(p->tokenizer);
        }
    } else if (p->token.type == TK_USING) {
        statement->type = AST_USING;
        p->token = tokenizer_next(p->tokenizer);

        if (p->token.type == TK_IDENTIFIER) {
            struct ast_t* node = parser_parse_statement(p);
            if (node->type != AST_CONSTANT_DECLARATION) {
                fprintf(stderr, "Error: Expected constant declaration at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            statement->data.using_.node = node;
        } else if (p->token.type == TK_IMPORT) {
            struct ast_t* node = parser_parse_expression(p);
            if (node->type != AST_IMPORT) {
                fprintf(stderr, "Error: Expected import at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            statement->data.using_.node = node;

            if (p->token.type != TK_SEMICOLON) {
                fprintf(stderr, "Error: Expected semicolon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
            p->token = tokenizer_next(p->tokenizer);
        } else {
            fprintf(stderr, "Error: Expected identifier or import at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
    } else {
        statement->type = AST_EXPRESSION_STATEMENT;
        statement->data.expression_statement.expression = parser_parse_expression(p);

        if (p->token.type != TK_SEMICOLON) {
            fprintf(stderr, "Error: Expected semicolon at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }

        p->token = tokenizer_next(p->tokenizer);
    }
    
    return statement;
}

struct ast_t* parser_parse_expression(struct parser_t* p) {
    return parser_parse_binary(p);
}

struct ast_t* parser_parse_binary(struct parser_t* p) {
    return parser_parse_unary(p);
}

struct ast_t* parser_parse_unary(struct parser_t* p) {
    return parser_parse_primary(p);
}

struct ast_t* parser_parse_primary(struct parser_t* p) {
    struct ast_t* expression = malloc(sizeof(struct ast_t));
    expression->next = NULL;

    struct token_t token = p->token;
    p->token = tokenizer_next(p->tokenizer);

    if (token.type == TK_STRING) {
        expression->type = AST_STRING;
        expression->data.string_.value = token.value;
    } else if (token.type == TK_INTEGER) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = atoi(token.value.chars);
        expression->data.integer.base = BASE_DECIMAL;
    } else if (token.type == TK_FLOAT) {
        expression->type = AST_FLOAT;
        expression->data.float_.value = atof(token.value.chars);
    } else if (token.type == TK_IDENTIFIER) {
        expression->type = AST_VARIABLE;
        expression->data.variable.identifier = token.value;
    } else if (token.type == TK_HEXADECIMAL) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol(token.value.chars, NULL, 16);
        expression->data.integer.base = BASE_HEXADECIMAL;
    } else if (token.type == TK_BINARY) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol(token.value.chars, NULL, 2);
        expression->data.integer.base = BASE_BINARY;
    } else if (token.type == TK_TRUE) {
        expression->type = AST_BOOLEAN;
        expression->data.boolean.value = true;
    } else if (token.type == TK_FALSE) {
        expression->type = AST_BOOLEAN;
        expression->data.boolean.value = false;
    } else if (token.type == TK_NULL) {
        expression->type = AST_NULL;
    } else if (token.type == TK_FN) {
        expression->type = AST_FUNCTION;
        p->token = tokenizer_next(p->tokenizer);
        struct parameter_t* parameters = NULL;
        while (p->token.type != TK_CLOSE_PAREN) {
            struct parameter_t* parameter = malloc(sizeof(struct parameter_t));
            parameter->type = NULL;
            string identifier = p->token.value;
            p->token = tokenizer_next(p->tokenizer);
            if (p->token.type != TK_COLON) {
                fprintf(stderr, "Error: Expected colon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            p->token = tokenizer_next(p->tokenizer);

            if (p->token.type == TK_PERIOD) {
                p->token = tokenizer_next(p->tokenizer);
                p->token = tokenizer_next(p->tokenizer);
                p->token = tokenizer_next(p->tokenizer);

                expression->data.function.variadic_parameter_name = identifier;
                expression->data.function.is_variadic = true;

                break;
            }

            if (p->token.type == TK_ASSIGN) {
                p->token = tokenizer_next(p->tokenizer);
                parameter->default_value = parser_parse_expression(p);
            } else {
                struct type_t* type = parser_parse_type(p);
                if (type == NULL) {
                    fprintf(stderr, "Error: Expected type at line %d, column %d\n", p->token.line, p->token.column);
                    return NULL;
                }

                if (p->token.type == TK_ASSIGN) {
                    p->token = tokenizer_next(p->tokenizer);
                    parameter->default_value = parser_parse_expression(p);
                } else {
                    parameter->default_value = NULL;
                }
                parameter->type = type;
            }

            parameter->name = identifier;
            parameter->next = parameters;
            parameters = parameter;

            if (p->token.type == TK_COMMA) {
                p->token = tokenizer_next(p->tokenizer);
            }
        }
        expression->data.function.parameters = parameters;
        p->token = tokenizer_next(p->tokenizer);

        if (p->token.type == TK_ARROW) {
            p->token = tokenizer_next(p->tokenizer);
            expression->data.function.return_type = parser_parse_type(p);
        } else {
            expression->data.function.return_type = (struct type_t*) malloc(sizeof(struct type_t));
            expression->data.function.return_type->type = TYPE_VOID;
        }

        expression->data.function.foreign = false;
        expression->data.function.entry_point = false;
        expression->data.function.inline_ = false;

        while (p->token.type == TK_HASH) {
            p->token = tokenizer_next(p->tokenizer);
            
            if (p->token.type != TK_IDENTIFIER) {
                fprintf(stderr, "Error: Expected identifier at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            string identifier = p->token.value;
            p->token = tokenizer_next(p->tokenizer);

            if (sv_compare(identifier, SV("foreign"))) {
                if (p->token.type != TK_STRING) {
                    fprintf(stderr, "Error: Expected string at line %d, column %d\n", p->token.line, p->token.column);
                    return NULL;
                }

                expression->data.function.foreign = true;
                expression->data.function.foreign_name = p->token.value;
                p->token = tokenizer_next(p->tokenizer);
            } else if (sv_compare(identifier, SV("entry_point"))) {
                expression->data.function.entry_point = true;
            } else if (sv_compare(identifier, SV("inline"))) {
                expression->data.function.inline_ = true;
            } else {
                fprintf(stderr, "Error: Unknown attribute \""SV_ARG"\" at line %d, column %d\n", SV_FMT(identifier), p->token.line, p->token.column);
                return NULL;
            }
        }

        if (p->token.type != TK_OPEN_BRACE) {
            expression->data.function.body = NULL;
            return expression;
        }

        p->token = tokenizer_next(p->tokenizer);
        
        struct ast_t* statements = NULL;
        while (p->token.type != TK_CLOSE_BRACE) {
            struct ast_t* statement = parser_parse_statement(p);
            if (statement == NULL) {
                fprintf(stderr, "Error: Expected statement at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            statement->next = statements;
            statements = statement;
        }

        expression->data.function.body = statements;
        p->token = tokenizer_next(p->tokenizer);
    } else if (token.type == TK_STRUCT) {
        expression->type = AST_STRUCT;

        if (p->token.type != TK_OPEN_BRACE) {
            fprintf(stderr, "Error: Expected open brace at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
        p->token = tokenizer_next(p->tokenizer);

        struct parameter_t* fields = NULL;
        while (p->token.type != TK_CLOSE_BRACE) {
            struct parameter_t* field = malloc(sizeof(struct parameter_t));
            field->type = NULL;
            string identifier = p->token.value;
            p->token = tokenizer_next(p->tokenizer);
            if (p->token.type != TK_COLON) {
                fprintf(stderr, "Error: Expected colon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            p->token = tokenizer_next(p->tokenizer);

            if (p->token.type == TK_ASSIGN) {
                p->token = tokenizer_next(p->tokenizer);
                field->default_value = parser_parse_expression(p);
            } else {
                struct type_t* type = parser_parse_type(p);
                if (type == NULL) {
                    fprintf(stderr, "Error: Expected type at line %d, column %d\n", p->token.line, p->token.column);
                    return NULL;
                }

                if (p->token.type == TK_ASSIGN) {
                    p->token = tokenizer_next(p->tokenizer);
                    field->default_value = parser_parse_expression(p);
                } else {
                    field->default_value = NULL;
                }
                field->type = type;
            }

            field->name = identifier;
            field->next = fields;
            fields = field;

            if (p->token.type != TK_SEMICOLON) {
                fprintf(stderr, "Error: Expected semicolon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
            p->token = tokenizer_next(p->tokenizer);
        }
        expression->data.struct_.fields = fields;

        if (p->token.type != TK_CLOSE_BRACE) {
            fprintf(stderr, "Error: Expected close brace at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
        p->token = tokenizer_next(p->tokenizer);
    } else if (token.type == TK_ENUM) {
        expression->type = AST_ENUM;
        expression->data.enum_.base_type = NULL;

        if (p->token.type == TK_OPEN_PAREN) {
            p->token = tokenizer_next(p->tokenizer);
            expression->data.enum_.base_type = parser_parse_type(p);
            if (expression->data.enum_.base_type == NULL) {
                fprintf(stderr, "Error: Expected type at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            if (p->token.type != TK_CLOSE_PAREN) {
                fprintf(stderr, "Error: Expected close paren at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
            p->token = tokenizer_next(p->tokenizer);
        } else {
            expression->data.enum_.base_type = NULL;
        }

        if (p->token.type != TK_OPEN_BRACE) {
            fprintf(stderr, "Error: Expected open brace at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
        p->token = tokenizer_next(p->tokenizer);

        struct enum_variant_t* values = NULL;
        while (p->token.type != TK_CLOSE_BRACE) {
            struct enum_variant_t* value = malloc(sizeof(struct enum_variant_t));
            value->name = p->token.value;
            value->value = NULL;
            p->token = tokenizer_next(p->tokenizer);

            if (p->token.type == TK_ASSIGN) {
                p->token = tokenizer_next(p->tokenizer);
                value->value = parser_parse_expression(p);
            }

            value->next = values;
            values = value;

            if (p->token.type == TK_COMMA) {
                p->token = tokenizer_next(p->tokenizer);
            }
        }
        expression->data.enum_.variants = values;

        if (p->token.type != TK_CLOSE_BRACE) {
            fprintf(stderr, "Error: Expected close brace at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
        p->token = tokenizer_next(p->tokenizer);
    } else if (token.type == TK_UNION) {
        expression->type = AST_UNION;

        if (p->token.type != TK_OPEN_BRACE) {
            fprintf(stderr, "Error: Expected open brace at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
        p->token = tokenizer_next(p->tokenizer);

        struct union_field_t* fields = NULL;
        while (p->token.type != TK_CLOSE_BRACE) {
            struct union_field_t* field = malloc(sizeof(struct union_field_t));
            // name : type,
            field->name = p->token.value;
            p->token = tokenizer_next(p->tokenizer);

            if (p->token.type != TK_COLON) {
                fprintf(stderr, "Error: Expected colon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
            p->token = tokenizer_next(p->tokenizer);

            field->type = parser_parse_type(p);
            field->next = fields;
            fields = field;

            if (p->token.type != TK_SEMICOLON) {
                fprintf(stderr, "Error: Expected semicolon at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
            p->token = tokenizer_next(p->tokenizer);
        }
        expression->data.union_.fields = fields;

        if (p->token.type != TK_CLOSE_BRACE) {
            fprintf(stderr, "Error: Expected close brace at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }
        p->token = tokenizer_next(p->tokenizer);
    } else if (token.type == TK_IMPORT) {
        expression->type = AST_IMPORT;
        expression->data.import.path = p->token.value;
        p->token = tokenizer_next(p->tokenizer);
    } else {
        fprintf(stderr, "Unexpected token: \""SV_ARG"\" (%d) at line %d, column %d\n", SV_FMT(token.value), token.type, token.line, token.column);
        return NULL;
    }
    
    return expression;
}

struct type_t* parser_parse_type(struct parser_t* p) {
    struct type_t* type = malloc(sizeof(struct type_t));

    if (p->token.type == TK_IDENTIFIER) {
        string identifier = p->token.value;
        p->token = tokenizer_next(p->tokenizer);

        // if (p->token.type == TK_LESS_THAN) {
        //     p->token = tokenizer_next(p->tokenizer);

        //     type->type = TYPE_GENERIC;
        //     type->data.generic.identifier = identifier;
        //     type->data.generic.arguments = parser_parse_type_arguments(p);

        //     if (p->token.type != TK_GREATER_THAN) {
        //         fprintf(stderr, "Error: Expected \">\" at line %d, column %d\n", p->token.line, p->token.column);
        //         return NULL;
        //     }

        //     p->token = tokenizer_next(p->tokenizer);
        // } else {
        type->type = TYPE_UNKNOWN;
        type->data.unknown.identifier = identifier;
        // }
    } else if (p->token.type == TK_I8) {
        type->type = TYPE_I8;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_I16) {
        type->type = TYPE_I16;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_I32) {
        type->type = TYPE_I32;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_I64) {
        type->type = TYPE_I64;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_U8) {
        type->type = TYPE_U8;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_U16) {
        type->type = TYPE_U16;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_U32) {
        type->type = TYPE_U32;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_U64) {
        type->type = TYPE_U64;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_F32) {
        type->type = TYPE_F32;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_F64) {
        type->type = TYPE_F64;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_CHAR) {
        type->type = TYPE_CHAR;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_BOOL) {
        type->type = TYPE_BOOLEAN;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_VOID) {
        type->type = TYPE_VOID;
        p->token = tokenizer_next(p->tokenizer);
    } else if (p->token.type == TK_ASTERISK) {
        p->token = tokenizer_next(p->tokenizer);
        type->type = TYPE_POINTER;
        type->data.pointer.inner = parser_parse_type(p);
    } else if (p->token.type == TK_OPEN_BRACKET) {
        p->token = tokenizer_next(p->tokenizer);
        type->type = TYPE_ARRAY;
        if (p->token.type != TK_CLOSE_BRACKET) {
            type->data.array.is_fixed = true;
            u64 size;
            if (p->token.type == TK_INTEGER) {
                size = atoi(p->token.value.chars);
                p->token = tokenizer_next(p->tokenizer);
            } else {
                fprintf(stderr, "Error: Expected integer at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }

            type->data.array.size = size;

            if (p->token.type != TK_CLOSE_BRACKET) {
                fprintf(stderr, "Error: Expected \"]\" at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
        } else {
            type->data.array.is_fixed = false;
        }

        p->token = tokenizer_next(p->tokenizer);
        type->data.array.inner = parser_parse_type(p);
    } else {
        fprintf(stderr, "Error: Expected type at line %d, column %d\n", p->token.line, p->token.column);
        return NULL;
    }

    return type;
}
