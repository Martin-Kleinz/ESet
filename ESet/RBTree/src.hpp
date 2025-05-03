#include <iostream>
#include <utility>
#include <algorithm>
#include <functional>

template <class T, class Compare = std::less<T>>
class RBTree
{
private:
    enum class Color{RED, BLACK};
    struct Node
    {
        Key key;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
        Node(const key& k, const Color c, Node* p = nullptr, Node* l = nullptr, Node* r = nullptr) : key(k), color(c), parent(p), left(l), right(r) {}
    };
    Node* root;
    size_t size;
    Compare comp;
    void Right_Rotate(Node* x)
    {
        Node *xl = x->left;
        x->left = xl->right;
        if(xl->right) xl->right->parent = x;
        xl->parent = x->parent;
        if(!xl->parent) root = xl;
        else if(xl == xl->parent->left) x->parent->left = xl;
        else x->parent->right = xl;
        xl->right = x;
        x = xl;
    }
    void Left_Rotate(Node* x)
    {
        Node* xr = x->right;
        x->right = xr->left;
        if(xr->left) xr->left->parent = x;
        xr->parent = x->parent;
        if(!x->parent) root = xr;
        else if(x == x->parent->left) x->parent->left = xr;
        else x->parent->right = xr;
        xr->left = x;
        x->parent = xr;
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
    void Insert(const Key& key)
    {
        Node *& c = Binary_Insert(key, root);
        Adjust(c);
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