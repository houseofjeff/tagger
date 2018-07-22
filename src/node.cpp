#include <map>
#include <list>
#include <iostream>
#include <node.hpp>

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


