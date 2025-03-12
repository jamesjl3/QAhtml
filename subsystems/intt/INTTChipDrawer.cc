#include "INTTChipDrawer.h"

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

INTTChipDrawer::INTTChipDrawer(std::string const& name)
  : SingleCanvasDrawer(name)
{
  histprefix = "h_InttClusterQA_"; 
}

INTTChipDrawer::~INTTChipDrawer()
{
  // ...
}

int INTTChipDrawer::MakeCanvas()
{
  if(SingleCanvasDrawer::MakeCanvas())
  {
    return 0;
  }
  m_canvas->SetTitle("INTT Plots 0");
  
  Pad[0] = new TPad("mypad_0_0", "put",  0.05, 0.52, 0.45, 0.97, 0);
  Pad[1] = new TPad("mypad_0_1", "a",    0.5,  0.52, 0.95, 0.97, 0);
  Pad[2] = new TPad("mypad_0_2", "name", 0.05, 0.02, 0.45, 0.47, 0);
  Pad[3] = new TPad("mypad_0_3", "here", 0.5,  0.02, 0.95, 0.47, 0);
  
  Pad[0]->Draw();
  Pad[1]->Draw();
  Pad[2]->Draw();
  Pad[3]->Draw();
  
  // this one is used to plot the run number on the canvas
  transparent = new TPad("transparent_0", "this does not show", 0, 0, 1, 1);
  transparent->SetFillStyle(4000);
  transparent->Draw();

  return 0;
}

int INTTChipDrawer::DrawCanvas()
{
  std::cout << "INTT DrawChipInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1F *h_occupancy = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("sensorOccupancy")));
  TH1F *h_clusSize = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterSize")));

  MakeCanvas();
  m_canvas->Clear("D");

  Pad[0]->cd();
  if (h_occupancy)
  {
    h_occupancy->SetTitle("INTT Chip Occupancy");
    h_occupancy->SetXTitle("Chip Occupancy [%]");
    h_occupancy->SetYTitle("Normalized Entries");
    h_occupancy->Scale(1./h_occupancy->Integral());
    h_occupancy->DrawCopy();
    gPad->SetLogy();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1]->cd();
  if (h_occupancy)
  {
    h_clusSize->SetTitle("INTT Cluster Size");
    h_clusSize->SetXTitle("Cluster Size");
    h_clusSize->SetYTitle("Normalized Entries");
    h_clusSize->Scale(1./h_clusSize->Integral());
    h_clusSize->DrawCopy();
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
  // runnostream1 << Name() << "_intt Info Run " << cl->RunNumber() << ", build " << cl->build();
  runnostream1 << "INTTQA_intt Info Run " << cl->RunNumber() << ", build " << cl->build();
  runstring1 = runnostream1.str();
  transparent->cd();
  PrintRun.DrawText(0.5, 1., runstring1.c_str());

  m_canvas->Update();
 
  std::cout << "DrawChipInfo Ending" << std::endl;
  return 0;
}
