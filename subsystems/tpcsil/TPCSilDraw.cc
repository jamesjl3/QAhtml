#include "TPCSilDraw.h"

#include <sPhenixStyle.C>

#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>

#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TLatex.h>
#include <TColor.h>
#include <TLegend.h>

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

TPCSilDraw::TPCSilDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  histprefix = "h_TpcSiliconQA_"; 
  return;
}

TPCSilDraw::~TPCSilDraw()
{
  /* delete db; */
  return;
}

int TPCSilDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "POSITION")
  {
    iret += DrawPositionInfo();
    idraw++;
  }
  if (what == "ALL" || what == "CUTS")
  {
    iret += DrawCutHistograms();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int TPCSilDraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("TPCSil Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  if (num != 0)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.32, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.37, 0.52, 0.64, 0.97, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.69, 0.52, 0.96, 0.97, 0);
    Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.05, 0.02, 0.32, 0.47, 0);
    Pad[num][4] = new TPad((boost::format("mypad%d4") % num).str().c_str(), "hi", 0.37, 0.02, 0.64, 0.47, 0);
    Pad[num][5] = new TPad((boost::format("mypad%d5") % num).str().c_str(), "hello", 0.69, 0.02, 0.96, 0.47, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
    Pad[num][4]->Draw();
    Pad[num][5]->Draw();
  }
  else
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "no", 0.05, 0.25, 0.45, 0.75);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "no", 0.5, 0.25, 0.95, 0.75);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
  }

  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int TPCSilDraw::DrawPositionInfo()
{
  std::cout << "TPCSil DrawPositionInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1 *h_crossing = dynamic_cast <TH1 *> (cl->getHisto(histprefix + std::string("crossing"))); 
  TH1F *h_xDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("xDiff")));
  TH1F *h_yDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("yDiff")));
  TH1F *h_zDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("zDiff")));
  TH1F *h_phiDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("phiDiff")));
  TH1F *h_etaDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("etaDiff")));

  if (! gROOT->FindObject("crossing_info"))
  {
    MakeCanvas("crossing_info", 0);
  }
  TC[0]->Clear("D");
  Pad[0][0]->cd();
  if (h_crossing)
  {
    h_crossing->DrawCopy("HIST");
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized coordinates
  PrintRun.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream1;
  std::string runstring1;
  runnostream1 << Name() << "_matching Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  TC[0]->Update();
    
  if (! gROOT->FindObject("position_info"))
  {
    MakeCanvas("position_info", 1);
  }
  TC[1]->Clear("D");

  Pad[1][0]->cd();
  if (h_xDiff)
  {
    h_xDiff->GetYaxis()->SetTitle("Entries (No Cuts)");
    h_xDiff->GetYaxis()->SetTitleOffset(2);
    h_xDiff->DrawCopy("HIST");
    gPad->SetLeftMargin(0.2);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][1]->cd();
  if (h_yDiff)
  {
    h_yDiff->GetYaxis()->SetTitle("Entries (No Cuts)");
    h_yDiff->GetYaxis()->SetTitleOffset(2);
    h_yDiff->DrawCopy("HIST");
    gPad->SetLeftMargin(0.2);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][2]->cd();
  if (h_zDiff)
  {
    h_zDiff->GetYaxis()->SetTitle("Entries (No Cuts)");
    h_zDiff->GetYaxis()->SetTitleOffset(2);
    h_zDiff->GetXaxis()->SetNdivisions(5);
    h_zDiff->DrawCopy("HIST");
    gPad->SetLeftMargin(0.2);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][3]->cd();
  if (h_etaDiff)
  {
    h_etaDiff->GetYaxis()->SetTitle("Entries (No Cuts)");
    h_etaDiff->GetYaxis()->SetTitleOffset(2);
    h_etaDiff->GetXaxis()->SetNdivisions(5);
    h_etaDiff->DrawCopy("HIST");
    gPad->SetLeftMargin(0.2);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][4]->cd();
  if (h_phiDiff)
  {
    h_phiDiff->GetYaxis()->SetTitle("Entries (No Cuts)");
    h_phiDiff->GetYaxis()->SetTitleOffset(2);
    h_phiDiff->GetXaxis()->SetNdivisions(5);
    h_phiDiff->DrawCopy("HIST");
    gPad->SetLeftMargin(0.2);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  TText PrintRun2;
  PrintRun2.SetTextFont(62);
  PrintRun2.SetTextSize(0.04);
  PrintRun2.SetNDC();  // set to normalized coordinates
  PrintRun2.SetTextAlign(23); // center/top alignment
  std::ostringstream runnostream2;
  std::string runstring2;
  runnostream2 << Name() << "_No Cuts Run " << cl->RunNumber() << ", build " << cl->build();
  runstring2 = runnostream2.str();
  transparent[1]->cd();
  PrintRun2.DrawText(0.5, 1., runstring2.c_str());

  TC[1]->Update();
 
  std::cout << "DrawPositionInfo Ending" << std::endl;
  return 0;
}
 
int TPCSilDraw::DrawCutHistograms()
{
  std::cout << "TPCSil DrawCutHistograms() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  std::vector<std::string> cutNames = {"_xyCut", "_etaCut", "_phiCut", "North", "South", "NorthAllCuts", "SouthAllCuts"};

  int i = 2;
  for (const std::string& name : cutNames)
  {
    TH1F *h_xDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("xDiff") + name));
    TH1F *h_yDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("yDiff") + name));
    TH1F *h_zDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("zDiff") + name));
    TH1F *h_phiDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("phiDiff") + name));
    TH1F *h_etaDiff = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("etaDiff") + name));
 
    if (! gROOT->FindObject((std::string("position") + name).c_str()))
    {
      MakeCanvas(std::string("position") + name, i);
    }
    TC[i]->Clear("D");

    Pad[i][0]->cd();
    if (h_xDiff)
    {
      std::string title = h_xDiff->GetTitle();
      h_xDiff->GetYaxis()->SetTitle(title.c_str());
      h_xDiff->GetYaxis()->SetTitleOffset(2);
      h_xDiff->DrawCopy("HIST");
      gPad->SetLeftMargin(0.2);
    }
    else
    {
      // histogram is missing
      return -1;
    }
    Pad[i][1]->cd();
    if (h_yDiff)
    {
      std::string title = h_yDiff->GetTitle();
      h_yDiff->GetYaxis()->SetTitle(title.c_str());
      h_yDiff->GetYaxis()->SetTitleOffset(2);
      h_yDiff->DrawCopy("HIST");
      gPad->SetLeftMargin(0.2);
    }
    else
    {
      // histogram is missing
      return -1;
    }
    Pad[i][2]->cd();
    if (h_zDiff)
    {
      std::string title = h_zDiff->GetTitle();
      h_zDiff->GetYaxis()->SetTitle(title.c_str());
      h_zDiff->GetYaxis()->SetTitleOffset(2);
      h_zDiff->GetXaxis()->SetNdivisions(5);
      h_zDiff->DrawCopy("HIST");
      gPad->SetLeftMargin(0.2);
    }
    else
    {
      // histogram is missing
      return -1;
    }
    Pad[i][3]->cd();
    if (h_etaDiff)
    {
      std::string title = h_etaDiff->GetTitle();
      h_etaDiff->GetYaxis()->SetTitle(title.c_str());
      h_etaDiff->GetYaxis()->SetTitleOffset(2);
      h_etaDiff->GetXaxis()->SetNdivisions(5);
      h_etaDiff->DrawCopy("HIST");
      gPad->SetLeftMargin(0.2);
    }
    else
    {
      // histogram is missing
      return -1;
    }
    Pad[i][4]->cd();
    if (h_phiDiff)
    {
      std::string title = h_phiDiff->GetTitle();
      h_phiDiff->GetYaxis()->SetTitle(title.c_str());
      h_phiDiff->GetYaxis()->SetTitleOffset(2);
      h_phiDiff->GetXaxis()->SetNdivisions(5);
      h_phiDiff->DrawCopy("HIST");
      gPad->SetLeftMargin(0.2);
    }
    else
    {
      // histogram is missing
      return -1;
    }
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream;
    std::string runstring;
    runnostream << Name() << "_cuts Run " << cl->RunNumber() << ", build " << cl->build();
    runstring = runnostream.str();
    transparent[i]->cd();
    PrintRun.DrawText(0.5, 1., runstring.c_str());

    TC[i]->Update();
    i++;
  }
 
  std::cout << "DrawCutHistograms Ending" << std::endl;
  return 0;
}

int TPCSilDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the 1st canvas png file to the menu and produces the png file.
  if (what == "ALL" || what == "POSITION")
  {
    pngfile = cl->htmlRegisterPage(*this, "crossing_info", "1", "png");
    cl->CanvasToPng(TC[0], pngfile);
    pngfile = cl->htmlRegisterPage(*this, "position_info", "2", "png");
    cl->CanvasToPng(TC[1], pngfile);
  }
  if (what == "ALL" || what == "CUTS")
  {
    std::vector<std::string> cutNames = {"_xyCut", "_etaCut", "_phiCut", "North", "South", "NorthAllCuts", "SouthAllCuts"};
    int i = 2;
    for (const std::string& name : cutNames)
    { 
      pngfile = cl->htmlRegisterPage(*this, (boost::format("position%s") % name).str(), (boost::format("%i") % (i + 1)).str(), "png");
      cl->CanvasToPng(TC[i], pngfile);
      i++;
    }
  }

  return 0;
}

int TPCSilDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
