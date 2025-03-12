#include "JetDraw.h"
//#include "JetGoodRunChecker.h"
#include <sPhenixStyle.C>
#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TString.h>
#include <TPad.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TGaxis.h>
#include <boost/format.hpp>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

JetDraw::JetDraw(const std::string &name)
: cl(QADrawClient::instance())
{
  if (name == "JetQA") {
    // Handle specific condition
  }
  memset(TC, 0, sizeof(TC));
  memset(transparent, 0, sizeof(transparent));
  memset(Pad, 0, sizeof(Pad));
  DBVarInit();
  histprefix = "h_constituentsinjets";
  histprefix1 = "h_eventwiserho";
  histprefix2 = "h_jetkinematiccheck";
  histprefix3 = "h_jetseedcount";
  return;
}

JetDraw::~JetDraw()
{
  /* delete db; */
  return;
}

int JetDraw::MakeHtml(const std::string &what) {
  if (what == "ALL") {
    // Handle the "ALL" case here if necessary
  } 
  int iret = 0;
  const int drawError = Draw(what);  // Call to Draw
  if (drawError) {
    return drawError;  // Return if there is an error in Draw
  }
    
  int resolutionDrawError = 0;
  for (uint32_t trigToDraw : m_vecTrigToDraw) {  // Loop over triggers
    for (auto& resAndName : m_mapResToName) {  // Loop over resolution and name pairs
      std::string pngFile = cl->htmlRegisterPage(
						 *this,
						 m_mapTrigToName[trigToDraw] + "/" + std::to_string(resAndName.first),
						 m_mapTrigToName[trigToDraw] + "_" + resAndName.second,
						 "png");
      // Convert canvas to PNG
      resolutionDrawError = cl->CanvasToPng(m_vecCanvas[trigToDraw][resAndName.first], pngFile);
      if (resolutionDrawError) {
	return resolutionDrawError;  // Handle errors appropriately
      }
    }
  }
  return iret;  // Return after processing all triggers and resolutions
}

int JetDraw::Draw(const std::string &what)
{
   if (what == "ALL") { /*Was not sure how to implement "what" so I have this here as a place holder*/
   }
   // 1st make sure there's enough space for each trigger
   m_vecCanvas.resize( m_vecTrigToDraw.size() );
   // now loop over triggers
   int iret = 0;
   // loop over indices of triggers we want to dsub
   for (uint32_t trigToDraw : m_vecTrigToDraw) { // loop over jet
     if  (what == "ALL" || what == "RHO") {
       iret += DrawRho(trigToDraw);
       ++idraw;
     }
     for (uint32_t resToDraw : m_vecResToDraw) {
       if (what == "ALL" || what == "CONTSTITUENTS") {
	 iret += DrawConstituents(trigToDraw, static_cast<JetRes>(resToDraw));
	 ++idraw;
       }
       if (what == "ALL" || what == "KINEMATICCHECK") {                                                                                                                                        
         iret += DrawJetKinematics(trigToDraw, static_cast<JetRes>(resToDraw));
         ++idraw;
       }
       if (what == "ALL" || what == "SEEDCOUNT") {                                                                                                                                        
         iret += DrawJetSeed(trigToDraw, static_cast<JetRes>(resToDraw));
         ++idraw;
       }
     }
   }   
   /* SetsPhenixStyle(); */
   gStyle->SetTitleSize(gStyle->GetTitleSize("X")*2.0, "X");
   gStyle->SetTitleSize(gStyle->GetTitleSize("Y")*2.0, "Y");
   gStyle->SetPadLeftMargin(0.15);
   gStyle->SetPadBottomMargin(0.15);
   gStyle->SetTitleOffset(0.85, "XY");
   gStyle->SetLabelSize(gStyle->GetLabelSize("X")*1.5, "X");
   gStyle->SetLabelSize(gStyle->GetLabelSize("Y")*1.5, "Y");
   gStyle->SetLabelSize(gStyle->GetLabelSize("Z")*1.5, "Z");
   TGaxis::SetMaxDigits(4);
   gStyle->SetPadTickX(1);
   gStyle->SetPadTickY(1);
   gStyle->SetOptStat(10);
   gROOT->ForceStyle();
   
   if (!idraw)
     {
       std::cout << " Unimplemented Drawing option: " << what << std::endl;
       iret = -1;
     }
     return iret;
 }
 
