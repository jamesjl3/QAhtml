// TestDrawingJetQA.C
#include <qahtml/OnlProdClient.h>
#include <qahtml/JetDraw.h>

R__LOAD_LIBRARY(libonlprodexample.so)

// draws jet pt for a specific R + trigger
void DrawJetPt(TFile* inFile, std::string histname, std::string rJetLabel, std::string trigLabel) {

  TH1D* hJetPt = (TH1D*) inFile -> Get( histname.data() );
  /* then do drawing stuff */

} 

// main function
void TestDrawingJetQA(std::string inFileName) {

  /* open input file */
  /* maybe open an output file? */

  for (/* looping through Rjet values */) {
    for (/* looping through trigger values */) {

      std::string rJetLabel = GetRLabel(rJetIndex);
      std::string trigLabel = GetTrigLabel(trigIndex);
      std::string ptHistName  = GetPtHistName(rJetIndex, trigIndex); 

      DrawJetPt(inFile, ptHistName, rJetLabel trigLabel);

    }
  }

  /* then do ending stuff */

}
