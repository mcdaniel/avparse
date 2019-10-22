# avparse
A parser library for FAA and International Aviation Information

# Working : https://www.oreilly.com/library/view/flex-bison/9780596805418/ch01.html

Gang: Lex/Yacc are certainly still used. A more modern tool is Antler (https://www.antlr3.org/) and it can generate code in almost any language. Another kind of parsing that got a lot of buzz last decade is Parsing Expression Grammars (PEG), which generates easier to read code (like recursive descent) and places less restriction on input grammars, with a little loss of efficiency compared to LL and LALR parsing. See http://bford.info/packrat/. 

METAR CODES, REFERENCE:

https://www.weather.gov/media/wrh/mesowest/metar_decode_key.pdf
https://business.desu.edu/sites/business/files/document/16/metar_and_taf_codes.pdf

TODO List

	a) Work on the time string [completed]
	b) Add correction [COR] - see example METAR
	c) Add X/YSM visibility to parsing/processing
	d) Add temperature to processing  [completed]
	e) Add altimeter setting to processing [completed]
	f) Add conditions to setttings, e.g., -DZ, -SN, ... (page 11 of desu link above)
	g) The conditions and coverage lists reorder anything longer than 2 elemetns (need to change this to add new elements onto tail)
	f) change error handling to something more appopriate 