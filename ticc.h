
enum {              // Token type
    TK_NUM = 256,
    TK_IDENT,
    TK_RETURN,
    TK_EOF,
};

enum {              // Node type
    ND_NUM = 256,
    ND_IDENT,
    ND_RETURN,
    ND_EOF,
};

typedef struct Token {
    int ty;             // type
    int val;            // TK_NUMの時のみ
    char *name;         // TK_IDENTの時(変数名),
    char *input;        // 入力の位置(デバッグ用)
} Token;

typedef struct Node {
    int ty;             // type
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // ND_NUMの時のみ
    char *name;          // ND_IDENTの時のみ
} Node;

typedef struct Vector {
    void **data;
    int len;                // 要素の数
    int capacity;           // dataの長さ(容量)
} Vector;

typedef struct Map {
    Vector *keys;
    Vector *vals;
} Map;

void error(char *fmt, ...);
Node* new_node(int ty, Node *lhs, Node *rhs);
Node* new_node_num(int val);
Node* new_node_ident(char *name);
Vector* new_vector();
void push_vector(Vector *vec, void *elm);
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
Map* new_map();
void map_set(Map *map, char *keys, void *val);
void* map_get(Map *map, char *key);
int ident_len(char *p);
char* substr(char *p, int start, int len);