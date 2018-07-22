#ifndef TEXTTRIE_H_
#define TEXTTRIE_H_
class TextTrie;
class TrieBuilder;

#include <memory>
#include <sstream>

class Node
{
    friend TextTrie;
    friend TrieBuilder;
    typedef std::map<char, Node*> ChildMap;

    private:
        Node* pParent;
        bool eow;
        char name;
        ChildMap children;

        bool has_child(char);
        Node* get_child(char);
        Node* add_child(char);
        bool is_word();
        Node* get_or_create_child(char);
 
    public:
        typedef std::list<Node*> NodeList;

        Node(Node*, char);
        ~Node();
        void set_eow();
        const char get_name();
        const std::string get_path();
        void print_tree(int);
};



class TextTrie
{
    friend TrieBuilder;

    public:
        typedef std::pair<long,std::string> Match;
        typedef std::list<Match> MatchList;
        typedef Node::NodeList&(*TransformFn)(Node::NodeList&);


        TextTrie();
        ~TextTrie();

        const long count_terms();
        const long count_nodes();
        std::unique_ptr<MatchList> next(char);
        std::string print_nodelist(Node::NodeList&);
        void print_tree();

    private:
        
        typedef std::list<Node*> CandidateList;
        void seteow(Node*);

        CandidateList candidates;
        Node* pRoot;
        long numTerms;
        long numNodes;
        long pos;
        bool isNewWord;

        std::unique_ptr<MatchList> advance(char);
        std::unique_ptr<MatchList> end();
};


class TrieBuilder
{
    public:
        TrieBuilder();
        void add_word(std::string*);
        std::unique_ptr<TextTrie> get_trie();

    private:
        char get_next();
        char peek_next();
        void process_term();
        std::list<char> process_group();

        long numTerms;
        long numNodes;

        std::unique_ptr<TextTrie> pTrie;
        Node::NodeList currentNodes;
        std::string* currentWord;
        u_int currentPos;
};

#endif
