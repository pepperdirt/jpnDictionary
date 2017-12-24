#include <iostream>
#include <fstream>

// # include ParseFileClass_VERSION[]= "2.0.1";
// # include jmdict_InfoClass_VERSION[]= "0.0.1";
#include "Wordnet_DictClass.cpp"    // Wordnet
// # include kanjiDict2_InfoClass_VERSION[]= "0.0.1";
// # include ParseFileClass_VERSION[]= "2.0.1";



/*
 * Programmer:	Pepperdirt
 * github:	github.com/pepperdirt
 *
	-Last Updated:2017/12/10  - Version 0.0.1
  
*/
    enum switch_names { FILE_NAME=0, VERSION_MAJOR=0, VERSION_MINOR=0,VERSION_=1 };
    enum COMMAND_SWITCHES { 
         D_Define=1, 
         E_EXTRA_SENTENCES=2,
         S_SYNONYM=3,
         W_WORDNET=4,
         H_help=5,
         V_version=6,
         END_TERMINATOR=0
    };

std::ostream& operator << (std::ostream& stream, const ustring& str) {
//    if (const auto len = str.size())
int len = str.size();
       stream.write(reinterpret_cast<const char*>(&str[0]), len);
    return stream;
}  



// std::size_t findPos( const unsigned char *const s, const unsigned char *const pin, const std::size_t index );
std::size_t strToNum(const char *const s, const std::size_t index, const int len );
void numToStr(char *const retStr, const std::size_t i);
int strNumlen(const char *s, const std::size_t index );
void help(); 
void getSwitchIndex( unsigned int *const ret, const int argc, const char **const argv );
void getInputFieldNums( std::vector<int>, const char **const argv, const int index );
void version(const char **const argv);

std::size_t numLines(ParseFileClass & file);
std::size_t largestDelimField(ParseFileClass & file, const unsigned char *delim);

std::vector<std::size_t> posOfKanji(ParseFileClass &, unsigned char *);
std::vector<std::size_t> findOffsetFromPos(ParseFileClass &FILE,
                                           std::vector<std::size_t> pos,
                                           std::vector<ustring> s,
                                           int searchDirection = 0);
std::size_t largestSize(std::vector<std::size_t> begOff,
                        std::vector<std::size_t> endOff );

