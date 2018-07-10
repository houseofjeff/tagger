class TextTrie;

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
        typedef std::list<std::string> MatchList;

        TextTrie();
        ~TextTrie();

        void add_word(std::string);
        long count_terms();
        long count_nodes();
        MatchList* next(char);

    private:
        typedef std::list<Node*> CandidateList;

        Node* pRoot;
        long numTerms;
        long numNodes;
        CandidateList candidates;
        bool isNewWord;

        MatchList* advance(char);
        MatchList* end();

};


