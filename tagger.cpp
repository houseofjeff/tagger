#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <stdlib.h>
#include <memory>

int numNodes = 0;
class TextTrie;

//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------

class TextTrie
{
    public:
        typedef std::list<std::string> MatchList;

        TextTrie();
        ~TextTrie();

        void add_word(std::string);
        long count();
        MatchList* next(char);

    private:
        typedef std::list<Node*> CandidateList;

        Node* pRoot;
        long numWords;
        CandidateList candidates;

        MatchList* advance(char);
        MatchList* end();

};

TextTrie::TextTrie()
{
    pRoot = new Node(NULL, ' ', false);
    candidates = CandidateList();
}

TextTrie::~TextTrie()
{
    delete pRoot;
}

long TextTrie::count()
{
    return numWords;
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
        }
    }
    this->numWords++;
}


TextTrie::MatchList* TextTrie::next(char c)
{
    c = tolower(c);
    if ((c == '\'') or ((c >= 'a') and (c <= 'z')))
    {
        this->advance(c);
        return NULL;
    }
    else
    {
        return this->end();
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
    if (pRoot->has_child(c)) 
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

        candidates.erase(it++);
    }

    return pResults;
}


//----------------------------------------------------------------------------

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
