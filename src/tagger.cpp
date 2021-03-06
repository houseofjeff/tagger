#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <list>
#include <texttrie.hpp>
#include <triebuilder.hpp>


//
//  Creates a TextTrie and populates it with the contents of dictionary file
//  The file should be formatted with one term per line
//

std::unique_ptr<TextTrie> create_trie(std::istream& infile)
{
    TrieBuilder builder = TrieBuilder();
    for (std::string term; getline(infile, term); )
    {
        try
        {
            builder.add_word(&term);
        }
        catch (syntax_error& e)
        {
            std::cout << "Syntax error in '" << term << "', " << e.what() << std::endl;
        }
    }

    auto pTrie = builder.get_trie();
    //pTrie->print_tree();

    return pTrie;
}


//
//  Outputs the results that came back from an iteration
//

void printResults(const std::unique_ptr<TextTrie::MatchList>& pResults )
{
    if (pResults == NULL)
        return;

    TextTrie::MatchList::iterator it = pResults->begin();
    while (it != pResults->end())
    {
        std::cout << (*it).first << "," << (*it).second << std::endl; 
        it++;
    }
}

//

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " [termsfile] [corpusfile]" << std::endl << std::endl;
        exit(-1);
    }
    
    //------------------------------------------------------------------------
    // Build the trie from the dictionary file

    std::ifstream dictfile(argv[1]);
    if (!dictfile)
    {
        std::cout << "Couldn't open file " << argv[1] << ", does it exist?" << std::endl;
        return 0;
    }
    std::unique_ptr<TextTrie> pTrie = create_trie(dictfile);


    //------------------------------------------------------------------------
    // now process the corpus file a character at a time, and pass the
    // characters into TextTrie.next(char) to evolve the internal state

    std::ifstream corpusfile(argv[2]);
    if (!corpusfile)
    {
        std::cout << "Couldn't open file " << argv[2] << ", does it exist?" << std::endl;
        return 0;
    }
    
    char c;
    while (corpusfile.get(c))
    {
        // one character at a time, evolve the list of candidate matches
        std::unique_ptr<TextTrie::MatchList> pResults = pTrie->next(c);
        printResults(pResults);
    }

    // And once more at the end of the file so that we finish things up
    std::unique_ptr<TextTrie::MatchList> pResults = pTrie->next(' ');
    printResults(pResults);
}
