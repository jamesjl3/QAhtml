#include "MicromegasDraw.h"

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

MicromegasDraw::MicromegasDraw(const std::string &name)
  : QADraw(name)
{
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  return;
}

MicromegasDraw::~MicromegasDraw()
{
  /* delete db; */
  return;
}

int MicromegasDraw::Draw(const std::string &what)
{
  /* SetsPhenixStyle(); */
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "TILES")
  {
    iret += DrawTileInfo();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int MicromegasDraw::MakeCanvas(const std::string &name, int num)
{
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("Micromegas Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2) , (int) (ysize / 1.2));
  gSystem->ProcessEvents();

  Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.52, 0.45, 0.97, 0);
  Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.52, 0.95, 0.97, 0);
  Pad[num][2] = new TPad((boost::format("mypad%d2") % num).str().c_str(), "name", 0.05, 0.02, 0.45, 0.47, 0);
  Pad[num][3] = new TPad((boost::format("mypad%d3") % num).str().c_str(), "here", 0.5, 0.02, 0.95, 0.47, 0);
   
  Pad[num][0]->Draw();
  Pad[num][1]->Draw();
  Pad[num][2]->Draw();
  Pad[num][3]->Draw();
  
  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int MicromegasDraw::DrawTileInfo()
{
  std::cout << "Micromegas DrawTileInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  // Loop over all tiles, 4 at a time
  for (int quad = 0; quad < 4; quad ++)
  {
    std::vector<TH2F*> h_clus;
    for (int i = 0; i < 4; i++)
    {
      TH2F *h_clus_tile;
      if (quad < 2) h_clus_tile = dynamic_cast <TH2F *> (cl->getHisto((boost::format("h_MicromegasClusterQA_ncluspertile0_%i") % (quad*4 + i)).str()));
      else h_clus_tile = dynamic_cast <TH2F *> (cl->getHisto((boost::format("h_MicromegasClusterQA_ncluspertile1_%i") % ((quad - 2)*4 + i)).str()));
      h_clus.push_back(h_clus_tile);
    }
    
    // make a canvas for the quad grouping
    if (! gROOT->FindObject((boost::format("clus_tile_%i") % quad).str().c_str()))
    {
      MakeCanvas((boost::format("clus_tile_%i") % quad).str(), quad);
    }
    TC[quad]->Clear("D");
    for (int i = 0; i < 4; i++)
    {
      Pad[quad][i]->cd();
      if (h_clus[i])
      {
        if (quad < 2) h_clus[i]->SetTitle((boost::format("TPOT Layer 0 Tile %i Clusters") % (quad*4 + i)).str().c_str());
        else h_clus[i]->SetTitle((boost::format("TPOT Layer 1 Tile %i Clusters") % ((quad - 2)*4 + i)).str().c_str());
        h_clus[i]->SetXTitle("Local z [cm]");
        h_clus[i]->SetYTitle("Local r#phi [cm]");
        h_clus[i]->GetXaxis()->SetNdivisions(2000);
        h_clus[i]->GetXaxis()->SetRangeUser(-30,30);
        h_clus[i]->GetYaxis()->SetNdivisions(2000);
        h_clus[i]->GetYaxis()->SetRangeUser(-20,20);
        h_clus[i]->DrawCopy();
        gPad->SetRightMargin(0.15);
      }
      else
      {
        // histogram is missing
        return -1;
      }
    }
    
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();  // set to normalized coordinates
    PrintRun.SetTextAlign(23); // center/top alignment
    std::ostringstream runnostream1;
    std::string runstring1;
    runnostream1 << Name() << "_tpot Clusters Run " << cl->RunNumber() << ", build " << cl->build();
    runstring1 = runnostream1.str();
    transparent[quad]->cd();
    PrintRun.DrawText(0.5, 1., runstring1.c_str());

    TC[quad]->Update();
  } 
  std::cout << "DrawTileInfo Ending" << std::endl;
  return 0;
}

int MicromegasDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret) // on error no html output please
  {
    return iret;
  }

  QADrawClient *cl = QADrawClient::instance();
  std::string pngfile;

  // Register the canvas png file to the menu and produces the png file.
  if (what == "ALL" || what == "TILES")
  {
    for (int quad = 0; quad < 4; quad++)
    {
      if (quad < 2) pngfile = cl->htmlRegisterPage(*this, (boost::format("TPOT_Clus_0_%i-%i") % (quad*4) % (quad*4 + 3)).str(), (boost::format("%i") % (quad + 1)).str(), "png");
      else pngfile = cl->htmlRegisterPage(*this, (boost::format("TPOT_Clus_1_%i-%i") % ((quad - 2)*4) % ((quad - 2)*4 + 3)).str(), (boost::format("%i") % (quad + 1)).str(), "png");
      cl->CanvasToPng(TC[quad], pngfile);
    }
  }
  return 0;
}

int MicromegasDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
