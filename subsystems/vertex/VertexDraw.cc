#include "VertexDraw.h"

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

#include <boost/format.hpp>

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

VertexDraw::VertexDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  histprefix = "h_VertexQA_";
  return;
}

VertexDraw::~VertexDraw()
{
  /* delete db; */
  return;
}

int VertexDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL")
  {
    iret += DrawVertexInfo();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int VertexDraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("Vertex Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  if (num != 1)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.45, 0.97, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.52, 0.95, 0.97, 0);
    Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.05, 0.02, 0.45, 0.47, 0);
    Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.5, 0.02, 0.95, 0.47, 0);
    
    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
  }
  
  else if (num == 1)
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

int VertexDraw::DrawVertexInfo()
{
  std::cout << "Vertex DrawVertexInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1 *h_vx = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vx")));
  TH1 *h_vy = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vy")));
  TH1 *h_vz = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vz")));
  TH1 *h_vt = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vt")));
  TH1 *h_nvertex = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("nrecovertices")));
  TH1 *h_vcrossing = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vertexcrossing")));
  TH1 *h_vchi2 = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vertexchi2")));
  TH1 *h_vndof = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("vertexndof")));
  TH1 *h_ntrackpervertex = dynamic_cast<TH1 *>(cl->getHisto(histprefix + std::string("ntrackspervertex")));

  if (! gROOT->FindObject("vertex_pos"))
  {
    MakeCanvas("vertex_pos", 0);
  }
  TC[0]->Clear("D");
  Pad[0][0]->cd();
  if (h_vx)
  {
    h_vx->SetTitle("Vertex x Distribution");
    h_vx->SetXTitle("vx [cm]");
    h_vx->SetYTitle("Counts");
    h_vx->GetXaxis()->SetNdivisions(100);
    h_vx->GetXaxis()->SetRangeUser(-0.1,0.1);
    h_vx->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[0][1]->cd();
  if (h_vy)
  {
    h_vy->SetTitle("Vertex y Distribution");
    h_vy->SetXTitle("vy [cm]");
    h_vy->SetYTitle("Counts");
    h_vy->GetXaxis()->SetNdivisions(100);
    h_vy->GetXaxis()->SetRangeUser(-0.1,0.1);
    h_vy->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[0][2]->cd();
  if (h_vz)
  {
    h_vz->SetTitle("Vertex z Distribution");
    h_vz->SetXTitle("vz [cm]");
    h_vz->SetYTitle("Counts");
    h_vz->GetXaxis()->SetNdivisions(100);
    h_vz->GetXaxis()->SetRangeUser(-15,15);
    h_vz->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[0][3]->cd();
  if (h_vt)
  {
    h_vt->SetTitle("Vertex t Distribution");
    h_vt->SetXTitle("vt [ns]");
    h_vt->SetYTitle("Counts");
    h_vt->GetXaxis()->SetNdivisions(100);
    h_vt->GetXaxis()->SetRangeUser(-1000,20000);
    h_vt->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  if (! gROOT->FindObject("vertex_fit"))
  {
    MakeCanvas("vertex_fit", 1);
  }
  TC[1]->Clear("D");
  Pad[1][0]->cd();
  if (h_vchi2)
  {
    h_vchi2->SetTitle("Vertex #chi^{2}");
    h_vchi2->SetXTitle("#chi^{2}");
    h_vchi2->SetYTitle("Counts");
    h_vchi2->GetXaxis()->SetNdivisions(100);
    h_vchi2->GetXaxis()->SetRangeUser(0,10000);
    h_vchi2->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1][1]->cd();
  if (h_vndof)
  {
    h_vndof->SetTitle("Vertex nDoF");
    h_vndof->SetXTitle("nDoF");
    h_vndof->SetYTitle("Counts");
    h_vndof->GetXaxis()->SetNdivisions(50);
    h_vndof->GetXaxis()->SetRangeUser(0,50);
    h_vndof->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
   
  if (! gROOT->FindObject("vertex_counts"))
  {
    MakeCanvas("vertex_counts", 2);
  }
  TC[2]->Clear("D");
  Pad[2][0]->cd();
  if (h_nvertex)
  {
    h_nvertex->SetTitle("Number of Reco Vertices Per Event");
    h_nvertex->SetXTitle("nRecoVertices");
    h_nvertex->SetYTitle("Counts");
    h_nvertex->GetXaxis()->SetNdivisions(200);
    h_nvertex->GetXaxis()->SetRangeUser(0,200);
    h_nvertex->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[2][1]->cd();
  if (h_vcrossing)
  {
    h_vcrossing->SetTitle("Vertex Beam Bunch Crossing");
    h_vcrossing->SetXTitle("Bunch Crossing");
    h_vcrossing->SetYTitle("Counts");
    h_vcrossing->GetXaxis()->SetNdivisions(100);
    h_vcrossing->GetXaxis()->SetRangeUser(-100,300);
    h_vcrossing->DrawCopy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[2][2]->cd();
  if (h_ntrackpervertex)
  {
    h_ntrackpervertex->SetTitle("Number of Tracks per Vertex");
    h_ntrackpervertex->SetXTitle("nTracks/Vertex");
    h_ntrackpervertex->SetYTitle("Counts");
    h_ntrackpervertex->GetXaxis()->SetNdivisions(20);
    h_ntrackpervertex->GetXaxis()->SetRangeUser(0,20);
    h_ntrackpervertex->DrawCopy();
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
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream1, runnostream2, runnostream3;
  std::string runstring1, runstring2, runstring3;
  runnostream1 << Name() << "_vertex_1 Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  runnostream2 << Name() << "_vertex_2 Run " << cl->RunNumber() << ", build " << cl->build();
  runstring2 = runnostream2.str();
  runnostream3 << Name() << "_vertex_3 Run " << cl->RunNumber() << ", build " << cl->build();
  runstring3 = runnostream3.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring2.c_str());
  transparent[2]->cd();
  PrintRun.DrawText(0.5, 1., runstring3.c_str());

  TC[0]->Update();
  TC[1]->Update();  
  TC[2]->Update();  

  std::cout << "DrawVertexInfo Ending" << std::endl;
  return 0;
}

int VertexDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the canvas png file to the menu and produces the png file.
  if (what == "ALL")
  {
    pngfile = cl->htmlRegisterPage(*this, "vertex_pos", "1", "png");
    cl->CanvasToPng(TC[0], pngfile);
    pngfile = cl->htmlRegisterPage(*this, "vertex_fit", "2", "png");
    cl->CanvasToPng(TC[1], pngfile);
    pngfile = cl->htmlRegisterPage(*this, "vertex_", "3", "png");
    cl->CanvasToPng(TC[2], pngfile);
  }
  return 0;
}

int VertexDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