int main(const int argc, const char **const argv) {

    int doAnything = 0; // Commands for outputing to screen
                        // If none, at least define term ( if provided )
    const char * WORDNET_DB = "jpn_wn_lmf.xml";


    if( argc == 1 ) { help(); return 0; }
    
    unsigned int switchIndexes[V_version+2];
    for(int i=0; i <= V_version; i++) { switchIndexes[ i ] = '\0'; } 
    switchIndexes[ V_version+1 ] = '\0';
    getSwitchIndex( 
        switchIndexes,
        argc,
        argv
    );

    int define = 0; // Define Term?
    if( switchIndexes[ H_help ]    ) { help(); return 0; }
    if( switchIndexes[ V_version ] ) { version(argv); return 0; }
    int sentences = 0; // Random sentences, no format specified
    int synonym   = 0; // Num synonyms to list;
    const unsigned char *term;
    const char HEADER[4]= { 0xEF, 0xBB, 0xBF, 0x00 };
    
    // USER INPUT
    term = (unsigned char *)*(argv+argc-1); // Last value MUST BE term;
    
    if(switchIndexes[ D_Define ]  ) 
        define = 1;
    if(switchIndexes[ E_EXTRA_SENTENCES ]  ) 
         sentences =  
         strToNum(
                    argv[switchIndexes[ E_EXTRA_SENTENCES ]],
                    0,
                    strNumlen( 
                                argv[switchIndexes[ E_EXTRA_SENTENCES ]],
                                0
                             )
                 );
    if(switchIndexes[ E_EXTRA_SENTENCES ] && !sentences  ) {  sentences = 1; }
        
    if(switchIndexes[ S_SYNONYM ]  ) 
        synonym  = 
        strToNum(
                    argv[switchIndexes[ S_SYNONYM ]],
                    0,
                    strNumlen( 
                                argv[switchIndexes[ S_SYNONYM ]],
                                0
                             )
                 );
    if(switchIndexes[ S_SYNONYM ] && !synonym  ) {  synonym = 1; }
        
    if(switchIndexes[ W_WORDNET ]  ) 
         WORDNET_DB = argv[switchIndexes[ W_WORDNET ]];


    kanjiDB::Wordnet_DictClass Wordnet( WORDNET_DB, kanjiDB::OPTIMIZE::NO_OPTIMIZATION ()  );
    if( 1==1 ) { 
        if( !Wordnet.fileLen() ) { 
            std::cout << "Error: "<< WORDNET_DB << " Not Found!\n";
            return 1; 
        }

        unsigned char *GZIP_HEADER = (unsigned char *)"\x1F\x8B\x08";

        unsigned char buff[4];
        // Test if GZIPPED
        Wordnet.readStr(buff, 3, 1);
        int i = 0;
        while( i<3 && buff[ i ] == GZIP_HEADER[ i ]  ){ i++; }
        if( i == 3 ) { 
            std::cout << "Error: "<< WORDNET_DB << " Needs Unzipped!\n";        
            return 2; 
        }
    }

    if( term && term[0] && !define && !sentences && !synonym ) { 
        define = 1; // At least define term; 
    }
    unsigned char buff[800];
    
    // Sets the term for Wordnet to use;
    if( Wordnet.setKanji( term ) != 0 ) {
        std::cout << "term( "<<term<<" ) not found.\n";
        return 1;
    } 
    std::size_t INDEX_OF_TERM = Wordnet.getIndex();  // Term ids
    // synsetIDs are the similar terms(IDs) matching term; 
    std::vector<ustring> synsetIDs = Wordnet.synset();    
    std::vector<ustring> lexiconIds = Wordnet.lexiconID(); 


    const unsigned char **holdSynsetIDs;
    if( synsetIDs.size() ) { holdSynsetIDs = (const unsigned char**)&synsetIDs[0];}
    int holdSynsetID_Index = 0;
    int currentIndex = 0;
    
    // Print the first synsetID( snynset ) w/a definition
    if( define ) { 
        int numDefined = 0;
        while( numDefined < define && holdSynsetID_Index < synsetIDs.size()) { 
            Wordnet.setSynsetPos( holdSynsetIDs[ holdSynsetID_Index ] );
            if( Wordnet.defineSynset( buff ) == 0 ) { 
                std::cout << buff << std::endl;
                numDefined++;
                if( numDefined == define ) { break; }
            }            
            holdSynsetID_Index++;
        }
        
        define = numDefined; 
    }
    
    INDEX_OF_TERM = holdSynsetID_Index;

if( define ) { 
    // Print the first example Sentence ( starting from currentIndex
    // **Needs changed to (if currentIndex !haveExampleSentence ) 
    //   { find next example w/Term included.; }
    std::vector<ustring> exmapleSentences;
    if( sentences ) { 
            std::size_t HOLD_POS = Wordnet.setSynsetPos( holdSynsetIDs[ holdSynsetID_Index ] ); // re-lookup

            Wordnet.setIndex( INDEX_OF_TERM );

            exmapleSentences = Wordnet.examples();
            if( exmapleSentences.size() == 0  ) { 
                // Find next example that contains TERM;
                
                // std::vector<ustring> lexiconIDs = lexiconID( INDEX_OF_TERM );
                // No examples present; iterate through lexiconIDs, finding Position,
                int i =0;
                while( i  < synsetIDs.size() ) { 
                    Wordnet.setSynsetPos( holdSynsetIDs[ i ] );
                    if( Wordnet.defineSynset( buff ) == 0 ) { 
                        exmapleSentences = Wordnet.examples();
                        if( exmapleSentences.size() ) { break; }
                    }
                
                    i++;
                }
            }
            
            if( exmapleSentences.size() == 0 ) { 
                // Find another Example sentence containing term; 
                int a;    
            }
            
            if( exmapleSentences.size() ) { 
                std::cout << "Sentences: \n";
                for(int j=0;j < sentences&& j< exmapleSentences.size();j++) { 
                    std::cout << exmapleSentences[j] << std::endl;
                }
            }
            
    }


    // Print synonyms
    if( synonym ) {
        std::vector<ustring> termsMatchingSynsets = synsetIdWrittenForm( Wordnet );
        const int matchSize = termsMatchingSynsets.size();
        if( matchSize ) { 
            std::cout << "Synsets: " << std::endl
                      << termsMatchingSynsets[ 0 ];  
        }
        
        for(int i = 1; i < synonym && i < matchSize; i++) {
            std::cout << "; " << termsMatchingSynsets[ i ];
        }
    }

std::cout << std::endl;
} // not define; 
//    std::cout << "END\n";
    

    return 0;
}

