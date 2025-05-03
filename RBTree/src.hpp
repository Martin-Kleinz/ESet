#include <iostream>
#include <utility>

template <class Key, class Compare = std::less<Key>>
class RBTree
{
public:
    enum class Color
    {
        RED,
        BLACK
    };
    struct Node
    {
        Key *key;
        Color color;
        Node *left;
        Node *right;
        Node *parent;
        Node(Key *k, const Color c, Node *p = nullptr, Node *l = nullptr, Node *r = nullptr) : key(k), color(c), parent(p), left(l), right(r) {}
        ~Node() {}
    };
    Node *root, *nil;
    size_t size_;
    Compare comp;
    void init_nil()
    {
        nil = new Node(nullptr, Color::RED);
        nil->left = nil->right = nil->parent = nil;
    }
    void Right_Rotate(Node *x)
    {
        Node *xl = x->left;
        x->left = xl->right;
        if (xl->right != nil)
            xl->right->parent = x;
        xl->parent = x->parent;
        if (root == x)
            root = xl;
        else if (x == x->parent->left)
            x->parent->left = xl;
        else
            x->parent->right = xl;
        xl->right = x;
        x->parent = xl;
    }
    void Left_Rotate(Node *x)
    {
        Node *xr = x->right;
        x->right = xr->left;
        if (xr->left != nil)
            xr->left->parent = x;
        xr->parent = x->parent;
        if (root == x)
            root = xr;
        else if (x == x->parent->left)
            x->parent->left = xr;
        else
            x->parent->right = xr;
        xr->left = x;
        x->parent = xr;
    }
    Node *Minimum(Node *x) const
    {
        while (x->left != nil)
            x = x->left;
        return x;
    }
    Node *Maximum(Node *x) const
    {
        while (x->right != nil)
            x = x->right;
        return x;
    }
    Node *Prev(Node *x) const
    {
        if(x == nil) return Maximum(root);
        if (x->left != nil)
            return Maximum(x->left);
        Node *y = x->parent;
        while (y != nil && x == y->left)
        {
            x = y;
            y = y->parent;
        }
        return y;
    }
    Node* Next(Node *x) const
    {
        if (x->right != nil)
            return Minimum(x->right);
        Node *y = x->parent;
        while (y != nil && x == y->right)
        {
            x = y;
            y = x->parent;
        }
        return y;
    }
    void Transplant(Node *u, Node *v)
    {
        if (u->parent == nil)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        v->parent = u->parent;
    }
    void Insert_Fixup(Node *z)
    {
        while (z->parent->color == Color::RED)
        {
            Node *p = z->parent;
            Node *pp = p->parent;
            if (p == pp->left)
            {
                if (pp->right->color == Color::RED) // case 1
                {
                    pp->color = Color::RED;
                    p->color = Color::BLACK;
                    pp->right->color = Color::BLACK;
                    z = pp;
                }
                else
                {
                    if (z == p->right) // case 2
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
                if (pp->left->color == Color::RED)
                {
                    pp->color = Color::RED;
                    p->color = Color::BLACK;
                    pp->left->color = Color::BLACK;
                    z = pp;
                }
                else
                {
                    if (z == p->left)
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
    void Remove_Fixup(Node *x)
    {
        while (x != root && x->color == Color::BLACK)
        {
            if (x == x->parent->left)
            {
                Node *bro = x->parent->right;
                if (bro->color == Color::RED) // case 1
                {
                    bro->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    Left_Rotate(x->parent);
                    bro = x->parent->right;
                }
                if (bro->left->color == Color::BLACK && bro->right->color == Color::BLACK) // case 2
                {
                    bro->color = Color::RED;
                    x = x->parent;
                }
                else
                {
                    if (bro->left->color == Color::RED) // case 3
                    {
                        bro->color = Color::RED;
                        bro->left->color = Color::BLACK;
                        Right_Rotate(bro);
                        bro = x->parent->right;
                    }
                    // case 4
                    bro->color = bro->parent->color;
                    bro->parent->color = Color::BLACK;
                    bro->right->color = Color::BLACK;
                    Left_Rotate(x->parent);
                    x = root;
                }
            }
            else
            {
                Node *bro = x->parent->left;
                if (bro->color == Color::RED)
                {
                    bro->parent->color = Color::RED;
                    bro->color = Color::BLACK;
                    Right_Rotate(x->parent);
                    bro = x->parent->left;
                }
                if (bro->left->color == Color::BLACK && bro->right->color == Color::BLACK)
                {
                    bro->color = Color::RED;
                    x = x->parent;
                }
                else
                {
                    if (bro->right->color == Color::RED)
                    {
                        bro->color = Color::RED;
                        bro->right->color = Color::BLACK;
                        Left_Rotate(bro);
                        bro = x->parent->left;
                    }
                    bro->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    bro->left->color = Color::BLACK;
                    Right_Rotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = Color::BLACK;
    }
    void Clear(Node *x)
    {
        if(x == nil) return;
        if(x->left != nil) Clear(x->left);
        if(x->right != nil) Clear(x->right);
        delete x->key;
        delete x;
        x = nullptr;
    }
    Node* Copy(Node *x, Node *parent, Node *nl)
    {
        if (x == nl)
            return nil;
        Node *newnode = new Node(new Key(*x->key), x->color, parent, nil, nil);
        newnode->left = Copy(x->left, newnode, nl);
        newnode->right = Copy(x->right, newnode, nl);
        return newnode;
    }

    RBTree() : size_(0), comp(Compare())
    {
        init_nil();
        root = nil;
    }

    RBTree(const RBTree &other) : size_(other.size_), comp(other.comp)
    {
        init_nil();
        root = Copy(other.root, nil, other.nil);
    } 

    RBTree(RBTree &&other) : root(other.root), nil(other.nil), size_(other.size_), comp(std::move(other.comp))
    {
        other.root = other.nil = nullptr;
        other.size_ = 0;
    }

    RBTree& operator=(const RBTree& other)
    {
        if(this != &other){
            Clear(root);
            delete nil;
            size_ = other.size_;
            comp = other.comp;
            init_nil();
            root = Copy(other.root, nil, other.nil);
        }
        return *this;
    }

    RBTree& operator=(RBTree&& other)
    {
        if(this != &other){
            Clear(root);
            delete nil;
            root = other.root;
            nil = other.nil;
            size_ = other.size_;
            comp = std::move(other.comp);
            other.root = other.nil = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~RBTree()
    {
        Clear(root);
        if(nil != nullptr) delete nil, nil = nullptr;
    }
    std::pair<Node *, bool> Insert(const Key &key)
    {
        Node *y = nil, *x = root;
        while (x != nil)
        {
            y = x;
            if (comp(key, *(x->key)))
                x = x->left;
            else if (comp(*(x->key), key))
                x = x->right;
            else
                return {x, false};
        }
        x = new Node(new Key(key), Color::RED, y, nil, nil);
        if (y == nil)
        {
            root = x;
            root->parent = y;
        }
        else 
        {
            if (comp(key, *(y->key)))
                y->left = x;
            else
                y->right = x;
        }
        size_++;
        Insert_Fixup(x);
        return {x, true};
    }
    bool Remove(const Key &key)
    {
        Node *z = root;
        while (z != nil)
        {
            if (comp(key, *(z->key)))
                z = z->left;
            else if (comp(*(z->key), key))
                z = z->right;
            else
                break;
        }
        if (z == nil)
            return false;
        Node *y = z;
        Color ycolor = y->color;
        Node *x = nil;
        if (z->left == nil)
        {
            x = z->right;
            Transplant(z, z->right);
        }
        else if (z->right == nil)
        {
            x = z->left;
            Transplant(z, z->left);
        }
        else
        {
            y = Minimum(z->right);
            ycolor = y->color;
            x = y->right;
            if (y->parent == z)
                x->parent = y;
            else
            {
                Transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            Transplant(z, y);
            y->left = z->left;
            z->left->parent = y;
            y->color = z->color;
        }
        delete z->key;
        delete z;
        size_--;
        if (ycolor == Color::BLACK)
        {
            Remove_Fixup(x);
        }
        return true;
    }
    size_t Size() const
    {
        return size_;
    }
    Node *Find(const Key &key) const
    {
        Node *x = root;
        while (x != nil)
        {
            if (comp(key, *(x->key)))
                x = x->left;
            else if (comp(*(x->key), key))
                x = x->right;
            else
                return x;
        }
        return nil;
    }
};

template <class Key, class Compare = std::less<Key>>
class ESet
{
private:
    RBTree<Key, Compare> tree;
public:
    class iterator
    {
    private:
        typename RBTree<Key, Compare>::Node *current;
        const RBTree<Key, Compare> *tree;

    public:
        iterator(typename RBTree<Key, Compare>::Node *node = nullptr, const RBTree<Key, Compare> *t = nullptr) : current(node), tree(t) {}
        ~iterator() {}
        
        const Key &operator*() const
        {
            if(current == tree->nil) throw "invalid iterator";
            return *(current->key);
        }
        iterator &operator++()
        {
            if(current != tree->nil)
                current = tree->Next(current);
            return *this;
        }
        iterator operator++(int)
        {
            iterator temp = *this;
            ++(*this);
            return temp;
        }
        iterator &operator--()
        {
            if(current == tree->Minimum(tree->root)) return *this;
            current = tree->Prev(current);
            return *this;
        }
        iterator operator--(int)
        {
            if(current == tree->Minimum(tree->root)) return *this;
            iterator temp = *this;
            --(*this);
            return temp;
        }
        bool operator==(const iterator &other) const
        {
            return current == other.current;
        }
        bool operator!=(const iterator &other) const
        {
            return current != other.current;
        }
    };

    ESet() = default;

    ~ESet() = default;

    std::pair<iterator, bool> insert(const Key &key)
    {
        auto result = tree.Insert(key);
        return {iterator(result.first, &tree), result.second};
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args)
    {
        return insert(Key(std::forward<Args>(args)...));
    }

    size_t erase(const Key &key)
    {
        return tree.Remove(key) ? 1 : 0;
    }

    iterator find(const Key &key) const
    {
        return iterator(tree.Find(key), &tree);
    }

    ESet(const ESet &other) : tree(other.tree) {}

    ESet &operator=(const ESet &other){
        if(this != &other)
            tree = other.tree;
        return *this;
    }

    ESet(ESet &&other) noexcept : tree(std::move(other.tree)) {}

    ESet &operator=(ESet &&other) noexcept{
        if(this != &other) tree = std::move(other.tree);
        return *this;
    }

    iterator lower_bound(const Key &key) const{
        typename RBTree<Key, Compare>::Node* x = tree.root;
        typename RBTree<Key, Compare>::Node* result = tree.nil;
        while(x != tree.nil)
        {
            if(!tree.comp(*(x->key), key)){
                result = x;
                x = x->left;
            }
            else x = x->right;
        }
        return iterator(result, &tree);
    }

    iterator upper_bound(const Key &key) const{
        typename RBTree<Key, Compare>::Node* x = tree.root;
        typename RBTree<Key, Compare>::Node* result = tree.nil;
        while(x != tree.nil)
        {
            if(tree.comp(key, *(x->key))){
                result = x;
                x = x->left;
            }
            else x = x->right;
        }
        return iterator(result, &tree);
    }


    size_t range(const Key &l, const Key &r) const{
        if(tree.comp(r, l)) return 0;
        iterator lower = lower_bound(l), upper = upper_bound(r);
        size_t cnt = 0;
        while(lower != upper)
        {
            ++cnt;
            ++lower;
        }
        return cnt;
    }

    size_t size() const noexcept
    {
        return tree.size_;
    }
    
    iterator begin() const noexcept
    {
        return iterator(tree.Minimum(tree.root), &tree);
    }

    iterator end() const noexcept
    {
        return iterator(tree.nil, &tree);
    }
    
};