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
	-Last Updated:2017/12/10  - Version 0.1.1
	                            + logic error, English supplied when no switches present fixed. 
                              - Version 0.1.0
                                + English translations added. 
	                            + Ready to start implementing Eng code lookup
                              - Version 0.0.2 + need test memcmp
	                            + Adding parts of speach to definitions. 
	                            + All source is revised to match cpp standards. 
	                            + Synsets returns first synsets encounterd .
	                            + Improved Example Sentences ( almost gaurunteed 
	                              to return an example sentence. 
                                + Fixed logic, can return Examples even if no -D switch
                                  supplied. 
                              - Version 0.0.1
	                            Version 
  
*/
    enum switch_names { FILE_NAME=0, VERSION_MAJOR=0, VERSION_MINOR=1,VERSION_=1 };
    const char * const versLetter = "\0"; // Letter for in-between releases.
    enum COMMAND_SWITCHES { 
         D_Define=1, 
         E_EXTRA_SENTENCES=2,
         S_SYNONYM=3,
         W_WORDNET=4,
         G_GLOSS_ENG=5,
         X_EXAMPLE_ENG=6,
         H_help=7,
         V_version=8,
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
void getDefString( ParseFileClass &WORDNET_DEF, unsigned char *buff, const unsigned char *synID_Number, const int synNum );
int main(const int argc, const char **const argv) {

    const char * WORDNET_DB  = "jpn_wn_lmf.xml";
    const char * WORDNET_EXAMPLES = "wnjpn-exe.tab";
    const char * WORDNET_DEFINITION = "wnjpn-def.tab";

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
    unsigned int sentences = 0; // Random sentences, no format specified
    int synonym   = 0; // Num synonyms to list;
    const unsigned char *term;
    const char *defineLANG_CODE = 0;
    const char *sentenceLANG_CODE = 0;
    // const unsigned char HEADER[4]= { 0xEF, 0xBB, 0xBF, 0x00 };

    // USER INPUT
    term = (unsigned char *)*(argv+argc-1); // Last value MUST BE term;
    
    if(switchIndexes[ D_Define ]  ) 
    {
        define = strToNum(
                            argv[switchIndexes[ D_Define ]],
                            0,
                            strNumlen( 
                                        argv[switchIndexes[ D_Define ]],
                                        0
                                     )
                         );
        if( !define && switchIndexes[ D_Define ] != argc-1 ) 
        {  
            // If not number, must be the 3-letter lang code.
            defineLANG_CODE = argv[switchIndexes[ D_Define ]];
//std::cout << "!define; ";
//std::cout << "argv("<<argv[switchIndexes[ D_Define ]+1] << ")["<<switchIndexes[ D_Define ]+1<<"]; ";

            // Optional 3-letter MAY be supplied. 
            // Try finding a number in NEXT index.
            define = strToNum(
                                argv[switchIndexes[ D_Define ]+1],
                                0,
                                strNumlen( 
                                            argv[switchIndexes[ D_Define ]+1],
                                            0
                                         )
                             );
        }
//std::cout << "define("<< define<<"); ";
        if(!define) { define = 1; }
    }

    if(switchIndexes[ E_EXTRA_SENTENCES ]  ) 
    {
         sentences =  
                     strToNum(
                                argv[switchIndexes[ E_EXTRA_SENTENCES ]],
                                0,
                                strNumlen( 
                                            argv[switchIndexes[ E_EXTRA_SENTENCES ]],
                                            0
                                         )
                             );
        if( !sentences && switchIndexes[ E_EXTRA_SENTENCES ] != argc-1 ) 
        {  
            // If not number, must be the 3-letter lang code.
            sentenceLANG_CODE = argv[switchIndexes[ E_EXTRA_SENTENCES ]];
            
            // Optional 3-letter MAY be supplied. 
            // Try finding a number in NEXT index.
            sentences = strToNum(
                                    argv[switchIndexes[ E_EXTRA_SENTENCES]+1],
                                    0,
                                    strNumlen( 
                                                argv[switchIndexes[ E_EXTRA_SENTENCES]+1],
                                                0
                                             )
                                 );
        }
        if(!sentences) { sentences = 1; }
    }
        
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
    if(switchIndexes[ X_EXAMPLE_ENG ]  ) 
         WORDNET_EXAMPLES = argv[switchIndexes[ X_EXAMPLE_ENG ]];
    if(switchIndexes[ G_GLOSS_ENG ]  ) 
         WORDNET_DEFINITION   = argv[switchIndexes[ G_GLOSS_ENG ]];

    
//    const char *defineLANG_CODE = 0;
//    const char *sentenceLANG_CODE = 0;
    ParseFileClass WORDNET_DEF( WORDNET_DEFINITION );
    ParseFileClass WORDNET_EXE( WORDNET_EXAMPLES );
    
    

//      static OPTIMIZE OPTIMIZE_SOME()    { return OPTIMIZE(1); }
//      static OPTIMIZE OPTIMIZE_MORE()    { return OPTIMIZE(2); }

    kanjiDB::Wordnet_DictClass Wordnet( WORDNET_DB, kanjiDB::OPTIMIZE::NO_OPTIMIZATION ()  );
//      kanjiDB::Wordnet_DictClass Wordnet( WORDNET_DB, kanjiDB::OPTIMIZE::OPTIMIZE_SOME()  );
//      kanjiDB::Wordnet_DictClass Wordnet( WORDNET_DB, kanjiDB::OPTIMIZE::OPTIMIZE_MORE()  );
    if( 1==1 ) { 
        if( !Wordnet.fileLen() ) { 
            std::cout << "Error: "<< WORDNET_DB << " Not Found!\n";
            return 1; 
        }

        unsigned char *GZIP_HEADER = (unsigned char *)"\x1F\x8B\x08";

        unsigned char buff[4];
        // Test if GZIPPED
        Wordnet.readStr(buff, 3, 1);
        int i = 1; // cannot read first str w/readStr;
        while( i<3 && buff[ i-1 ] == GZIP_HEADER[ i ]  ){ i++; }
        if( i == 3 ) { 
            std::cout << "Error: "<< WORDNET_DB << " Needs Unzipped!\n";        
            return 2; 
        }
        
        if( defineLANG_CODE ) { 
            if( !WORDNET_DEF.getFileLength() ) {
                std::cout << "Error: "<< WORDNET_DEFINITION << " Not Found!\n"; 
                return 1; 
            }
            
            WORDNET_DEF.read( buff, 3 );
            i=0;
            while( i<3 && buff[ i ] == GZIP_HEADER[ i ] ) { i++; }
            if( i == 3 ) { 
                std::cout << "Error: "<< WORDNET_DEFINITION << " Needs Unzipped!\n";        
                return 2;                 
            }
        }
        
        if( sentenceLANG_CODE ) { 
            if( !WORDNET_EXE.getFileLength() ) {
                std::cout << "Error: "<< WORDNET_EXAMPLES << " Not Found!\n"; 
                return 1; 
            }
            
            WORDNET_EXE.read( buff, 3 );
            i=0;
            while( i<3 && buff[ i ] == GZIP_HEADER[ i ] ) { i++; }
            if( i == 3 ) { 
                std::cout << "Error: "<< WORDNET_EXAMPLES << " Needs Unzipped!\n";        
                return 2;                 
            }
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
    // synsetIDs are the similar terms(IDs) matching term; 
    std::vector<ustring> synsetIDs = Wordnet.synset();    
    std::vector<ustring> lexiconIds = Wordnet.lexiconID(); 

    const unsigned char **holdSynsetIDs;
    if( synsetIDs.size() ) { holdSynsetIDs = (const unsigned char**)&synsetIDs[0];}
    std::size_t holdSynsetID_Index = 0;
    
    // Print the first synsetID( snynset ) w/a definition
    if( define ) { 
        int numDefined = 0;
        unsigned char grammarNote[150 ];
        while( numDefined < define && holdSynsetID_Index < synsetIDs.size()) { 
            int counter = 0;
            Wordnet.setSynsetPos( holdSynsetIDs[ holdSynsetID_Index ] );
            std::vector<ustring> SynsetId = Wordnet.synRealtions();
            std::vector<ustring> SynTypes = Wordnet.synRealtionTypes();

            // Mods synset Pos; 
            Wordnet.synsetGrammarNote( grammarNote );
            if( Wordnet.defineSynset( buff ) == 0 ) { 
                if( !defineLANG_CODE ) 
                    std::cout << numDefined+1 << " ("<<grammarNote<< "). "<< buff << std::endl;
                    
                numDefined++; counter++;
                if( numDefined == define ) 
                {
                    if( defineLANG_CODE ) { 
                        getDefString( WORDNET_DEF, buff, holdSynsetIDs[  holdSynsetID_Index ], counter  );
                        std::cout << numDefined+1-1 << " ("<<grammarNote<<"). "<< buff << std::endl;
                    } 
                    break; 
                }
            }            
            
            const unsigned char**ccc = (const unsigned char**)&SynsetId[0];
            const unsigned char**relType = (const unsigned char**)&SynTypes[0];
            for(std::size_t a = 0; a < SynsetId.size(); a++) 
            {
                counter = 0;
                Wordnet.setSynsetPos( ccc[ a ]  );
                if( Wordnet.defineSynset( buff ) == 0 ) { 
                    if( !defineLANG_CODE ) 
                        std::cout << numDefined+1 << " ("<<relType[a] << "). "<< buff << std::endl;
                    numDefined++; counter++;
                    if( numDefined == define ) 
                    { 
                        
                        if( defineLANG_CODE ) { 
                            getDefString( WORDNET_DEF, buff, ccc[ a ], counter  );
                            std::cout << numDefined+1-1 << " ("<<relType[a]<<"). "<< buff << std::endl;
                        }
                        break; 
                    }
                }                            
            }
            
            holdSynsetID_Index++;
        }
        
        define = numDefined;         
    }


    // Print the first example Sentence ( starting from currentIndex
    // **Needs changed to (if currentIndex !haveExampleSentence ) 
    //   { find next example w/Term included.; }
    std::vector<ustring> exmapleSentences;
    if( sentences ) { 

        // Special, only used if sentenceLANG_CODE used. 
        // holds index of a given synidSET.
        std::vector<unsigned int> exampleIndexes; 
/*        for(unsigned int synIndex = 0, sentenceGrabbed = 0, synsetSIZE = synsetIDs.size(), SENTENCES_LEFT_TO_GRAB = sentences; 
                sentenceGrabbed < sentences &&
                synIndex < synsetSIZE; 
                    synIndex++, SENTENCES_LEFT_TO_GRAB = sentences - sentenceGrabbed 
           )
        {  
            std::size_t SYNSET_POSITION_FOUND = Wordnet.setSynsetPos( holdSynsetIDs[ synIndex ] );
            if( SYNSET_POSITION_FOUND ) 
            {
                std::vector<ustring> retExamples = Wordnet.examples();
                
                // Check against num left; Don't grab more
                // Sentences than needed. 
                unsigned int examplesSentencesToPUSH_BACK = retExamples.size();
                if( examplesSentencesToPUSH_BACK > SENTENCES_LEFT_TO_GRAB ) 
                {
                    examplesSentencesToPUSH_BACK = SENTENCES_LEFT_TO_GRAB; 
                } 
                
                // Counter through returned Examples adding to examples();
                // Every iteration in this loop adds to sentencesGrabbed; 
                for(unsigned int sentenceCounter=0;
                        sentenceCounter < examplesSentencesToPUSH_BACK; 
                            sentenceCounter++, sentenceGrabbed++
                   )
                {
std::cout << "add0(); ";
                    if ( !sentenceLANG_CODE )     
                        exmapleSentences.push_back( retExamples[ sentenceCounter ] );
                    else { 
                        exmapleSentences.push_back( holdSynsetIDs[ synIndex ] );
                        exampleIndexes.push_back( sentenceCounter ); 
                    }     
                }
            }
        }          
*/        
        int CODE_TO_RETURN = 1;
        if( sentenceLANG_CODE )
            CODE_TO_RETURN = 0; // force grab of synsets instead of sentences; 
        
        // Find any sentences(example) including term;
        if( exmapleSentences.size() < sentences ) { 
            
            std::vector<ustring> retExamples = getExampleSentences( Wordnet, 
                                                                      term,
                                                                      ( sentences - exmapleSentences.size() ),
                                                                      exmapleSentences,
                                                                      CODE_TO_RETURN
                                                                  );
            std::vector<ustring> retExamplesIndexs;
            const unsigned char**retExamplesIndexsChar = 0; 
            if( sentenceLANG_CODE 
            &&  exmapleSentences.size() + retExamples.size() >= sentences  ) { 
                retExamplesIndexs = getExampleSentences( Wordnet, 
                                                           term,
                                                           ( sentences - exmapleSentences.size() ),
                                                           exmapleSentences,
                                                           2
                                                       );
                retExamplesIndexsChar = (const unsigned char**)&retExamplesIndexs[0];    
            }

            int examplesSentencesToPUSH_BACK = retExamples.size();
            for(int i=0; i < examplesSentencesToPUSH_BACK; i++) { 
                if( !sentenceLANG_CODE ) { 
                    exmapleSentences.push_back( retExamples[ i ] );
                }
                else
                {
                    if( retExamplesIndexsChar ) {
                        exmapleSentences.push_back( retExamples[ i ] );
                        std::size_t sentenceIndexNo = strToNum(
                                                                (char *)retExamplesIndexsChar[ i ],
                                                                0,
                                                                strNumlen( 
                                                                            (char *)retExamplesIndexsChar[ i ],
                                                                            0
                                                                         )
                                                              );
                        exampleIndexes.push_back( sentenceIndexNo  ); 
                    }
                    else { // insert dummy vals; 
                        exmapleSentences.push_back( buff );
                        exampleIndexes.push_back( 0 );                         
                    }
                }
            }        
            
            if( exmapleSentences.size() < sentences ) { 
                retExamples = getGlossSentences( Wordnet, 
                                                 term,
                                                 ( sentences - exmapleSentences.size() ),
                                                 exmapleSentences,
                                                 CODE_TO_RETURN
                                               );
                if( sentenceLANG_CODE 
                &&  exmapleSentences.size() + retExamples.size() >= sentences  ) { 
                    retExamplesIndexs = getGlossSentences( Wordnet, 
                                                               term,
                                                               ( sentences - exmapleSentences.size() ),
                                                               exmapleSentences,
                                                               2
                                                           );
                    retExamplesIndexsChar = (const unsigned char**)&retExamplesIndexs[0];    
                }

                int examplesSentencesToPUSH_BACK = retExamples.size();
                for(int i=0; i < examplesSentencesToPUSH_BACK; i++) { 
                    if( !sentenceLANG_CODE )
                    {
                        exmapleSentences.push_back( retExamples[ i ] );
                    }
                    else
                    {
                        if( retExamplesIndexsChar ) {
                            exmapleSentences.push_back( retExamples[ i ] );
                            std::size_t sentenceIndexNo = strToNum(
                                                                    (char *)retExamplesIndexsChar[ i ],
                                                                    0,
                                                                    strNumlen( 
                                                                                (char *)retExamplesIndexsChar[ i ],
                                                                                0
                                                                             )
                                                                  );
                            exampleIndexes.push_back( sentenceIndexNo  ); 
                        }
                        else { // insert dummy vals; 
                            exmapleSentences.push_back( buff );
                            exampleIndexes.push_back( 0 );                         
                        }
                    }
                }        
                
            } 
        }
        
        if( exmapleSentences.size() ) { 
            std::cout << "Sentences: \n";
            for(unsigned int j=0;j < sentences&& j< exmapleSentences.size();j++) { 
                if( !sentenceLANG_CODE )
                    std::cout << j+1   <<". " << exmapleSentences[j] << std::endl;
                else { 
                    if( j == sentences - 1 ) { 
                        const unsigned char**synids = (const unsigned char**)&exmapleSentences[0];    
                        // exmapleSentences[j] actually holds synsetID(NUM ONLY); 
                        getDefString( WORDNET_EXE, buff, synids[ j ], exampleIndexes[j]+1  );
                        std::cout << j+1   <<". " << buff << std::endl;
                    }
                }
            }
        }
    }


    // Print synonyms
    if( synonym ) {
        Wordnet.setSynsetPos( holdSynsetIDs[ 0 ] );        
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
               case 'G':
                    ret[ G_GLOSS_ENG ] = i+1;
                    break;
               case 'X':
                    ret[ X_EXAMPLE_ENG ] = i+1;    
                    
                        
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
    retStr[len-1] = (0x30+(i%10));
    retStr[len ] = '\0';

    return ;
}
std::size_t strToNum(const char *s, const std::size_t index, const int len ) {
    if( !s || !len ) { return 0; }

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
//         <<"jpn_dict.exe [-W jpn_wn_lmf.xml] [-D ## ] [-S ## ] [-E ## ] [-V] [-H] Term\n"
         <<"jpn_dict.exe [-D [lang] [##] ] [-S [##] ] [-E [lang] [##] ]\n"
         <<" [-W jpn_wn_lmf.xml] [-G wnjpn-def.tab] [-X wnjpn-exe.tab] [-V] [-H] Term\n"
// Going to change format to: [-D [eng] [##] ]
// [eng] will modify output to display english and optional ## for a single english translation. 
         << "\nDictionarie -W is Requried. Download First and\n"
         << "supply to program. Default value: jpn_wn_lmf.xml\n"
         << "English meanings for definitions(-G) and examples(-X) are also avaliable.\n"

		<< "\n"
		<< "\t Term The term to lookup.\n"
		<< "\t lang The 3-letter language code for translations. Avaliable options:\n"
		<< "\t English(eng)\n"
		<< "  -D\tDefine Term. If only Term is supplied, will define Term regardless.\n"
		<< "\t Optional: supply the number of maximum definitions to list. Or if\n"
		<< "\t lang was supplied, translates the number supplied.\n"
        << "  -S\tSynonym for Term.\n"
        << "\t Optional: supply the number of maximum synonym to list.\n"
        << "  -E\tExample sentences for Term.\n"
        << "\t Optional: supply the number of maximum sentences to list.\n"
		<< "\t lang was supplied, translates the number supplied.\n"
		<< "  -H\tPrints this help.\n"
		<< "  -V\tPrints the version\n"
		<< "\n"
		<< "  Example Usage: \n"
        << "main.exe -D -S 99 -E 99 TERM\n"
		<< "\t This is command will print one definition and up to 99 sentences\n"
		<< "\t and synsets for TERM\n"
		<< "\n";

} 

void version(const char **const argv) { 
    std::cout << *(argv+0) << " Version: "<< VERSION_MAJOR << VERSION_MINOR << VERSION_ << versLetter<< std::endl;
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
    
    while( (isNextMatchFound = FILE.findPos( s, endDelim ))  ) {  
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


void getDefString( ParseFileClass &WN_D, 
              unsigned char *buff,
              const unsigned char *const synID,
              const int synNumber )
{
    buff[0] = '\0';
    if( !synNumber ) { return ; }
    
    WN_D.setGetPointer( 0 );
    int LEN = 800;
    std::size_t pos = 0;
    for(int i = 0; i < synNumber; i++)  
    {
        pos = WN_D.findPos( synID );
        WN_D.setGetPointer( pos+15 ); // Probably ownt skip entry;
    }
    WN_D.setGetPointer( pos );
    
    const unsigned char *const TAB = (unsigned char *)"\x09";
    WN_D.getLine( buff, LEN, TAB ); // grabbed ID+grammar
    WN_D.getLine( buff, LEN, TAB ); // grabbed index
    
    WN_D.getLine( buff, LEN, TAB ); // grabbed English!
    
    return ;
}
