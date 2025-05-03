#include <iostream>
#include <utility>
#include <algorithm>
#include <functional>

template <class Key, class Compare = std::less<Key>>
class RBTree
{
private:
    enum class Color{RED, BLACK};
    struct Node
    {
        size_t size;
        Key* key;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
        Node(Key* k, const Color c, Node* p = nullptr, Node* l = nullptr, Node* r = nullptr) : key(k), color(c), parent(p), left(l), right(r), size(1) {}
        ~Node(){
            delete key;
        }
    };
    Node *root, *nil;
    size_t size_;
    Compare comp;
    void init_nil()
    {
        nil = new Node(nullptr, Color::BLACK);
        nil->left = nil->right = nil->parent = nil;
        nil->size = 0;
    }
    void update_size(Node*& x)
    {
        if(x != nil)
            x->size = x->left->size + x->right->size + 1;
    }
    void Right_Rotate(Node*& x)
    {
        Node *xl = x->left;
        x->left = xl->right;
        if(xl->right != nil) xl->right->parent = x;
        xl->parent = x->parent;
        if(xl->parent == nil) root = xl;
        else if(xl == xl->parent->left) x->parent->left = xl;
        else x->parent->right = xl;
        xl->right = x;
        x->parent = xl;
        update_size(x);
        update_size(xl);
        x = xl;
    }
    void Left_Rotate(Node* x)
    {
        Node* xr = x->right;
        x->right = xr->left;
        if(xr->left != nil) xr->left->parent = x;
        xr->parent = x->parent;
        if(x->parent == nil) root = xr;
        else if(x == x->parent->left) x->parent->left = xr;
        else x->parent->right = xr;
        xr->left = x;
        x->parent = xr;
        update_size(x);
        update_size(xr);
        x = xr;
    }
    Node* Minimum(Node* x) const
    {
        while(x->left != nil) x = x->left;
        return x;
    }
    Node* Maximum(Node* x) const 
    {
        while(x->right != nil) x = x->right;
        return x;
    }
    void Transplant(Node* u, Node* v)
    {
        if(u->parent == nil) root = v;
        else if(u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        v->parent = u->parent;
    }
    void Insert_Fixup(Node* z)
    {
        while(z->parent->color == Color::RED)
        {
            Node* p = z->parent;
            Node* pp = p->parent;
            if(p == pp->left)
            {
                if(pp->right->color == Color::RED) // case 1
                {
                    pp->color = Color::RED;
                    p->color = Color::BLACK;
                    pp->right->color = Color::BLACK;
                    z = pp;
                }
                else 
                {
                    if(z == p->right) // case 2
                    {
                        z = z->parent;
                        Left_Rotate(z);
                    }
                    // case 3;
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    Right_Rotate(z->parent->parent);
                }
            }
            else 
            {
                if(pp->left->color == Color::RED)
                {
                    pp->color = Color::RED;
                    p->color = Color::BLACK;
                    pp->left->color = Color::BLACK;
                    z = pp;
                }
                else
                {
                    if(z == p->left)
                    {
                        z = z->parent;
                        Right_Rotate(p);
                    }    
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    Left_Rotate(z->parent->parent);
                }
            }
        }
        root->color = Color::BLACK;
    }
    void Delete_Fixup(Node* z)
    {

    }
    Node* Binary_Insert(const Key& key, Node *& x)
    {
        if(x == nullptr) 
        {
            x = new Node(key, Color::RED, x->parent);
            return x;
        }
        else if(comp(key, x->key)) return Binary_Insert(key, x->left);
        else if(comp(x->key, key)) return Binary_Insert(key, x->right);
    }
    void Adjust(Node* c)
    {
        while(c->parent && c->parent->color == Color::RED)
        {
            Node* p = c->parent;
            Node* pp = p->parent;
            if(p == pp->left)
            {
                if(pp->right->color == Color::RED)
                {
                    //case 1
                    pp->color = Color::RED;
                    p->color = Color::BLACK;
                    pp->right->color = Color::BLACK;
                    c = pp;
                }
                else
                {
                    if(c == p->right)
                    {
                        //case 2
                        Left_Rotate(p);
                    }
                    // case 3
                    c->color = Color::BLACK;
                    pp->color = Color::RED;
                    Right_Rotate(pp);
                }
            }
            else
            {
                if(pp->left->color == Color::RED)
                {
                    pp->color = Color::RED;
                    p->color = Color::BLACK;
                    pp->right->color = Color::BLACK;
                    c = pp;
                }
                else 
                {
                    if(c == p->left)
                    {
                        Right_Rotate(p);
                    }
                    c->color = Color::BLACK;
                    pp->color = Color::RED;
                    Left_Rotate(pp);
                }
            }
        }
        root->color = Color::BLACK;
    }
    void Clear(Node* x)
    {
        if(x != nil){
            Clear(x->left);
            Clear(x->right);
            delete x;
        }
    }
public:
    RBTree() : size_(0), comp(Compare()){
        init_nil();
        root = nil;
    }
    ~RBTree(){
        Clear(root);
        delete nil;
    }
    std::pair<Node*, bool> Insert(const Key& key)
    {
        Node *y = nil, *x = root;
        while(x != nil)
        {
            y = x;
            y->size++;
            if(comp(key, *(x->key))) x = x->left;
            else if(comp(*(x->key), key)) x = x->right;
            else
            {
                while(x != y)
                {
                    x->size--;
                    x= x->parent;
                }
                y->size--;
                return {x, false};
            }
            Node *z = new Node(key, Color::RED, y, nil, nil);
            if(y == nil) root = z;
            else if(comp(key, *(y->key))) y->left = z;
            else y->right = z;
            size_++;
            Insert_Fixup(z);
            return {z, true};
        }
    }
    void Erase(const Key& key)
    {
        Node* c = root;
        while (c)
        {
            if(comp(key, c->key)) c = c->left;
            else if(comp(c->key, key)) c = c->right;
            else break;
        }
        
    }
};


template <class Key, class Compare = std::less<Key>>
class ESet
{
public:
    class iterator
    {
    private:
        Node* current;
        const ESet* set;
    public:
        iterator();
        ~iterator();
    };

    ESet();

    ~ESet();

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args);

    size_t erase(const Key *key);

    iterator find(const Key &key) const;

    ESet(const ESet &other);

    ESet &operator=(const ESet &other);

    ESet(ESet &&other);

    ESet &operator=(ESet &&other) noexcept;

    size_t range(const Key &l, const Key &r) const;

    size_t size() const noexcept;

    iterator lower_bound(const Key &key) const;

    iterator upper_bound(const Key &key) const;

    iterator begin() const noexcept;

    iterator end() const noexcept;

};