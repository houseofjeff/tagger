#include <string>
#include <map>
#include <list>
#include <stdlib.h>
#include <texttrie.hpp>
#include <sstream>
#include <memory>

//
//  Each node in the Trie is a Node object. Each Node has a map of characters 
//  to next states, and a boolean indicating if that node marks the end of 
//  a word.
//


Node::Node(Node* pParent, char name, bool eow) : pParent(pParent), name(name), eow(eow)
{
    children = ChildMap();
}

Node::~Node()
{
    // on destruction, go through and destroy all the children
    for (ChildMap::iterator it = children.begin(); it != children.end(); ++it)
    {
        delete it->second;
    }
}

bool Node::has_child(char c)
{
    return children.find(c) != children.end();
}

Node* Node::get_child(char c)
{
    return children[c];
}

Node* Node::add_child(char c, bool eow)
{
    children[c] = new Node(this, c, eow);
    return children[c];
}

bool Node::is_word()
{
    return this->eow;
}

void Node::set_eow()
{
    this->eow = true;
}

char Node::get_name()
{
    return this->name;
}

//
//  Return the word or partial word represented by the path of nodes down to
//  the current one.   If our path is root -> P -> A -> T -> H (where H is the
//  current node), then the returned value will be 'PATH'
//

std::string Node::get_path()
{
    // Work backward up the tree, collecting the letters for each level...
    std::list<char> pathback = std::list<char>();
    Node* current = this;

    while (current->pParent != NULL)
    {
        pathback.push_front( current->get_name() );
        current = current->pParent;
    }

    // ...then use those letters to construct a string
    std::string pathstr = std::string();
    std::list<char>::iterator it = pathback.begin();
    while (it != pathback.end())
    {
        pathstr.push_back(*it);
        it++;
    }

    return pathstr;
}


//
//  This is the primary object, it respresents the full Trie structure and the
//  functionality to run text through it, returning matched terms as it goes.
//

TextTrie::TextTrie() : isNewWord(true), numTerms(0), numNodes(0), pos(0)
{
    pRoot = new Node(NULL, ' ', false);
    candidates = CandidateList();
}

TextTrie::~TextTrie()
{
    delete pRoot;
}

long TextTrie::count_terms()
{
    return numTerms;
}

long TextTrie::count_nodes()
{
    return numNodes;
}

void TextTrie::add_word(std::string word)
{
    Node* current = pRoot;

    // iterate through the chars in the word, stepping down a level in the
    // trie each time.
    for (int i = 0; i < word.length(); i++)
    {
        char c = tolower(word[i]);
        bool eow = (i >= word.length()-1);

        // does the next level under this letter already exist?
        if (current->has_child(c))
        {
            // it does, so get the child for that letter
            current = current->get_child(c);

            // it's possible that the node already exists (if you add 'men',
            // but the word 'menu' was already added) so make this eow
            if (eow) 
            {
                current->set_eow();
            }
        }
        else
        {
            // it doesn't, so add a new child to this node
            current = current->add_child(c, eow);
            this->numNodes++;
        }
    }
    this->numTerms++;
}


//
//  This is the public function, called once per character in the corpus file
//  If the character is a letter, then we evolve each of the candidate matches
//  by calling advance(char).   If it's whitespace or punctuation, we treat it
//  as an end-of-word and we return still-valid candidates by calling end().
//

std::unique_ptr<TextTrie::MatchList> TextTrie::next(char c)
{
    c = tolower(c);
    if ((c == '\'') or ((c >= 'a') and (c <= 'z')))
    {
        this->advance(c);
        this->isNewWord = false;
        return NULL;
    }
    else
    {
        this->isNewWord = true;
        std::unique_ptr<MatchList> pResults = this->end();
        this->advance(c);
        return pResults;
    }
}

std::unique_ptr<TextTrie::MatchList> TextTrie::advance(char c) 
{
    int startlen = candidates.size();

    CandidateList::iterator it = candidates.begin();
    while (it != candidates.end())
    {
        if ((*it)->has_child(c))
        {
            Node* next = (*it)->get_child(c);
            candidates.push_front(next);
        }
        else
        {
            // this term isn't in the list
        }
        
        candidates.erase(it++);
    }

    if (this->isNewWord && (pRoot->has_child(c)))
    {
        candidates.push_front( pRoot->get_child(c) );
    }
   
    this->pos++;

    return std::unique_ptr<MatchList>{};
}

std::unique_ptr<TextTrie::MatchList> TextTrie::end()
{
    CandidateList::iterator it = candidates.begin();
    std::unique_ptr<MatchList> pResults = std::make_unique<MatchList>();;

    while (it != candidates.end())
    {
        if ((*it)->is_word())
        {
            pResults->push_back( Match(pos, (*it)->get_path()) );
        }
        it++;
    }

    return pResults;
}


