#include <string>
#include <map>
#include <list>
#include <stdlib.h>
#include <texttrie.hpp>


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

std::string Node::get_path()
{
    // Work backward up the tree, collecting the letters for each level...
    std::list<char> pathback = std::list<char>();
    Node* current = this;

    while (current != NULL)
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



TextTrie::TextTrie() : isNewWord(true), numTerms(0), numNodes(0)
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


TextTrie::MatchList* TextTrie::next(char c)
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
        MatchList* pResults = this->end();
        this->advance(c);
        return pResults;
    }
}

TextTrie::MatchList* TextTrie::advance(char c) 
{
    int startlen = candidates.size();
    //if (startlen == 0)
    //    std::cout << "-------" << std::endl;

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
            //std::cout << "  ?: " << (*it)->get_path() << c << std::endl;
        }
        
        candidates.erase(it++);
    }

    if (this->isNewWord and (pRoot->has_child(c)) )
    {
        candidates.push_front( pRoot->get_child(c) );
    }
    
}

TextTrie::MatchList* TextTrie::end()
{
    CandidateList::iterator it = candidates.begin();
    MatchList* pResults = new MatchList();

    while (it != candidates.end())
    {
        if ((*it)->is_word())
            pResults->push_back((*it)->get_path());
        it++;
    }

    return pResults;
}