void getSwitchIndex(unsigned int *const ret, const int argc, const char **const argv ) 
{
    for(int i=0; i < argc; i++) { ret[i] = 0; }
    
//         <<"kanjiInfo.exe [-I] [-D Delim] [-N ##] [-S ##,##,##,etc] \n
//         << "\t[-P ##] [-F] [-O OutputFile] [-K [ F ] [ O ] [ K ] [ S ]]  \n"
//         << "\t[-M]\n"
// *********************** USER INPUT HANDLER *******************************
    if(argc>0)
    {    /* Exits loop when at the second to last Pointer */
        for(int i=1;i<argc;i++)
        {
            if( *argv[i]=='-')
            {
               switch( toupper(*(argv[i]+1) ) )
               {                       
               case 'D':
                    ret[ D_Define ]   = i+1;
                    break;                       
               case 'S':
                    ret[ S_SYNONYM ] = i+1;
                    break;
               case 'E':
                    ret[ E_EXTRA_SENTENCES ] = i+1;
                    break;
               case 'W':                
                    ret[ W_WORDNET ] = i+1; 
                    break;
               
               
               case 'H': 
                    ret[ H_help ] = i+1; 
                    break;
               case 'V': 
                    ret[ V_version ] = i+1; 
                    break;


               default:
               break;
               }
            }
        }
    }

// *********************** END USER INPUT HANDLER ***************************

}
void numToStr(char *const retStr, const std::size_t i)
{
    std::size_t len = 0;
    std::size_t mult = 1;
    while( mult ==1 || mult <= i ) { mult *= 10; len++; }
    if( len == 0 ) { retStr[0]=0x30; retStr[1]='\0'; return ; }
    mult /= 10;
    
    std::size_t copyI = i;
    for(unsigned int c=0, num=0; c < len-1; c++, mult /= 10) {
        num = (copyI / mult );
        copyI -=  (num * mult );
        retStr[c] = num + 0x30;
    }
    copyI = i % 10;
    retStr[len-1]= copyI;
    retStr[len-1] += 0x30;
    retStr[len ] = '\0';

    return ;
}
std::size_t strToNum(const char *s, const std::size_t index, const int len ) {
    if( !s || !len || index < 0 ) { return 0; }

    std::size_t ret = 0;
    std::size_t mult = 1; 
    for(int i=len-1; i >= 0; i--, mult*=10) { ret+= (s[ index+i ]-0x30) * mult;  }

    return ret; 
}

int strNumlen(const char *s, const std::size_t index ) { 
    if( !s ) { return 0 ; }
    int i = 0;
    while( s[ index+i ] > 0x2F && s[ index+i ] < 0x3A ) { i++; }

    return i; 
}


// input should be delimiterized list of commas and numbers
// Convert numbers and place into *ret; 
// Arg1: return value, holds ints recovered from user input
// Arg2: CLI user input
// Arg2: Index value of *(argv+?) where string resides;
void getInputFieldNums( std::vector<int> ret, const char **const argv, const int index )
{
    int numIntsAddedToRet = 0;
    for(int i=0; argv[index][i]; i++){ 
        int numLen = strNumlen( argv[index], i );
        if( numLen ) {
            ret.push_back( strToNum( argv[i], i, numLen ) );
            numIntsAddedToRet++;
            i+= numLen-1; // Dont skip past NULL_TERMINATOR;
        } 
    }
    
    return ;
}



