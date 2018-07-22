#ifndef TRIEBUILDER_H_
#define TRIEBUILDER_H_

#include <memory>
#include <node.hpp>

class TrieBuilder
{
    public:
        TrieBuilder();
        void add_word(std::string*);
        std::unique_ptr<TextTrie> get_trie();

    private:
        std::unique_ptr<TextTrie> pTrie;
        Node::NodeList currentNodes;
        std::string* currentWord;
        u_int currentPos;
        long numTerms;
        long numNodes;

        char get_next();
        char peek_next();
        void process_term();
        std::list<char> process_group();
};

#endif
