
enum {
    TK_NUM = 256,
    TK_EOF,
};

enum {
    ND_NUM = 256,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

typedef struct {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

void error(char *fmt, ...);
Node* new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
int consume(int ty);
void tokenize(char *p);
Node* add();
Node* mul();
Node* term();
void gen();
