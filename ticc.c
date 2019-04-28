#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "ticc.h"

// globals

Vector *tokens;
Node *code[100];
int pos = 0;


// utils

// // general utils

void error(char *fmt, ...) {
    /* エラー出力 (printf format, auto newline) */
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Vector* new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->capacity = 16;
    vec->len = 0;
    vec->data = malloc(sizeof(void*) * vec->capacity);
}

void push_vector(Vector *vec, void *elm) {
    if(vec->len == vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void*) * vec->capacity);
    }
    vec->data[vec->len++] = elm;
}


// // tokenize utils

int is_alnum(char c) {
    return  ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && '9' <= c) ||
            (c == '_');
}


// // AST genelator utils

Node* new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node* new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

Node* new_node_ident(char name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

int consume(int ty) {
    if(((Token*)(tokens->data[pos]))->ty != ty) {
        return 0;
    }
    pos++;
    return 1;
}


// tokenize

void tokenize(char *p) {
    tokens = new_vector();
    Token *token;
    while(*p) {
        if(isspace(*p)) {
            p++;
            continue;
        }

        token = malloc(sizeof(Token));

        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';') {
            token->ty = *p;
            token->input = p;
            push_vector(tokens, token);
            p++;
            continue;
        }

        if(strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            token->ty = TK_RETURN;
            token->input =p;
            push_vector(tokens, token);
            p += 6;
            continue;
        }

        if('a' <= *p && *p <= 'z') {
            token->ty = TK_IDENT;
            token->input = p;
            push_vector(tokens, token);
            p++;
            continue;
        }

        if(isdigit(*p)) {
            token->ty = TK_NUM;
            token->input = p;
            token->val = strtol(p, &p, 10);
            push_vector(tokens, token);
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }

    token = malloc(sizeof(Token));
    token->ty = TK_EOF;
    token->input = p;
    push_vector(tokens, token);
}


// AST nodes genelate

void program() {
    int i = 0;
    while(((Token*)(tokens->data[pos]))->ty != TK_EOF) {
        code[i++] = stmt();
    }
    Node *eof = malloc(sizeof(Node));
    eof->ty = ND_EOF;
    code[i] = eof;
}

Node* stmt() {
    Node *node;

    if(consume(TK_RETURN)) {
     node = malloc(sizeof(Node));
     node->ty = ND_RETURN;
     node->lhs = assign();
    } else {
        node = assign();
    }
    
    if(!consume(';')) {
        error("';'ではないトークンです: %s", ((Token*)(tokens->data[pos]))->input);
    }
    return node;
}

Node* assign() {
    Node *node = add();
    while(1) {
        if(consume('=')) {
            node = new_node('=', node, assign());
        } else {
            return node;
        }
    }
}

Node* add() {
    Node *node = mul();

    while(1) {
        if(consume('+')) {
            node = new_node('+', node, mul());
        } else if(consume('-')) {
            node = new_node('-', node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node *node = term();

    while(1) {
        if(consume('*')) {
            node = new_node('*', node, term());
        } else if(consume('/')) {
            node = new_node('/', node, term());
        } else {
            return node;
        }
    }
}

Node* term() {
    if(consume('(')) {
        Node *node = add();
        if(!consume(')')) {
            error("(に対応する)がありません: %s", ((Token*)(tokens->data[pos]))->input);
        }
        return node;
    }

    if(((Token*)(tokens->data[pos]))->ty == TK_NUM) {
        return new_node_num(((Token*)(tokens->data[pos++]))->val);
    }

    if(((Token*)(tokens->data[pos]))->ty == TK_IDENT) {
        return new_node_ident(*(((Token*)(tokens->data[pos++]))->input));
    }

    error("(でも)でもないトークンです: %s", ((Token*)(tokens->data[pos]))->input);
}


// assembry code generator

void gen_lval(Node *node) {
    if(node->ty != ND_IDENT) {
        error("代入の左辺値が変数ではありません");
    }

    int offset = ('z' - node->name + 1) * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
}

void gen(Node *node) {
    if(node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    if(node->ty == ND_RETURN) {
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    if(node->ty == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

    if(node->ty == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->ty) {
        case '+':
            printf("  add rax, rdi\n");
            break;
        case '-':
            printf("  sub rax, rdi\n");
            break;
        case '*':
            printf("  mul rdi\n");
            break;
        case '/':
            printf("  mov rdx, 0\n");
            printf("  div rdi\n");
            break;
    }

    printf("  push rax\n");
}


// main

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    tokenize(argv[1]);
    program();
    
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    for(int i=0; code[i]->ty != ND_EOF; i++) {
        gen(code[i]);

        printf("  pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}