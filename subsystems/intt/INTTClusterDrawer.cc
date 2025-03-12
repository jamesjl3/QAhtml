#include "INTTClusterDrawer.h"

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

INTTClusterDrawer::INTTClusterDrawer(std::string const& name)
  : SingleCanvasDrawer(name)
{
  histprefix = "h_InttClusterQA_"; 
}

INTTClusterDrawer::~INTTClusterDrawer()
{
  // ...
}

int INTTClusterDrawer::MakeCanvas()
{
  if(SingleCanvasDrawer::MakeCanvas())
  {
    return 0;
  }
  m_canvas->SetTitle("INTT Plots 1");
  
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

int INTTClusterDrawer::DrawCanvas()
{
  std::cout << "INTT DrawClusterInfo() Beginning" << std::endl;
  QADrawClient *cl = QADrawClient::instance();

  TH1F *h_clusPhi_incl = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_incl")));
  TH1F *h_clusPhi_l34 = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_l34")));
  TH1F *h_clusPhi_l56 = dynamic_cast <TH1F *> (cl->getHisto(histprefix + std::string("clusterPhi_l56")));
  TH2F *h_clusZ_clusPhi_l34 = dynamic_cast <TH2F *> (cl->getHisto(histprefix + std::string("clusterZ_clusPhi_l34")));
  TH2F *h_clusZ_clusPhi_l56 = dynamic_cast <TH2F *> (cl->getHisto(histprefix + std::string("clusterZ_clusPhi_l56")));

  MakeCanvas();
  m_canvas->Clear("D");
  
  Pad[0]->cd();
  if (h_clusPhi_incl && h_clusPhi_l34 && h_clusPhi_l56)
  {
    h_clusPhi_incl->SetTitle("INTT Cluster Phi");
    h_clusPhi_incl->SetXTitle("Cluster #phi wrt origin [rad]");
    h_clusPhi_incl->SetYTitle("Entries");
    h_clusPhi_incl->SetLineColor(kBlack);
    h_clusPhi_incl->SetMarkerSize(0.5);
    h_clusPhi_incl->DrawCopy();
    h_clusPhi_l34->SetLineColor(kRed);
    h_clusPhi_l34->SetMarkerSize(0.5);
    h_clusPhi_l34->SetMarkerColor(kRed);
    h_clusPhi_l34->DrawCopy("same");
    h_clusPhi_l56->SetLineColor(kBlue);
    h_clusPhi_l56->SetMarkerSize(0.5);
    h_clusPhi_l56->SetMarkerColor(kBlue);
    h_clusPhi_l56->DrawCopy("same");
    auto legend = new TLegend(0.45, 0.75, 0.7, 0.9);
    legend->AddEntry(h_clusPhi_incl, "Inclusive", "pl");
    legend->AddEntry(h_clusPhi_l34, "Inner layer (3+4)", "pl");
    legend->AddEntry(h_clusPhi_l56, "Outer layer (5+6)", "pl");
    legend->SetFillStyle(0);
    legend->Draw();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[1]->cd();
  if (h_clusZ_clusPhi_l34)
  {
    h_clusZ_clusPhi_l34->SetTitle("INTT Cluster Z vs #phi Inner Layer");
    h_clusZ_clusPhi_l34->SetXTitle("Inner cluster Z [cm]");
    h_clusZ_clusPhi_l34->SetYTitle("Inner cluster #phi wrt origin [rad]");
    h_clusZ_clusPhi_l34->SetMarkerSize(0.5);
    h_clusZ_clusPhi_l34->DrawCopy("colz");
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();
  }
  else
  {
    // histogram is missing
    return -1;
  }
  Pad[2]->cd();
  if (h_clusZ_clusPhi_l56)
  {
    h_clusZ_clusPhi_l56->SetTitle("INTT Cluster Z vs #phi Outer Layer");
    h_clusZ_clusPhi_l56->SetXTitle("Outer cluster Z [cm]");
    h_clusZ_clusPhi_l56->SetYTitle("Outer cluster #phi wrt origin [rad]");
    h_clusZ_clusPhi_l56->SetMarkerSize(0.5);
    h_clusZ_clusPhi_l56->DrawCopy("colz");
    gPad->SetRightMargin(0.15);
    gPad->SetLogz();
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
 
  std::cout << "DrawClusterInfo Ending" << std::endl;
  return 0;

}
