> Now, a few words on looking for things. When you go looking for something specific, your chances of finding it are very bad. Because of all the things in the world, you're only looking for one of them. When you go looking for anything at all, your chances of finding it are very good. Because of all the things in the world, you're sure to find some of them.
>- Daryl Zero, ["Zero Effect"](https://www.imdb.com/title/tt0120906/)



# Tagger
### Searching text for thousands of search terms simultaneously in linear time
  
  
What do you do if you want to match mb or gb of text against tens of thousands of different search terms?  You get *Tagger*.

*Tagger* takes a 'terms' file, containing all of the search terms to match (these can include spaces), and a 'corpus' file (the text to search) and outputs a list of all matching terms and their locations.


### Building *tagger*

There is an include Makefile, just run `make`.

   
### Running *tagger*

`bin/taggerapp [terms file] [corpus file]`

Example: 
  
`bin/taggerapp texts/cities.txt texts/mobydick.txt`

This will return the list of all the city names that can be found in Moby Dick.


### Specifying a terms file

The terms file contains the list of strings you're looking for, one word per line.  Terms can include multiple-choice groups using [] notation.  A sample terms file might look like this:

```text
one
term  
per  
line
they can contain spaces
and[yi] ; matches both andy & andi
```

