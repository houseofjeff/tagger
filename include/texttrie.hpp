#ifndef TEXTTRIE_H_
#define TEXTTRIE_H_
class TextTrie;

#include <memory>
#include <sstream>

class Node
{
    friend TextTrie;
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
    public:
        typedef std::pair<long,std::string> Match;
        typedef std::list<Match> MatchList;
        typedef Node::NodeList&(*TransformFn)(Node::NodeList&);


        TextTrie();
        ~TextTrie();

        Node::NodeList& transform(Node::NodeList&, TransformFn);
        void add_word(std::string);
        const long count_terms();
        const long count_nodes();
        std::unique_ptr<MatchList> next(char);
        std::string print_nodelist(Node::NodeList&);
        void print_tree();

    private:
        
        typedef std::list<Node*> CandidateList;

        std::list<char> process_group(std::istringstream& term);
        void process_term(std::istringstream& term, Node::NodeList& currentNodes);
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

#endif
