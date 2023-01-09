
typedef struct Node{
    char name[32];
    struct{
        int int_val;
        float float_val;
        char char_val[32];
    }val;

    struct Node *first_child;
    struct Node *next_brother;

    int column;
    int leaf;
}Node;

struct Node* generate_node(char *name, int col, int int_val, float float_val, char *char_val);
Node* get_n_child(Node* root, int n);