int JetDraw::MakeCanvas(const std::string &name, int num)
{
  //  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  // xpos (-1) negative: do not draw menu bar
  TC[num] = new TCanvas(name.c_str(), (boost::format("Jet Plots %d") % num).str().c_str(), -1, 0, (int) (xsize / 1.2), (int) (ysize / 1.2));
  TC[num]->UseCurrentStyle();
  gSystem->ProcessEvents();

  if (num==1)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "put", 0.05, 0.25, 0.45, 0.75, 0);
    Pad[num][1] = new TPad((boost::format("mypad%d1") % num).str().c_str(), "a", 0.5, 0.25, 0.95, 0.75, 0);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
  }
  else if (num==7)
  {
    Pad[num][0] = new TPad((boost::format("mypad%d0") % num).str().c_str(), "", 0.05, 0.05, 0.95, 0.95, 0);
    Pad[num][0]->Draw();
  }
  else if (num==5)
  {
    TC[num]->Divide(3, 2, 0.025, 0.025);
    Pad[num][0] = (TPad*)TC[num]->GetPad(1);
    Pad[num][1] = (TPad*)TC[num]->GetPad(2);
    Pad[num][2] = (TPad*)TC[num]->GetPad(3);
    Pad[num][3] = (TPad*)TC[num]->GetPad(4);
    Pad[num][4] = (TPad*)TC[num]->GetPad(5);
    Pad[num][5] = (TPad*)TC[num]->GetPad(6);

    Pad[num][0]->Draw();
    Pad[num][1]->Draw();
    Pad[num][2]->Draw();
    Pad[num][3]->Draw();
    Pad[num][4]->Draw();
    Pad[num][5]->Draw();
  }
  else
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

  // this one is used to plot the run number on the canvas
  transparent[num] = new TPad((boost::format("transparent%d") % num).str().c_str(), "this does not show", 0, 0, 1, 1);
  transparent[num]->SetFillStyle(4000);
  transparent[num]->Draw();

  return 0;
}

