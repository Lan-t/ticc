
enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF,
};

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_EOF,
};

typedef struct Token{
    int ty;
    int val;
    char *input;
} Token;

typedef struct Node{
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

typedef struct {
    Token **data;
    int len;
    int capacity;
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