void help() { 

std::cout << "CLI Japanese to Japanese dictionary.\n"
         <<"jpn_dict.exe [-D] [-C Delim] [-S [ ## ]] [-E [ ## ]] [-V] [-H] Term\n"

         << "\nDictionarie -W is Requried. Download First and\n"
         << "supply them to program. Default values: JMdict.xml / kanjidic2.xml\n"

		<< "\n"
		<< "\t Term The term to lookup.\n"
		<< "  -D\tDefine Term.\n"
        << "  -S\tSynonym for Term.\n"
        << "\t Optional: supply the number of maximum synonym to list.\n"
        << "  -E\tExample sentences for Term.\n"
        << "\t Optional: supply the number of maximum sentences to list.\n"
		<< "  -H\tPrints this help.\n"
		<< "  -V\tPrints the version\n"
		<< "\n"
		<< "  Example Usage: \n"
        << "jpn_dict.exe -D JMdict_e_ALL_LANGUAGES.xml -W kanjidic2.xml -i list.txt\n"
        << "-K K O T Y -o out.cvs -M -C ;\n"
		<< "\t This is command will use default Delim of \"newLine\" in file list.txt\n"
		<< "\t and output the delim (-C) ';' to out.cvs. It will use default \n"
        << "\t search field (-S), field 1 and output (-K) On/Kun yomi, Translation,\n"
        << "\t and Translation of Kun-Yomis to output out.csv using HTML styling(-M).\n"
		<< "\n";

} 

void version(const char **const argv) { 
    std::cout << *(argv+0) << " Version: "<< VERSION_MAJOR << VERSION_MINOR << VERSION_ << std::endl;
}


std::size_t numLines(ParseFileClass & file)
{
    const unsigned char *delim = (const unsigned char *)"\x0A";
    std::size_t pos = 0;
    std::size_t numLines = 0;
    while( (pos = file.findPos(delim) ) ) {
        numLines++; 
        file.setGetPointer( pos + 1 );
    }
    
    return numLines;
}

std::size_t largestDelimField(ParseFileClass & file, const unsigned char *delim)
{
    std::size_t pos = 0;
    std::size_t lastPos = 0;
    std::size_t MAX_LINE_LENGTH = 0;
    std::size_t lineLen = 0;
    
    while( (pos = file.findPos(delim) ) ) {
        lineLen = pos - lastPos;
        if( lineLen > MAX_LINE_LENGTH ) { MAX_LINE_LENGTH = lineLen; }
        
        file.setGetPointer( pos + 1 );
        lastPos = pos + 1;
    }

    return MAX_LINE_LENGTH;
}


std::vector<std::size_t> posOfKanji(ParseFileClass &FILE,
                                    unsigned char *s)
{
    std::vector<std::size_t> posFound;
    std::size_t isNextMatchFound = 0;
    const unsigned char endDelim[] = "\0";
    
    while( isNextMatchFound = FILE.findPos( s, endDelim )  ) {  
        posFound.push_back( isNextMatchFound );
        FILE.setGetPointer(isNextMatchFound+1);
    }
    
    return posFound; 
}


// Returns vector of positions;
//  Positions are offsets from POS[0] of file(* FILE);
//  Returns offset supplied if no match found ( std::vector pos ); 

