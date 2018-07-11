#include <stdlib.h>
#include <string>
#include <istream>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <list>


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " [termsfile] [corpusfile]" << std::endl << std::endl;
        exit(-1);
    }

    // Load up the list of terms into memory
    std::list<std::string> termslist = std::list<std::string>();
    std::ifstream infile(argv[1]);

    for (std::string term; getline(infile, term); )
    {
        termslist.push_back(term); 
    }

    // Load up the corpus into a string
    std::ifstream t(argv[2]);
    std::string corpus;

    t.seekg(0, std::ios::end);   
    int len = t.tellg();
    corpus.reserve(t.tellg());
    std::cout << "allocating " << len << " bytes" << std::endl;
    t.seekg(0, std::ios::beg);

    char* temptxt = new char[len+1];
    std::istreambuf_iterator<char> txtit(t);
    std::istreambuf_iterator<char> eos;
    int i = 0;
    while (txtit != eos)
    {
        char c = tolower(*txtit);
        
        temptxt[i] = (((c >= 'a') && (c <= 'z')) || (c == '\'')) ? c : ' ';
        txtit++;
        i++;
    }
    temptxt[i] = '\0';;
    corpus.assign(temptxt);
    delete temptxt;

    std::list<std::string>::iterator it = termslist.begin();
    while (it != termslist.end())
    {

        // does it start with the term?
        std::string search = (*it)+" ";
        if (corpus.compare(0, search.length(), search) == 0)
        {
            std::cout << 0 << "," << (*it) << std::endl;
        }


        // does it end with the term?
        search = " "+(*it);
        if (corpus.compare(corpus.length()-search.length()-1, search.length(), search) == 0)
        {
            std::cout << corpus.length()-search.length()-1 << "," << (*it) << std::endl;
        }

        // does it appear in the string?
        search =" "+(*it)+" ";

        size_t pos = corpus.find(search, 0);
        while(pos != -1)
        {
            std::cout << pos << "," << (*it) << std::endl;
            pos = corpus.find(search,pos+1);
        }

        it++;
    }

 }
