#ifndef NODE_H_
#define NODE_H_

class TextTrie;
class TrieBuilder;

class Node
{
    friend TextTrie;
    friend TrieBuilder;

    private:
        typedef std::map<char, Node*> ChildMap;
        typedef std::list<Node*> NodeList;

        Node* pParent;
        bool eow;
        char name;
        ChildMap children;

        Node(Node*, char);
        ~Node();

        bool has_child(char);
        Node* get_child(char);
        Node* add_child(char);
        Node* get_or_create_child(char);

        void set_eow();
        bool is_word();

        const char get_name();
        const std::string get_path();

        void print_tree(int);
 };


#endif
