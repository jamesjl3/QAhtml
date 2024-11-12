#include <qahtml/QADrawClient.h>
#include <iostream>
#include <qahtml/jet/JetDraw.h>
#include <sPhenixStyle.C>
#include <TSystem.h>

R__LOAD_LIBRARY(libqadrawjet.so)


// ----------------------------------------------------------------------------
//! Draw jet QA
// ----------------------------------------------------------------------------
void draw_jet(const std::string& rootfile, const bool do_debug = false) {

  // set plotting style to sPHENIX
  SetsPhenixStyle();
  if (do_debug) {
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
  if (do_debug) {
    std::cout << " --- JetDraw client registered" << std::endl;
  }

  // read histograms from file
  client -> ReadHistogramsFromFile(rootfile);
  if (do_debug) {
    std::cout << " --- Histograms read" << std::endl;
  }

  // make html page
  client -> MakeHtml();
  if (do_debug) {
    std::cout << " --- HTML page made" << std::endl;
  }

  // delete QADrawClient instance
  delete client;
  if (do_debug) {
    std::cout << " --- Deleted QADrawClient, exiting" << std::endl;
  }

  // exit ROOT and return
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
