#include <string>
#include <map>
#include <list>
#include <stdlib.h>
#include <texttrie.hpp>
#include <sstream>
#include <memory>
#include <iostream>
#include <algorithm>

//
//  Each node in the Trie is a Node object. Each Node has a map of characters 
//  to next states, and a boolean indicating if that node marks the end of 
//  a word.
//


Node::Node(Node* pParent, char name) : pParent(pParent), name(name)
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

Node* Node::add_child(char c)
{
    children[c] = new Node(this, c);
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

const char Node::get_name()
{
    return this->name;
}

Node* Node::get_or_create_child(char c)
{
    if (this->has_child(c))
        return this->get_child(c);
    else
        return this->add_child(c);
}

void Node::print_tree(int depth)
{
    for (ChildMap::iterator it = children.begin(); it != children.end(); ++it)
    {
        std::cout << std::string(depth*2,' ') << (*it).first;
        if ((*it).second->is_word())
            std::cout << '*';
        std::cout << std::endl;
        (*it).second->print_tree( depth + 1);
    }
}

//
//  Return the word or partial word represented by the path of nodes down to
//  the current one.   If our path is root -> P -> A -> T -> H (where H is the
//  current node), then the returned value will be 'PATH'
//

const std::string Node::get_path()
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

TextTrie::TextTrie() : numTerms(0), numNodes(0), pos(0), isNewWord(true)
{
    pRoot = new Node(NULL, ' ');
    candidates = CandidateList();
}

TextTrie::~TextTrie()
{
    delete pRoot;
}

const long TextTrie::count_terms()
{
    return numTerms;
}

const long TextTrie::count_nodes()
{
    return numNodes;
}


void TextTrie::print_tree()
{
    pRoot->print_tree(0);
}

std::string TextTrie::print_nodelist(Node::NodeList& l) 
{
    std::stringstream ss = std::stringstream();

    Node::NodeList::iterator lit = l.begin();
    ss << "[";
    while (lit != l.end())
    {
        ss << (*lit)->get_path();
        lit++;
        if (lit != l.end())
            ss << ", ";
    }

    ss << "]";

    return ss.str();
}


std::list<char> TextTrie::process_group(std::istringstream& term)
{
    // To get in here we had to peek at a [, so consume it now
    char c;
    term.get(c);
    std::list<char> groupcontents = std::list<char>();

    // until we see the group terminator, grab and store each char
    while (c != ']')
    {
        term.get(c);
        if (c != ']')
        {
            groupcontents.push_back(c);
        }
    }

    return groupcontents;
}


void TextTrie::process_term(std::istringstream& term, Node::NodeList& currentNodes)
{
    while (1) 
    {
        int nextToken = term.peek();
        if (nextToken == EOF)
        {
            // end of stream = send of term, so mark it all EOW
            for (Node* n : currentNodes)
                n->set_eow();
            break;
        }

        if (nextToken == '[')
        {
            std::list<char> groupContents = this->process_group(term);
            Node::NodeList newList = Node::NodeList();
            for (Node* n : currentNodes)
                for (char c : groupContents)
                {
                    Node* newNode =  n->get_or_create_child(c);
                    newList.push_back(newNode);
                }
           
            currentNodes = Node::NodeList(newList);
            continue;
        }
        else
        {
            char c;
            term.get(c);

            auto newList = Node::NodeList();
            for (Node* n : currentNodes)
            {
                Node* newNode = n->get_or_create_child(c);
                newList.push_back( newNode );
    //            std::cout << "Transforming " << n->get_path() << " -> " << newNode->get_path() << std::endl;
            }
            currentNodes = Node::NodeList(newList);
        }
    }
}

void TextTrie::add_word(std::string word)
{
    auto inputString = std::istringstream(word);
    Node::NodeList currentNodes( { this->pRoot } ) ;
    this->process_term(inputString, currentNodes);
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


