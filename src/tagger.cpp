#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <stdlib.h>
#include <memory>
#include <texttrie.hpp>

int numNodes;

TextTrie* create_trie( char* fname )
{
    std::ifstream infile(fname);

    TextTrie* pTrie = new TextTrie();

    for (std::string line; getline(infile, line); )
    {
        std::istringstream f(line);
        for (std::string word; getline(f, word, ' ');)
        {
            if (word.length() != 1)
                pTrie->add_word(word);
        }
    }

    std::cout << numNodes << " nodes used for " << pTrie->count() << " words" << std::endl;
    infile.close();

    return pTrie;
}


void printResults( TextTrie::MatchList* pResults )
{
    if (pResults == NULL)
        return;

    TextTrie::MatchList::iterator it = pResults->begin();
    while (it != pResults->end())
    {
        //std::cout << (*it) << std::endl;
        it++;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " [dictionaryfile] [corpusfile]" << std::endl << std::endl;
        exit(-1);
    }
    
    // Build the trie from the dictionary file
    TextTrie* pTrie = create_trie(argv[1]);

    // now process the corpus file a character at a time
    char c;
    std::ifstream corpusfile(argv[2]);

    while (corpusfile.get(c))
    {
        TextTrie::MatchList* pResults = pTrie->next(c);
        printResults(pResults);
        delete pResults;
    }

    TextTrie::MatchList* pResults = pTrie->next(' ');
    printResults(pResults);
    delete pResults;

    delete pTrie;
}
