#ifndef TEXTTRIE_H_
#define TEXTTRIE_H_

#include <memory>
#include <sstream>
#include <node.hpp>


class TextTrie
{
    friend TrieBuilder;

    public:
        typedef std::pair<long,std::string> Match;
        typedef std::list<Match> MatchList;

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

#endif
