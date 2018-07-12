#ifndef TEXTTRIE_H_
#define TEXTTRIE_H_
class TextTrie;

#include <memory>

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
        Node* add_child(char, bool);
        bool is_word();
 
    public:
        Node(Node*, char, bool);
        ~Node();
        void set_eow();
        char get_name();
        std::string get_path();
};



class TextTrie
{
    public:
        typedef std::pair<long,std::string> Match;
        typedef std::list<Match> MatchList;


        TextTrie();
        ~TextTrie();

        void add_word(std::string);
        long count_terms();
        long count_nodes();
        std::unique_ptr<MatchList> next(char);
        //MatchList* next(char);

    private:
        typedef std::list<Node*> CandidateList;

        Node* pRoot;
        long numTerms;
        long numNodes;
        long pos;
        CandidateList candidates;
        bool isNewWord;

        std::unique_ptr<MatchList> advance(char);
        std::unique_ptr<MatchList> end();
        //MatchList* advance(char);
        //MatchList* end();

};

#endif
