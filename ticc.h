
enum {              // Token type
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF,
};

enum {              // Node type
    ND_NUM = 256,
    ND_IDENT,
    ND_EOF,
};

typedef struct Token{
    int ty;             // type
    int val;            // TK_NUMの時のみ
    char *input;        // TK_IDENTの時(変数名), 入力の位置(デバッグ用)
} Token;

typedef struct Node{
    int ty;             // type
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // ND_NUMの時のみ
    char name;          // ND_IDENTの時のみ
} Node;

typedef struct TokenVector{ // token可変配列
    Token **data;
    int len;                // 要素の数
    int capacity;           // dataの長さ(容量)
} TokenVector;

void error(char *fmt, ...);
Node* new_node(int ty, Node *lhs, Node *rhs);
Node* new_node_num(int val);
Node* new_node_ident(char name);
TokenVector* new_tvector();
void push_tvector(TokenVector *vec, Token *token);
int consume(int ty);
void tokenize(char *p);
Node* add();
Node* mul();
Node* term();
Node* assign();
Node* stmt();
void program();
void gen_lval(Node* node);
void gen();
