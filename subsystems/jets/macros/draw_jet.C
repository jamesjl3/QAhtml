#include <qahtml/QADrawClient.h>
#include <iostream>
#include <qahtml/jet/JetDraw.h>
#include <sPhenixStyle.C>
#include <TFile.h>
#include <TSystem.h>

R__LOAD_LIBRARY(libqadrawjet.so)



// ----------------------------------------------------------------------------
//! Draw jet QA
// ----------------------------------------------------------------------------
/*! Short ROOT macro to test the Jet QADrawClient.
 *  Reads histograms in `infile` and saves produced
 *  plots to `outfile`.
 *
 *  \param[in]  infile   input histogram ROOT file
 *  \param[out] outfile  output ROOT file to save plots to
 *  \param      do_debug turn on (true)/ off (false) debugging messages
 *  \param      do_html  turn on (true)/ off (false) trying to make html page
 */
void draw_jet(const std::string& infile,
              const std::string& outfile,
              const bool do_debug = false,
              const bool do_html = false)
{

  // set plotting style to sPHENIX
  SetsPhenixStyle();
  if (do_debug)
  {
    std::cout << " --- Testing JetDraw\n"
              << " --- Style set to sPHENIX"
              << std::endl;
  }

  // create instance of relevant module
  JetDraw* jets = new JetDraw();
  jets -> SetDoDebug(do_debug);

  // create draw client
  QADrawClient* client = QADrawClient::instance();
  client -> registerDrawer(jets);
  if (do_debug)
  {
    std::cout << " --- JetDraw client registered" << std::endl;
  }

  // read histograms from file
  client -> ReadHistogramsFromFile(infile);
  if (do_debug)
  {
    std::cout << " --- Histograms read" << std::endl;
  }

  // make html page if needed
  if (do_html)
  {
    client -> MakeHtml("ALL");
    if (do_debug) std::cout << " --- HTML page made" << std::endl;
  }
  else
  {
    jets -> Draw("ALL");
    if (do_debug) std::cout << " --- Drew plots" << std::endl;
  }

  // save histograms
  TFile* output = new TFile(outfile.data(), "recreate");
  jets -> SaveCanvasesToFile(output);
  if (do_debug)
  {
     std::cout << " --- Save plots to file" << std::endl;
  }

  // delete QADrawClient instance
  delete client;
  if (do_debug)
  {
    std::cout << " --- Deleted QADrawClient, exiting" << std::endl;
  }

  // exit ROOT and return
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
