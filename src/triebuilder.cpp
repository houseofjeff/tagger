#include <map>
#include <list>
#include <iostream>
#include <triebuilder.hpp>
#include <texttrie.hpp>

// ===========================================================================
//

TrieBuilder::TrieBuilder() 
{
    // Pointer to the trie, will be sent back eventually
    this->pTrie = std::make_unique<TextTrie>();

    // these will be set for each pass through add_word()
    this->currentNodes = Node::NodeList();
    this->currentWord = NULL;
}



//
//  Adds a word to the trie by traversing it and adding nodes that
//  don't exist yet

void TrieBuilder::add_word(std::string* pWord)
{
    //std::cout << *pWord << std::endl;

    // set up the variables that will be used while adding this word
    this->currentNodes.clear();
    this->currentWord = pWord;
    this->currentPos = 0;

    // currentNodes is the list of nodes presently being added to, this
    // can be more than one in the case of a group
    currentNodes.push_back( this->pTrie->pRoot );

    // start the processing to add the word
    this->process_term();
    this->numTerms++;
}


// 
//  Look at the current position in the input, but don't consume it

char TrieBuilder::peek_next()
{
    if (currentPos < currentWord->length())
        return (*currentWord)[currentPos];
    else
        return EOF;
}


//
//  Take the next token from the word string

char TrieBuilder::get_next()
{
    char next = peek_next();
    if (next != 0)
        currentPos++;
    return next;
}


//
//  Process the word a character at a time.  

void TrieBuilder::process_term()
{
    while (1) 
    {
        if (peek_next() == EOF)
        {
            // end of stream = send of term, so mark it all EOW and exit
            for (Node* n : currentNodes)
                n->set_eow();
            break;
        }

        char nextToken = get_next();
        Node::NodeList newList = Node::NodeList();

        // if the parser gets to a [, that's the start of a group
        if (nextToken == '[')
        {
            // read that group and return the contents, a list of chars 
            // in the group
            std::list<char> groupContents = this->process_group();

            // for each of the currentNodes, advance down the tree
            // from their present position
            for (Node* n : currentNodes)
                for (char c : groupContents)
                {
                    Node* newNode = n->get_or_create_child(c);
                    newList.push_back(newNode);
                }
        }
        else if (nextToken == ']')
        {
            // encountered a end-of-group bracket without an open group
            throw syntax_error("Found end-of-group bracket outside of a group");
        }
        else
        {
            // Just a regular character, advance all the currentNodes
            // down into the trie from their present positions.
            for (Node* n : currentNodes)
            {
                Node* newNode = n->get_or_create_child(nextToken);
                newList.push_back( newNode );
            }
        }
        currentNodes.swap(newList);
    }
}

//
//

std::list<char> TrieBuilder::process_group()
{
    // To get in here we had to peek at a [, so consume it now
    std::list<char> groupcontents = std::list<char>();

    // until we see the group terminator, grab and store each char
    while (1)
    {
        char c = get_next();

        if (c == EOF)
            throw syntax_error("Term ended with open group");
        if (c == '[')
            throw syntax_error("Can't have a group-within-a-group");

        if (c != ']')
            groupcontents.push_back(c);
        else
            break;
    }

    return groupcontents;
}


//
//

std::unique_ptr<TextTrie> TrieBuilder::get_trie()
{
    return std::move(pTrie);
}
