#ifndef WORDNET_DICTCLASS
#define WORDNET_DICTCLASS
#include <cstring>
#ifndef KANJI_INFO_CLASS
  #include "KanjiInfoClass.cpp" // pure-poly-base class; 
#endif

namespace kanjiDB { 
          // Incorrectly derived from KanjiInfoClass ( Not a Kanji info class )
          //  We're goin' to roll with it thought. Probably should simply inherit
          //  the class rather than derive from...
class Wordnet_DictClass: public KanjiInfoClass {
    private:
            /* const */ std::size_t fileLength;      // used for bounds checking
//            const unsigned char * const file;
            Wordnet_DictClass& operator=( const Wordnet_DictClass &other );  // no assignment op
            Wordnet_DictClass( const Wordnet_DictClass& other );             // no copy constructor
            Wordnet_DictClass(); // No default ctor
            std::size_t  *keytable_DefinitionPos; // holds pos of NUMERIC_VALUE of kanji ( not computated, simply a number assoc. w/Kanji )
            std::size_t savedSynset;
    const unsigned char *const SYNSET_STR; // = (unsigned char *)"synset='jpn-1.1-"; // 01211019-n'/>
    const unsigned char *const END_LEXICON_ENTRY_STR; // = (unsigned char *)"</LexicalEntry>";
    const unsigned char *const SYNSET_TAG; //<Synset id='jpn-1.1-XXXXXXXX-a' baseConcept='3'>
    const unsigned char *const GLOSS_STR; // <Definition gloss="???">
    const unsigned char *const END_GLOSS_STR; // See above;
    const unsigned char *const EXAMPLE_STR;     // <Statement example="
    const unsigned char *const END_EXAMPLE_STR; // "/> 
    const unsigned char *const END_SYNSET_TAG;
    const unsigned char *const SYNSET_RELATION; // <SynsetRelation targets='jpn-1.1-01509066-a' relType='sim'/>
    const unsigned char *const WRITTEN_FORM;    //       <Lemma writtenForm='ŒÛ•‘Œƒ—ã' partOfSpeech='n'/>
    const unsigned char *const SENSE_ID_WRITTEN; // <Sense id='w230510_00668805-v' synset='jpn-1.1-00668805-v'/>
    const unsigned char *const END_SENSE_ID_WRITTEN; // "_"; 




// relType='dmnc' / hype / sim / hypo / hprt / inst / dmnr / mprt / hmem / dmnu / also
    public:
            explicit Wordnet_DictClass(const char fName[]);
           ~Wordnet_DictClass();

    private:
           unsigned int termValue(const std::size_t pos) const;
           unsigned int termValue(const unsigned char *const) const;
           unsigned int ballparkIndexVal( const unsigned char *const ) const;

    public:
           // if setKeyTable() is NOT SET, changes behavior to position instead of indexVal;
           std::vector<ustring> synset(const unsigned int termPosition) const;
           std::vector<ustring> synset(const unsigned char *const term) const;
           
           std::vector<ustring> lexiconID(const unsigned int termIndex) const;
           std::vector<ustring> lexiconID(const unsigned char *const term) const;
           
                      
           std::size_t  setSynsetPos(const unsigned char *const synsetID);
           void setSynsetPos(const std::size_t i) { if(i<fileLen()){ savedSynset = i;} }
           int  defineSynset(unsigned char *ret) const;
           std::vector<ustring> examples() const;
           std::vector<ustring> synRealtions() const;
           
           std::vector<ustring> kunyomi  ();
           std::vector<ustring> onyomi   ();
           int setKanji( const unsigned char *k); // virtual
                      

           
           void getTermNumber( char *retVal, std::size_t termPos );
           
           // Should only apply to namespace KanjiInfoClass {}; 
           // (BELOW functions);           
           
           // Actual private functions to Wordnet_DictClass; 
//           void fillKeyTable(); 
};



void func(Wordnet_DictClass &term) {
 std::cout << "do something" << std::endl;
}

} // NAMESPACE


#endif