// Use Parse File Class == arg1
// arg2: starting position
// arg3: str to look for from pos(arg2);
// arg4 direction of serch( + or - );
// Returns position found; ( 1st[CLOSEST] found in vector s;
std::vector<std::size_t> findOffsetFromPos(ParseFileClass &FILE,
                                           std::vector<std::size_t> pos,
                                           std::vector<ustring> s,
                                           int searchDirection) // 0 == +; 1 == negative search direction
{
    int sign = 1;
    if( searchDirection ) { sign = -1; } 

    std::vector<std::size_t> posFound;
    std::size_t isNextMatchFound = 0;
    std::size_t counter = 0;
    const std::size_t S_SIZE = s.size();
    const std::size_t SIZE = pos.size();
    const unsigned char endDelim[] = "\0";
    std::size_t found = 0;
    std::size_t smallestFound = 0;
    
    std::size_t iter = 0;
    while( counter < SIZE ) { 
        FILE.setGetPointer( pos[counter] );
        smallestFound = 0;

        iter = 0;
        while( iter < S_SIZE ) {
            const unsigned char **hold_ustring = ((const unsigned char **)&s[iter]);
            // ** If s[iter] is !found, will return 0( beg of file );  
            found = FILE.findPos( hold_ustring[0],
                                  endDelim,
                                  0,
                                  searchDirection);

            // period: "\xE3\x80\x82";                                  
            if( found && hold_ustring[0][0] // at least 3 index values avaliable.
                      && hold_ustring[0][1]
                      && hold_ustring[0][0] == 0xE3 
                      && hold_ustring[0][1] == 0x80 
                      && hold_ustring[0][2] == 0x82   ) {
            
                std::size_t pos2 = found;
                if(  searchDirection ) { // (-) direction
                    pos2-= 3; // go back delimLen in attempt to find period again;
                } else {
                    pos2+= 3; // Skip delim(period); in attempt to find periond again;
                }                             
                
                const std::size_t SAVED_POS = FILE.getPositionPointer();
                FILE.setGetPointer(pos2 );
                
                if( FILE.findPos( hold_ustring[0],
                              endDelim,
                              0,
                              searchDirection)
                 == pos2 ) 
                { 
                    // Search until first pos is NOT enter char(JPN);
                    // (first char: curr position; (MULTIPLE period SKIP; Not end delim ));
                    while( 
                    (found = FILE.findPos( hold_ustring[0],
                                  endDelim,
                                  0,
                                  searchDirection)
                    ) == pos2 ) 
                    { 
                        if( !found || found < 3 ) { break; }
                        if(  searchDirection ) { // (-) direction
                            found-= 3; // go back delimLen in attempt to find period again;
                        } else {
                            found+= 3; // Skip delim(period); in attempt to find periond again;
                        }                             
                        
                        FILE.setGetPointer( found );
                        pos2 = found; 
                        
                    }
                } // END IF;
                
                FILE.setGetPointer(SAVED_POS);
            }

            // Skips delim character ( in (NEGATIVE) direcion );
            if( searchDirection  ) {
                if( found ) { 
                    int lengthustring = 0;
                    while( hold_ustring[0][lengthustring] ) { lengthustring++; }
                    found += lengthustring;
                }
            }
            
            if( !smallestFound || 
                (sign * found) < (sign * smallestFound) ) { 
                // Search direction deterimines if smaller or
                // larger value ( value closest to original Point( setGetPointer() )
                if( found ) { 
                    smallestFound = found; 
                } else { 
                    if( searchDirection ) { 
                        // NEGATIVE search direction ( possible 0 );
    
                        // Verify if match found at 0 or no match found
                        FILE.setGetPointer( 0 );
                        unsigned char tmpchar[3+1];
                        
                        // Command will read input (UNTIL delim str(arg3) is found)
                        // If arg3 is found at pos(0), will not fill tmpchar with anyhting; 
                        FILE.getLine(tmpchar,
                                     3,
                                     ((const unsigned char **)&s[iter])[0] );
                        if( tmpchar[0] == '\0' ) {
                            // Was indeed found at position 0.  
                            smallestFound = 0;
                        }
                        FILE.setGetPointer( pos[counter] );
                    }
                }
            }
            iter++; 
        }
        
        
        // Fix Positive search to AT LEAST be equal to starting pos; 
        if( !searchDirection && !smallestFound ) {
            posFound.push_back( pos[counter] );
        } 
        else { // Negative direction form offset;  
            posFound.push_back( smallestFound );
        }
        counter++;
    }
    
    return posFound; 
}

std::size_t largestSize(std::vector<std::size_t> begOff,
                        std::vector<std::size_t> endOff )
{
    std::size_t largestDifference = 0;
    std::size_t difference =0;
    const std::size_t SIZE = begOff.size();
    
    for(std::size_t i=0; i < SIZE; i++) { 
        difference = endOff[i] - begOff[i];

        if( difference > largestDifference ) 
        { 
            largestDifference = difference;  
        }
    }
    
    return largestDifference;
}
