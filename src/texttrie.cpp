#include <map>
#include <list>
#include <texttrie.hpp>
#include <node.hpp>

//
//
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


//
//  Outputs a formatted list of nodes suitable for debugging

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


