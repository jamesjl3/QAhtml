#include "INTTRawHitSummaryDrawer.h"

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

INTTRawHitSummaryDrawer::INTTRawHitSummaryDrawer(const std::string &name)
  : SingleCanvasDrawer(name)
{
  histprefix = "h_InttRawHitQA_";
}

INTTRawHitSummaryDrawer::~INTTRawHitSummaryDrawer()
{
  // Do nothing
}

int INTTRawHitSummaryDrawer::MakeCanvas()
{
  std::cout << PHWHERE << " Beginning" << std::endl;

  if(SingleCanvasDrawer::MakeCanvas())
  {
    return 0;
  }

  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  m_canvas->SetCanvasSize(xsize, ysize * 2.2);

  int nrow = 4;
  double yoffset = 0.02;
  double ywidth = (1. - yoffset - yoffset) / (double) nrow;
  double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  for (int i = 0; i < 2 * nrow; i++)
  {
    if (i % 2 == 0)
    {
      x1 = 0.5;
      x2 = 0.95;
    }
    else if (i % 2 == 1)
    {
      x1 = 0.05;
      x2 = 0.5;
    }
    y1 = 0.02 + (i / 2) * ywidth;
    y2 = 0.02 + (i / 2 + 1) * ywidth;

    Pad[2 * nrow - 1 - i] = new TPad(
        (boost::format("mypad_%1%_%2%") % 0 % i).str().c_str(),
        "pad",
        x1, y1, x2, y2);
  }

  for (int i = 0; i < 2 * nrow; i++) Pad[2 * nrow - 1 - i]->Draw();

  // this one is used to plot the run number on the canvas
  transparent = new TPad((boost::format("transparent%d") % 0).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent->SetFillStyle(4000);
  transparent->Draw();

  std::cout << PHWHERE << " Ending" << std::endl;

  return 0;
}

int INTTRawHitSummaryDrawer::DrawCanvas()
{
  std::cout << PHWHERE << " Beginning" << std::endl;

  QADrawClient *cl = QADrawClient::instance();
  TH1 *nhit = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%snhit") % histprefix).str().c_str()));
  TH1 *nhitnorth = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%snhit_north") % histprefix).str().c_str()));
  TH1 *nhitsouth = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%snhit_south") % histprefix).str().c_str()));
  TH1 *pid = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%spid") % histprefix).str().c_str()));
  TH1 *adc = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%sadc") % histprefix).str().c_str()));
  TH1 *bco = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%sbco") % histprefix).str().c_str()));
  TH1 *bcofull = dynamic_cast<TH1 *>(cl->getHisto((boost::format("%sbco_full") % histprefix).str().c_str()));

  MakeCanvas();
  m_canvas->SetEditable(true);

  Pad[0]->cd();
  if (nhit)
  {
    nhit->GetXaxis()->SetTitle("N_{hit}");
    nhit->DrawCopy();
  }
  else
  {
    return -1;
  }
  Pad[1]->cd();
  if (nhitnorth)
  {
    nhitnorth->GetYaxis()->SetTitle("N_{hit}");
    nhitnorth->SetTitle("INTT Hits North");
    nhitnorth->DrawCopy();
  }
  else
  {
    return -1;
  }
  Pad[2]->cd();
  if (nhitsouth)
  {
    nhitsouth->GetYaxis()->SetTitle("N_{hit}");
    nhitsouth->SetTitle("INTT Hits South");
    nhitsouth->DrawCopy();
  }
  else
  {
    return -1;
  }
  Pad[3]->cd();
  if (pid)
  {
    pid->GetXaxis()->SetTitle("Packet ID");
    pid->DrawCopy();
  }
  else
  {
    return -1;
  }
  Pad[4]->cd();
  if (adc)
  {
    adc->DrawCopy();
  }
  else
  {
    return -1;
  }
  Pad[5]->cd();
  if (bco)
  {
    bco->GetXaxis()->SetTitle("Crossing");
    bco->DrawCopy();
  }
  else
  {
    return -1;
  }
  Pad[6]->cd();
  if (bcofull)
  {
    bcofull->DrawCopy();
  }
  else
  {
    return -1;
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.06);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << m_name << "_intt Summary Info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  Pad[7]->cd();
  PrintRun.DrawText(0.5, 0.5, runstring1.c_str());

  m_canvas->SetEditable(false);
  m_canvas->Update();

  std::cout << PHWHERE << " Ending" << std::endl;

  return 0;
}