int JetDraw::DrawConstituents(const uint32_t trigToDraw, const JetRes resToDraw)
{
  // Use resToDraw to select histograms or adjust drawing logic
  switch (resToDraw) {
  case R02:
    std::cout << "Resolution R02" << std::endl;
    break;
  case R03:
    std::cout << "Resolution R03" << std::endl;
    break;
  case R04:
    std::cout << "Resolution R04" << std::endl;
    break;
  case R05:
    std::cout << "Resolution R05" << std::endl;
    break;
  default:
    std::cerr << "Unknown resolution" << std::endl;
    break;
  }

  //  QADrawClient *cl = QADrawClient::instance();
  // TCanvas* canvas = new TCanvas( /* etc */ );
  /* do drawing */
  TH1D *constituents_ncsts_cemc = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_ncsts_cemc")));
  TH1D *constituents_ncsts_ihcal = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_ncsts_ihcal")));
  TH1D *constituents_ncsts_ohcal = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_ncsts_ohcal")));
  TH1D *constituents_ncsts_total = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_ncsts_total")));
  TH2D *constituents_ncstsvscalolayer = dynamic_cast<TH2D *>(cl->getHisto(histprefix + std::string("_ncstsvscalolayer")));
  TH1D *constituents_efracjet_cemc = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_efracjet_cemc")));
  TH1D *constituents_efracjet_ihcal = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_efracjet_ihcal")));
  TH1D *constituents_efracjet_ohcal = dynamic_cast<TH1D *>(cl->getHisto(histprefix + std::string("_efracjet_ohcal")));
  TH2D *constituents_efracjetvscalolayer = dynamic_cast<TH2D *>(cl->getHisto(histprefix + std::string("_efracjetcscalolayer")));
 
  // Ensure m_vecCanvas is a 2D vector if needed
  if (trigToDraw >= m_vecCanvas.size()) {
    m_vecCanvas.resize(trigToDraw + 1); // Resize if necessary
  }

  // Initialize canvas if not already done
  if (m_vecCanvas[trigToDraw].empty()) {
    // Initialize with appropriate TCanvas* objects if needed
    // Example: m_vecCanvas[trigToDraw].push_back(new TCanvas("canvas_name", "Title", 800, 600));
  }

  // Assuming you want to push TCanvas* objects into the vector
  // Example canvas creation and pushing
  TCanvas* canvas = new TCanvas("canvas_name", "Title", 800, 600);
  m_vecCanvas[trigToDraw].push_back(canvas);
 
 // canvas 1                                                                                                                 
  if (!gROOT->FindObject("jet1"))                 
    {                                                                                    
      MakeCanvas("jet1", 0);
    }                                                                                                                                          
    // TC[0]->Clear("D"); //                                                                      
    Pad[0][0]->cd();                                                                                             
    if (constituents_ncsts_cemc)                                                                                                                  
      {                                                                                                                                           
	constituents_ncsts_cemc->SetTitle("Jet N Constituents in CEMC");                                                                            
	constituents_ncsts_cemc->SetXTitle("N Constituents");                                                                        
	constituents_ncsts_cemc->SetYTitle("Counts");                    
	constituents_ncsts_cemc->GetXaxis()->SetNdivisions(505);                                                                                 
	constituents_ncsts_cemc->GetXaxis()->SetRangeUser(-1, 15);                                                   
	constituents_ncsts_cemc->DrawCopy("HIST"); //1D histogram                                                                                         
	gPad->UseCurrentStyle();                                                                                             
	gPad->SetLogy();                                                   
	//gPad->SetLogz() //No z axis   
	gPad->SetRightMargin(0.15);                                                                                         
      }                                                                             
    else                                                                                                                     
      {                                                                                                                                    
	// histogram is missing                                                                                                             
	return -1;                                                                               
      }                
    Pad[0][1]->cd();                                                                                                                
    if (constituents_ncsts_ihcal)                                                                                                          
      {                                                                                                                                 
	constituents_ncsts_ihcal->SetTitle("Jet N Constituents in IHCal");                                                       
	constituents_ncsts_ihcal->SetXTitle("N Constituents");                                                                           
	constituents_ncsts_ihcal->SetYTitle("Counts");                                   
	constituents_ncsts_ihcal->GetXaxis()->SetNdivisions(505);                                                         
	constituents_ncsts_ihcal->DrawCopy("COLZ"); //I think it might be HIST because it's 1D                        
	gPad->UseCurrentStyle();                                                                                                       
	gPad->SetRightMargin(0.15);                                                            
      }
                                                                                                                   
    Pad[0][2]->cd();                                                                                                                     
    if (constituents_ncsts_ohcal)                                                                                                            
      {                                                                                                                                             
	constituents_ncsts_ohcal->SetTitle("Jet N Constituents in OHCal");                                     
	constituents_ncsts_ohcal->SetXTitle("N Constituents");                                                                                               
	constituents_ncsts_ohcal->SetYTitle("Counts");                                                                                               
	constituents_ncsts_ohcal->GetXaxis()->SetNdivisions(505);                                                                  
	constituents_ncsts_ohcal->DrawCopy("COLZ"); //2D Histogram                             
	gPad->UseCurrentStyle();                                                                                                              
	gPad->SetRightMargin(0.15);                                                                                         
      }                                                                                                                                      
    Pad[0][3]->cd();                                                                                                                  
    if (constituents_ncsts_total)                                                                                                  
      {                                                                                                          
	constituents_ncsts_total->SetTitle("Jet N Constituents");                                                                          
	constituents_ncsts_total->SetXTitle("N Constituents");
	constituents_ncsts_total->GetXaxis()->SetNdivisions(505); //                                                                            
	constituents_ncsts_total->SetYTitle("Counts");                                                                      
	constituents_ncsts_total->DrawCopy("COLZ"); //No clue, this is a TProfile                                                           
	gPad->UseCurrentStyle();                                                                            
      }  
    // canvas 2                                                                                                                                  
    if (!gROOT->FindObject("jet2"))                                                                                                           
      {                                                                                                
	MakeCanvas("jet2", 1);                                                                                                                  
      }                                                                                                                                     
    // TC[1]->Clear("D"); //                                                                                                                  
    Pad[1][0]->cd();                                                                                                                       
    if (constituents_ncstsvscalolayer)                                                                                
      {                                                                                                                                 
	constituents_ncstsvscalolayer->SetTitle("Jet N Constituents vs Calo Layer");                                                                                  
	constituents_ncstsvscalolayer->SetXTitle("N Constituents");                                                                              
	constituents_ncstsvscalolayer->SetYTitle("Calo Layer");
	constituents_ncstsvscalolayer->SetZTitle("Counts");
	constituents_ncstsvscalolayer->DrawCopy("COLZ"); //2D histogram                                                               
	gStyle->SetPalette(4, kBird);
        gPad->UseCurrentStyle();
        gPad->Update();
        gPad->SetRightMargin(0.15);                                                                                                       
      }                                                                                                                                   
    Pad[1][1]->cd();                                                                                                                              
    if (constituents_efracjetvscalolayer)                                                                                                        
      {                                                                                                                                             
	constituents_efracjetvscalolayer->SetTitle("Jet E Fraction vs Calo Layer");                                                                 
	constituents_efracjetvscalolayer->SetXTitle("Jet E Fraction");                                                                     
	constituents_efracjetvscalolayer->SetYTitle("Calo Layer");
	constituents_efracjetvscalolayer->SetZTitle("Counts");                                                                                       
	constituents_efracjetvscalolayer->DrawCopy("COLZ"); //2D histogram                                                                                   
	gStyle->SetPalette(4, kBird);
        gPad->UseCurrentStyle();
        gPad->Update();
        gPad->SetRightMargin(0.15);
      }
    // canvas 3                                    
    if (!gROOT->FindObject("jet3"))
      {
	MakeCanvas("jet3", 6);
      }
    Pad[6][0]->cd();
    if (constituents_efracjet_cemc)
      {
	constituents_efracjet_cemc->SetTitle("Jet E Fraction in CEMC");
	constituents_efracjet_cemc->SetXTitle("Jet E fraction");
	constituents_efracjet_cemc->SetYTitle("Counts");
	constituents_efracjet_cemc->DrawCopy("COLZ");
	gStyle->SetPalette(4, kBird);
	gPad->UseCurrentStyle();
	gPad->Update();
	gPad->SetRightMargin(0.15);
      }
    Pad[6][1]->cd();
    if (constituents_efracjet_ihcal)
      {
	constituents_efracjet_ihcal->SetTitle("Jet Fraction in IHCal");
	constituents_efracjet_ihcal->SetXTitle("Jet E Fraction");
	constituents_efracjet_ihcal->SetYTitle("Counts");
	constituents_efracjet_ihcal->DrawCopy("COLZ");
	gStyle->SetPalette(4, kBird);
	gPad->UseCurrentStyle();
	gPad->Update();
	gPad->SetRightMargin(0.15);
      }
    Pad[6][2]->cd();
    if (constituents_efracjet_ohcal)
      {
	constituents_efracjet_ohcal->SetTitle("Jet Fraction in OHCal");
	constituents_efracjet_ohcal->SetXTitle("Jet E Fraction");
	constituents_efracjet_ohcal->SetYTitle("Counts");
	constituents_efracjet_ohcal->DrawCopy("COLZ");
	gStyle->SetPalette(4, kBird);
	gPad->UseCurrentStyle();
	gPad->Update();
	gPad->SetRightMargin(0.15);
      }
    TText PrintRun;
    PrintRun.SetTextFont(62);
    PrintRun.SetTextSize(0.04);
    PrintRun.SetNDC();          // set to normalized coordinates                                                                                                   
    PrintRun.SetTextAlign(23);  // center/top alignment                                                                              
                          
    // Generate run strings
    std::ostringstream runnostream[6];
    std::string runstring[6];
    runnostream[0] << Name() << "_constituentsinjets_inclusive Run " << cl->RunNumber();
    runstring[0] = runnostream[0].str();
    runnostream[1] << Name() << "_constituentsinjets_mbdns1 Run " << cl->RunNumber();
    runstring[1] = runnostream[1].str();
    runnostream[2] << Name() << "_constituentsinjets_mbdnsjet1 Run " << cl->RunNumber();
    runstring[2] = runnostream[2].str();
    runnostream[3] << Name() << "_constituentsinjets_mbdnsjet2 Run " << cl->RunNumber();
    runstring[3] = runnostream[3].str();
    runnostream[4] << Name() << "_constituentsinjets_mbdnsjet3 Run " << cl->RunNumber();
    runstring[4] = runnostream[4].str();
    runnostream[5] << Name() << "_constituentsinjets_mbdnsjet4 Run " << cl->RunNumber();
    runstring[5] = runnostream[5].str();

    // Loop over pads and add the corresponding runstring
    for (int i = 0; i < 6; ++i) {
      transparent[i]->cd();  // Set current pad
      PrintRun.DrawText(0.5, 0.95, runstring[i].c_str());  // Adjust coordinates as needed
      // Draw histograms here if needed
      TC[i]->Update();  // Update the pad
    }
    return 0;
}

 int JetDraw::DrawRho(const uint32_t trigToDraw /*const JetRes resToDraw*/)
{
  //  QADrawClient *cl = QADrawClient::instance();
  // TCanvas* canvas = new TCanvas( /* etc */ );                                                                                                                                                        
  TH1D *eventwiserho_rhoarea = dynamic_cast<TH1D *>(cl->getHisto(histprefix1 + m_mapTrigToTag[trigToDraw] + std::string("_rhoarea")));
  TH1D *eventwiserho_rhomult = dynamic_cast<TH1D *>(cl->getHisto(histprefix1 + m_mapTrigToTag[trigToDraw] + std::string("_rhomult")));
  TH1D *eventwiserho_sigmaarea = dynamic_cast<TH1D *>(cl->getHisto(histprefix1 + m_mapTrigToTag[trigToDraw] + std::string("_sigmaarea")));
  TH1D *eventwiserho_sigmamult = dynamic_cast<TH1D *>(cl->getHisto(histprefix1 + m_mapTrigToTag[trigToDraw] + std::string("_sigmamult")));

  if (trigToDraw >= m_vecCanvas.size()) {
    m_vecCanvas.resize(trigToDraw + 1); // Resize if necessary                                                                                                                          
  }                                                                              
  if (m_vecCanvas[trigToDraw].empty()) {
  }               
  TCanvas* canvas = new TCanvas("canvas_name", "Title", 800, 600);
  m_vecCanvas[trigToDraw].push_back(canvas);
  // canvas 1
  if (!gROOT->FindObject("eventwiserho"))
    {
      MakeCanvas("eventwiserho", 2);
    }
  /* TC[2]->Clear("D"); */
  Pad[2][0]->cd();
  if (eventwiserho_rhoarea)
    {
      eventwiserho_rhoarea->SetTitle("Rho Area");
      eventwiserho_rhoarea->SetXTitle("#rho*A");
      eventwiserho_rhoarea->SetYTitle("Counts");
      // eventwiserho_rhoarea->GetXaxis()->SetNdivisions(505);
      // eventwiserho_rhoarea->GetXaxis()->SetRangeUser(-1, 15);
      eventwiserho_rhoarea->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
      gPad->SetLogy();
      gPad->SetLogz();
      gPad->SetRightMargin(0.15);
    }
  else
    {                              
      return -1;
    }
  Pad[2][1]->cd();
  if (eventwiserho_rhomult)
    {
      eventwiserho_rhomult->SetTitle("#rho Multiplicity");
      eventwiserho_rhomult->SetXTitle("#rho_{mult}");
      eventwiserho_rhomult->SetYTitle("Counts");
      eventwiserho_rhomult->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  Pad[2][2]->cd();
  if (eventwiserho_sigmaarea)
    {
      eventwiserho_sigmaarea->SetTitle("Sigma Area");
      eventwiserho_sigmaarea->SetXTitle("#sigma*A");
      eventwiserho_sigmaarea->SetYTitle("Counts");
      // eventwiserho_sigmaarea->GetXaxis()->SetNdivisions(505);
      eventwiserho_sigmaarea->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  Pad[2][3]->cd();
  if (eventwiserho_sigmamult)
    {
      eventwiserho_sigmamult->SetTitle("#sigma Multiplicity");
      eventwiserho_sigmamult->SetXTitle("#sigma_{mult}");
      eventwiserho_sigmamult->SetYTitle("Counts");
      eventwiserho_sigmamult->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
    }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
     
  std::ostringstream runnostream[6];
  std::string runstring[6];
  runnostream[0] << Name() << "_eventwiserho_inclusive Run " << cl->RunNumber();
  runstring[0] = runnostream[0].str();
  runnostream[1] << Name() << "_eventwiserho_mbdns1 Run " << cl->RunNumber();
  runstring[1] = runnostream[1].str();
  runnostream[2] << Name() << "_eventwiserho_mbdnsjet1 Run " << cl->RunNumber();
  runstring[2] = runnostream[2].str();
  runnostream[3] << Name() << "_eventwiserho_mbdnsjet2 Run " << cl->RunNumber();
  runstring[3] = runnostream[3].str();
  runnostream[4] << Name() << "_eventwiserho_mbdnsjet3 Run " << cl->RunNumber();
  runstring[4] = runnostream[4].str();
  runnostream[5] << Name() << "_eventwiserho_mbdnsjet4 Run " << cl->RunNumber();
  runstring[5] = runnostream[5].str();
  
  // Loop over pads and add the corresponding runstring                                                                                                                             
  for (int i = 0; i < 6; ++i) {
    transparent[i]->cd();  // Set current pad                              
    PrintRun.DrawText(0.5, 0.95, runstring[i].c_str());  // Adjust coordinates as needed                                                                                                    
    // Draw histograms here if needed                                                                                                                                                        
    TC[i]->Update();  // Update the pad                                                                                                                                                      
  }
  return 0;
}

int JetDraw::DrawJetKinematics(const uint32_t trigToDraw, const JetRes resToDraw)
{
  // Use resToDraw to select histograms or adjust drawing logic
  switch (resToDraw) {
  case R02:
    std::cout << "Resolution R02" << std::endl;
    break;
  case R03:
    std::cout << "Resolution R03" << std::endl;
    break;
  case R04:
    std::cout << "Resolution R04" << std::endl;
    break;
  case R05:
    std::cout << "Resolution R05" << std::endl;
    break;
  default:
    std::cerr << "Unknown resolution" << std::endl;
    break;
  }

  //  QADrawClient *cl = QADrawClient::instance();

  TH2D *jetkinematiccheck_etavsphi = dynamic_cast<TH2D *>(cl->getHisto(histprefix2 + m_mapTrigToTag[trigToDraw] +std::string("_etavsphi_") + m_mapResToTag[resToDraw]));
  TH2D *jetkinematiccheck_jetmassvseta = dynamic_cast<TH2D *>(cl->getHisto(histprefix2 + m_mapTrigToTag[trigToDraw] + std::string("_jetmassvseta_") + m_mapResToTag[resToDraw]));         
  TH2D *jetkinematiccheck_jetmassvspt = dynamic_cast<TH2D *>(cl->getHisto(histprefix2 + m_mapTrigToTag[trigToDraw] + std::string("_jettmassvspt_") + m_mapResToTag[resToDraw]));
  TH2D *jetkinematiccheck_spectra = dynamic_cast<TH2D *>(cl->getHisto(histprefix2 + m_mapTrigToTag[trigToDraw] + std::string("_spectra_") + m_mapResToTag[resToDraw]));
  
  // map of resolution index to tag for TProfiles                                                                                                                                               
  TProfile *jetkinematiccheck_jetmassvseta_pfx = dynamic_cast<TProfile *>(cl->getHisto(histprefix + m_mapTrigToTag[trigToDraw] + std::string("_jetmassvseta_") + m_mapResToTag[resToDraw] + std::string("_pfx")));
  TProfile *jetkinematiccheck_jetmassvspt_pfx = dynamic_cast<TProfile *>(cl->getHisto(histprefix + m_mapTrigToTag[trigToDraw] + std::string("_jettmassvspt_") + m_mapResToTag[resToDraw] + std::string("_pfx")));

  // Ensure m_vecCanvas is a 2D vector if needed                                                                               
  if (trigToDraw >= m_vecCanvas.size()) {
    m_vecCanvas.resize(trigToDraw + 1); // Resize if necessary                               
  }
  // Initialize canvas if not already done                 
  if (m_vecCanvas[trigToDraw].empty()) {
  }                                                                               
  TCanvas* canvas = new TCanvas("canvas_name", "Title", 800, 600);
  m_vecCanvas[trigToDraw].push_back(canvas);
  
  // canvas 1                                                                              
  if (!gROOT->FindObject("jetkinematiccheck"))
    {
      MakeCanvas("jetkinematiccheck", 3);
    }
  Pad[3][0]->cd();
  if (jetkinematiccheck_etavsphi)
    {
      jetkinematiccheck_etavsphi->SetTitle("Jet #eta vs #phi");
      jetkinematiccheck_etavsphi->SetXTitle("Jet #eta");
      jetkinematiccheck_etavsphi->SetYTitle("Jet #phi");
      jetkinematiccheck_etavsphi->SetZTitle("Counts");
      // jetkinematiccheck_etavsphi->GetXaxis()->SetNdivisions(505);
      // jetkinematiccheck_etavsphi->GetXaxis()->SetRangeUser(-1, 15);
      jetkinematiccheck_etavsphi->DrawCopy("COLZ");
      gStyle->SetPalette(4, kBird);
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);     
    }
  else
    {                                                                         
      return -1;
    }
  Pad[3][1]->cd();
  if (jetkinematiccheck_jetmassvseta)
    {
      jetkinematiccheck_jetmassvseta->SetTitle("Jet Mass vs #eta");
      jetkinematiccheck_jetmassvseta->SetXTitle("Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvseta->SetYTitle("Jet #eta");
      jetkinematiccheck_jetmassvseta->SetZTitle("Counts");
      jetkinematiccheck_jetmassvseta->DrawCopy("COLZ");
      gStyle->SetPalette(4, kBird);
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  Pad[3][2]->cd();
  if (jetkinematiccheck_jetmassvspt)
    {
      jetkinematiccheck_jetmassvspt->SetTitle("Jet Mass vs p_{T}");
      jetkinematiccheck_jetmassvspt->SetXTitle("Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvspt->SetYTitle("Jet p_{T} [GeV/c]");
      jetkinematiccheck_jetmassvspt->SetZTitle("Counts");
      jetkinematiccheck_jetmassvspt->DrawCopy("COLZ");
      gStyle->SetPalette(4, kBird);
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  Pad[3][3]->cd();
  if (jetkinematiccheck_spectra)
    {
      jetkinematiccheck_spectra->SetTitle("Jet Spectra");
      jetkinematiccheck_spectra->SetXTitle("p_{T,Jet} [GeV/c]");
      jetkinematiccheck_spectra->SetYTitle("Counts");
      jetkinematiccheck_spectra->DrawCopy("HIST");
      gPad->UseCurrentStyle();
    }
  Pad[3][4]->cd();
  if (jetkinematiccheck_jetmassvseta_pfx)
    {
      jetkinematiccheck_jetmassvseta_pfx->SetTitle("Average Jet Mass vs #eta");
      jetkinematiccheck_jetmassvseta_pfx->SetXTitle("#eta");
      jetkinematiccheck_jetmassvseta_pfx->SetYTitle("Average Jet Mass [GeV/c^{2}])");
      jetkinematiccheck_jetmassvseta_pfx->DrawCopy("HIST");
      gPad->UseCurrentStyle();
    }
  Pad[3][5]->cd();
  if (jetkinematiccheck_jetmassvspt_pfx)
    {
      jetkinematiccheck_jetmassvspt_pfx->SetTitle("Average Jet Mass vs p_{T}");
      jetkinematiccheck_jetmassvspt_pfx->SetXTitle("Average Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvspt_pfx->SetYTitle("p_{T} [GeV/c]");
      jetkinematiccheck_jetmassvspt_pfx->DrawCopy("HIST");
      gPad->UseCurrentStyle();
    }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized 
  PrintRun.SetTextAlign(23);  // center/top alignment
                               
  // Generate run strings		     
  std::ostringstream runnostream[6];
  std::string runstring[6];
  runnostream[0] << Name() << "_jetkinematiccheck_inclusive Run " << cl->RunNumber();
  runstring[0] = runnostream[0].str();
  runnostream[1] << Name() << "_jetkinematiccheck_mbdns1 Run " << cl->RunNumber();
  runstring[1] = runnostream[1].str();
  runnostream[2] << Name() << "_jetkinematiccheck_mbdnsjet1 Run " << cl->RunNumber();
  runstring[2] = runnostream[2].str();
  runnostream[3] << Name() << "_jetkinematiccheck_mbdnsjet2 Run " << cl->RunNumber();
  runstring[3] = runnostream[3].str();
  runnostream[4] << Name() << "_jetkinematiccheck_mbdnsjet3 Run " << cl->RunNumber();
  runstring[4] = runnostream[4].str();
  runnostream[5] << Name() << "_jetkinematiccheck_mbdnsjet4 Run " << cl->RunNumber();
  runstring[5] = runnostream[5].str();
  
  for (int i = 0; i < 6; ++i) {
    transparent[i]->cd();  // Set current pad
    PrintRun.DrawText(0.5, 0.95, runstring[i].c_str());  // Adjust coordinates as 
    TC[i]->Update();  // Update 
  }
  return 0;
}

int JetDraw::DrawJetSeed(const uint32_t trigToDraw, const JetRes resToDraw)
{
  // Use resToDraw to select histograms or adjust drawing logic                                                                           
  switch (resToDraw) {
  case R02:
    std::cout << "Resolution R02" << std::endl;
    break;
  case R03:
    std::cout << "Resolution R03" << std::endl;
    break;
  case R04:
    std::cout << "Resolution R04" << std::endl;
    break;
  case R05:
    std::cout << "Resolution R05" << std::endl;
    break;
  default:
    std::cerr << "Unknown resolution" << std::endl;
    break;
  }
  
  //  QADrawClient *cl = QADrawClient::instance();
  TH2F *jetseedcount_rawetavsphi = dynamic_cast<TH2F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_rawetavsphi")));
  TH1F *jetseedcount_rawpt = dynamic_cast<TH1F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_rawpt")));
  TH1F *jetseedcount_rawptall = dynamic_cast<TH1F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_rawptall")));
  TH1F *jetseedcount_rawseedcount = dynamic_cast<TH1F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_rawseedcount")));
  TH2F *jetseedcount_subetavsphi = dynamic_cast<TH2F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_subetavsphi")));
  TH1F *jetseedcount_subpt = dynamic_cast<TH1F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_subpt")));
  TH1F *jetseedcount_subptall = dynamic_cast<TH1F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_subptall")));  
  TH1F *jetseedcount_subseedcount = dynamic_cast<TH1F *>(cl->getHisto(histprefix3 + m_mapTrigToTag[trigToDraw] + m_mapResToTag[resToDraw] + std::string("_subseedcount")));
  
  // Ensure m_vecCanvas is a 2D vector if needed                                      
  if (trigToDraw >= m_vecCanvas.size()) {
    m_vecCanvas.resize(trigToDraw + 1);
  }
  // Initialize canvas if not already done                                                                                                                
  if (m_vecCanvas[trigToDraw].empty()) {
  }                                                                                                                                                                                             
  TCanvas* canvas = new TCanvas("canvas_name", "Title", 800, 600);
  m_vecCanvas[trigToDraw].push_back(canvas);
  //canvas 1                                                                                  
  if (!gROOT->FindObject("jetseedcount1"))
    {
      MakeCanvas("jetseedcount1", 4);
    }
  Pad[4][0]->cd();
  if (jetseedcount_rawetavsphi)
    {
      jetseedcount_rawetavsphi->SetLineColor(kBlue);
      jetseedcount_rawetavsphi->GetXaxis()->SetRangeUser(0.0, 12000);
      jetseedcount_rawetavsphi->SetTitle("Raw Seed #eta vs #phi");
      jetseedcount_rawetavsphi->SetXTitle("Jet #eta_{raw} [Rads.]");
      jetseedcount_rawetavsphi->SetYTitle("Jet #phi_{raw} [Rads.]");
      jetseedcount_rawetavsphi->SetZTitle("Counts");
      // jetseedcount_rawetavsphi->GetXaxis()->SetNdivisions(505);
      jetseedcount_rawetavsphi->DrawCopy();
      gPad->UseCurrentStyle();
    }
  else
    {                                                                            
      return -1;
    }
  Pad[4][1]->cd();
  if (jetseedcount_rawpt)
    {
      jetseedcount_rawpt->SetTitle("Raw p_{T}");
      jetseedcount_rawpt->SetXTitle("Jet p_{T,Raw} [GeV]");
      jetseedcount_rawpt->SetYTitle("Counts");
      jetseedcount_rawpt->DrawCopy("");
      gPad->UseCurrentStyle();
    }
  Pad[4][2]->cd();
  if (jetseedcount_rawptall)
    {
      jetseedcount_rawptall->SetTitle("Raw p_{T} (all jet seeds)");
      jetseedcount_rawptall->SetXTitle("Jet p_{T,all seed} [GeV]");
      jetseedcount_rawptall->SetYTitle("Counts");
      jetseedcount_rawptall->DrawCopy("");
      gPad->UseCurrentStyle();
    }
  Pad[4][3]->cd();
  if (jetseedcount_rawseedcount)
    {
      jetseedcount_rawseedcount->SetTitle("Raw Seed Count per Event");
      jetseedcount_rawseedcount->SetXTitle("Raw Seed Count per Event");
      jetseedcount_rawseedcount->SetYTitle("Counts");
      jetseedcount_rawseedcount->DrawCopy("");
      gPad->UseCurrentStyle();
    }

  //canvas 2                                                                                                                                                                                  
  if (!gROOT->FindObject("jetseedcount2"))
    {
      MakeCanvas("jetseedcount2", 5);
    }
  Pad[5][0]->cd();
  if (jetseedcount_subetavsphi)
    {
      jetseedcount_subetavsphi->SetLineColor(kBlue);
      // jetseedcount_subetavsphi->GetXaxis()->SetRangeUser(0.0, 12000);
      jetseedcount_subetavsphi->SetTitle("Sub-seed #eta vs #phi");
      jetseedcount_subetavsphi->SetXTitle("Jet #eta_{subseed} [Rads.]");
      jetseedcount_subetavsphi->SetYTitle("Jet #phi_{subseed} [Rads.]");
      jetseedcount_subetavsphi->SetZTitle("Counts");
      // jetseedcount_subetavsphi->GetXaxis()->SetNdivisions(505);
      jetseedcount_subetavsphi->DrawCopy();
      gPad->UseCurrentStyle();
    }
  else
    {
      return -1;
    }
  Pad[5][1]->cd();
  if (jetseedcount_subpt)
    {
      jetseedcount_subpt->SetTitle("Sub. p_{T}");
      jetseedcount_subpt->SetXTitle("Jet p_{T,sub.} [GeV]");
      jetseedcount_subpt->SetYTitle("Counts");
      jetseedcount_subpt->DrawCopy("");
      gPad->UseCurrentStyle();
    }
  Pad[5][2]->cd();
  if (jetseedcount_subptall)
    {
      jetseedcount_subptall->SetTitle("Sub. p_{T} (all jet seeds)");
      jetseedcount_subptall->SetXTitle("Jet p_{T, all-sub.} [GeV]");
      jetseedcount_subptall->SetYTitle("Counts");
      jetseedcount_subptall->DrawCopy("");
      gPad->UseCurrentStyle();
    }
  Pad[5][3]->cd();
  if (jetseedcount_subseedcount)
    {
      jetseedcount_subseedcount->SetTitle("Sub Seed Count per Event");
      jetseedcount_subseedcount->SetXTitle("Sub Seed Count per Event");
      jetseedcount_subseedcount->SetYTitle("Counts");
      jetseedcount_subseedcount->DrawCopy("");
      gPad->UseCurrentStyle();
    }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized                                                                                                                                                                                                                                     
  PrintRun.SetTextAlign(23);  // center/top alignment                                                                                                                                                                                                                           
  // Generate run strings                                                                                                                                                                      
  std::ostringstream runnostream[6];
  std::string runstring[6];
  runnostream[0] << Name() << "_jetseedcount_inclusive Run " << cl->RunNumber();
  runstring[0] = runnostream[0].str();
  runnostream[1] << Name() << "_jetseedcount_mbdns1 Run " << cl->RunNumber();
  runstring[1] = runnostream[1].str();
  runnostream[2] << Name() << "_jetseedcount_mbdnsjet1 Run " << cl->RunNumber();
  runstring[2] = runnostream[2].str();
  runnostream[3] << Name() << "_jetseedcount_mbdnsjet2 Run " << cl->RunNumber();
  runstring[3] = runnostream[3].str();
  runnostream[4] << Name() << "_jetseedcount_mbdnsjet3 Run " << cl->RunNumber();
  runstring[4] = runnostream[4].str();
  runnostream[5] << Name() << "_jetseedcount_mbdnsjet4 Run " << cl->RunNumber();
  runstring[5] = runnostream[5].str();

  for (int i = 0; i < 6; ++i) {
    transparent[i]->cd();  // Set current pad                                                                                                                                                 
    PrintRun.DrawText(0.5, 0.95, runstring[i].c_str());  // Adjust coordinates as                                                                                                             
    TC[i]->Update();  // Update                                                                                                                                                                
  }
  return 0;
}

int JetDraw::DBVarInit()
{
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}
 void JetDraw::SetJetSummary(TCanvas* c)
 {
   jetSummary = c;
   jetSummary->cd();

   //  QADrawClient *cl = QADrawClient::instance();
   TPad* tr = new TPad("transparent_jet", "", 0, 0, 1, 1);
   tr->SetFillStyle(4000);
   tr->Draw();
   TText PrintRun;
   PrintRun.SetTextFont(62);
   PrintRun.SetTextSize(0.04);
   PrintRun.SetNDC();          // set to normalized coordinates                                                                                                                                                                                                                  
   PrintRun.SetTextAlign(23);  // center/top alignment                                                                                                                                                                                                                           
   std::ostringstream runnostream;
   std::string runstring;
   runnostream << Name() << "_jet_summary Run " << cl->RunNumber();
   runstring = runnostream.str();
   tr->cd();
   PrintRun.DrawText(0.5, 1., runstring.c_str());
   jetSummary->Update();
 }
void JetDraw::myText(double x, double y, int color, const char *text, double tsize)
{
  TLatex l;
  l.SetTextAlign(22);
  l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x, y, text);
}

