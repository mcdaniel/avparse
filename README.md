# avparse
A parser library for FAA and International Aviation Information

# Working : https://www.oreilly.com/library/view/flex-bison/9780596805418/ch01.html

Gang: Lex/Yacc are certainly still used. A more modern tool is Antler (https://www.antlr3.org/) and it can generate code in almost any language. Another kind of parsing that got a lot of buzz last decade is Parsing Expression Grammars (PEG), which generates easier to read code (like recursive descent) and places less restriction on input grammars, with a little loss of efficiency compared to LL and LALR parsing. See http://bford.info/packrat/. 


