#include "INTTRawHitChipDrawer.h"

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>
#include <phool/phool.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TPad.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

INTTRawHitChipDrawer::INTTRawHitChipDrawer(const std::string &name, int which_intt)
  : SingleCanvasDrawer(name)
  , m_which_intt{which_intt}
{
  histprefix = "h_InttRawHitQA_";
}

INTTRawHitChipDrawer::~INTTRawHitChipDrawer()
{
  // Do nothing
}

int INTTRawHitChipDrawer::MakeCanvas()
{
  if(SingleCanvasDrawer::MakeCanvas())
  {
    return 0;
  }

  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  m_canvas->SetTitle((boost::format("INTT chip plots felix %i") % m_which_intt).str().c_str());
  m_canvas->SetCanvasSize(xsize, ysize * 2.2);
  m_canvas->Divide(4, 4);

  return 0;
}

int INTTRawHitChipDrawer::DrawCanvas()
{
  std::cout << PHWHERE << " Beginning" << std::endl;

  QADrawClient *cl = QADrawClient::instance();

  TH1 *h_nhits[14] = {};
  for (int lad = 0; lad < 14; lad++)
  {
    h_nhits[lad] = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%sintt%i_%i") % histprefix % m_which_intt % lad).str().c_str()));
  }

  MakeCanvas();
  m_canvas->SetEditable(true);
  for (int lad = 0; lad < 14; lad++)
  {
    m_canvas->cd(lad + 1);
    gPad->SetRightMargin(0.13);
    gPad->SetLeftMargin(0.13);
    if (h_nhits[lad])
    {
      h_nhits[lad]->SetTitle((boost::format("FEE %i") % lad).str().c_str());
      h_nhits[lad]->DrawCopy("colz");
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.06);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << "Server " << m_which_intt << " FEE Hit Count Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();

  m_canvas->cd(15);
  PrintRun.DrawText(0.5, 0.5, runstring1.c_str());
  m_canvas->Update();
  m_canvas->SetEditable(false);

  std::cout << PHWHERE << " Ending" << std::endl;

  return 0;
}

