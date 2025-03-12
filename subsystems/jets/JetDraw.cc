//Jennifer James <jennifer.l.james@vanderbilt.edu>, McKenna Sleeth, and Derek Anderson
#include "JetDraw.h"
#include <sPhenixStyle.C>
#include <qahtml/QADrawClient.h>
#include <qahtml/QADrawDB.h>
#include <TCanvas.h>
#include <TDatime.h>
#include <TFile.h>
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
: QADraw(name)
{
  DBVarInit();
  m_do_debug = false;
  m_constituent_prefix = "h_constituentsinjets";
  m_rho_prefix = "h_eventwiserho";
  m_kinematic_prefix = "h_jetkinematiccheck";
  m_seed_prefix = "h_jetseedcount";
  m_jet_type = "towersub1_antikt";
  return;
}

JetDraw::~JetDraw()
{
  /* delete db; */
  return;
}

int JetDraw::MakeHtml(const std::string &what)
{
  const int drawError = Draw(what);  // Call to Draw
  if (drawError) {
    return drawError;  // Return if there is an error in Draw
  }

  // instantiate draw client
  QADrawClient *cl = QADrawClient::instance();

  // loop over triggers to draw
  for (std::size_t iTrigToDraw = 0; iTrigToDraw < m_vecTrigToDraw.size(); ++iTrigToDraw)
<<<<<<< HEAD
  {
    // grab index & name of trigger being drawn
    const uint32_t    idTrig   = m_vecTrigToDraw[iTrigToDraw];
    const std::string nameTrig = m_mapTrigToName[idTrig];

    // draw rho plots
    const std::string pngRho = cl->htmlRegisterPage(*this, nameTrig, nameTrig, "png");
    cl->CanvasToPng(m_vecRhoCanvas[iTrigToDraw], pngRho);

    // loop over jet resolutions to draw
    for (std::size_t iResToDraw = 0; iResToDraw < m_vecResToDraw.size(); ++iResToDraw)
    {
      // grab index & name of resolution being drawn
      const uint32_t    idRes   = m_vecResToDraw[iResToDraw];
      const std::string nameRes = m_mapResToName[idRes];
      const std::string dirRes  = nameTrig + "/" + nameRes;
      const std::string fileRes = nameTrig + "_" + nameRes;

      // draw constituent plots 
      const std::string pngCst = cl->htmlRegisterPage(*this, dirRes, fileRes + "_cst", "png");
      cl->CanvasToPng(m_vecCstCanvas[iTrigToDraw][iResToDraw], pngCst);

      // draw kinematic plots 
      const std::string pngKine = cl->htmlRegisterPage(*this, dirRes, fileRes + "_kine", "png");
      cl->CanvasToPng(m_vecKineCanvas[iTrigToDraw][iResToDraw], pngKine);

      // draw seed plots 
      const std::string pngSeed = cl->htmlRegisterPage(*this, dirRes, fileRes + "_seed", "png");
      cl->CanvasToPng(m_vecSeedCanvas[iTrigToDraw][iResToDraw], pngSeed);
    }
  }
=======
    {
      // grab index & name of trigger being drawn
      const uint32_t    idTrig   = m_vecTrigToDraw[iTrigToDraw];
      const std::string nameTrig = m_mapTrigToName[idTrig];

      // draw rho plots
      const std::string pngRho = cl->htmlRegisterPage(*this, nameTrig, nameTrig, "png");
      cl->CanvasToPng(m_vecRhoCanvas[iTrigToDraw], pngRho);

      // loop over jet resolutions to draw
      for (std::size_t iResToDraw = 0; iResToDraw < m_vecResToDraw.size(); ++iResToDraw)
	{
	  // grab index & name of resolution being drawn
	  const uint32_t    idRes   = m_vecResToDraw[iResToDraw];
	  const std::string nameRes = m_mapResToName[idRes];
	  const std::string dirRes  = nameTrig + "/" + nameRes;
	  const std::string fileRes = nameTrig + "_" + nameRes;

	  // draw constituent plots 
	  const std::string pngCst = cl->htmlRegisterPage(*this, dirRes, fileRes + "_cst", "png");
	  cl->CanvasToPng(m_vecCstCanvas[iTrigToDraw][iResToDraw], pngCst);

	  // draw kinematic plots 
	  const std::string pngKine = cl->htmlRegisterPage(*this, dirRes, fileRes + "_kine", "png");
	  cl->CanvasToPng(m_vecKineCanvas[iTrigToDraw][iResToDraw], pngKine);

	  // draw seed plots 
	  const std::string pngSeed = cl->htmlRegisterPage(*this, dirRes, fileRes + "_seed", "png");
	  cl->CanvasToPng(m_vecSeedCanvas[iTrigToDraw][iResToDraw], pngSeed);
	}
    }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)

  // return w/o error
  return 0;
}

int JetDraw::Draw(const std::string &what)
{
<<<<<<< HEAD
   // loop over triggers
   int iret = 0;
   int idraw = 0;
   for (uint32_t trigToDraw : m_vecTrigToDraw)
   {

     // add new rows for reso.-dependent canvases
     m_vecCstCanvas.push_back( {} );
     m_vecKineCanvas.push_back( {} );
     m_vecSeedCanvas.push_back( {} );

     // likewise for the run pad vectors
     m_vecCstRun.push_back( {} );
     m_vecKineRun.push_back( {} );
     m_vecSeedRun.push_back( {} );

     // draw rho plots
     if  (what == "ALL" || what == "RHO")
     {
       iret = DrawRho(trigToDraw);
       ++idraw;
     }

     // now loop over resolutions to draw
     for (uint32_t resToDraw : m_vecResToDraw)
     {

       // draw constituent plots
       if (what == "ALL" || what == "CONTSTITUENTS")
       {
	 iret = DrawConstituents(trigToDraw, static_cast<JetRes>(resToDraw));
	 ++idraw;
       }

       // draw kinematic plots
       if (what == "ALL" || what == "KINEMATICCHECK")
       {
         iret = DrawJetKinematics(trigToDraw, static_cast<JetRes>(resToDraw));
         ++idraw;
       }

       // draw seed plots
       if (what == "ALL" || what == "SEEDCOUNT")
       {
         iret = DrawJetSeed(trigToDraw, static_cast<JetRes>(resToDraw));
         ++idraw;
       }
     }
   }

   if (!idraw)
   {
     std::cout << " Unimplemented Drawing option: " << what << std::endl;
     return -1;
   }

   // should return -1 if error, 0 otherwise
   return iret;
=======
  // loop over triggers
  int iret = 0;
  int idraw = 0;
  for (uint32_t trigToDraw : m_vecTrigToDraw)
    {

      // add new rows for reso.-dependent canvases
      m_vecCstCanvas.push_back( {} );
      m_vecKineCanvas.push_back( {} );
      m_vecSeedCanvas.push_back( {} );

      // likewise for the run pad vectors
      m_vecCstRun.push_back( {} );
      m_vecKineRun.push_back( {} );
      m_vecSeedRun.push_back( {} );

      // draw rho plots
      if  (what == "ALL" || what == "RHO")
	{
	  iret = DrawRho(trigToDraw);
	  ++idraw;
	}

      // now loop over resolutions to draw
      for (uint32_t resToDraw : m_vecResToDraw)
	{

	  // draw constituent plots
	  if (what == "ALL" || what == "CONTSTITUENTS")
	    {
	      iret = DrawConstituents(trigToDraw, static_cast<JetRes>(resToDraw));
	      ++idraw;
	    }

	  // draw kinematic plots
	  if (what == "ALL" || what == "KINEMATICCHECK")
	    {
	      iret = DrawJetKinematics(trigToDraw, static_cast<JetRes>(resToDraw));
	      ++idraw;
	    }

	  // draw seed plots
	  if (what == "ALL" || what == "SEEDCOUNT")
	    {
	      iret = DrawJetSeed(trigToDraw, static_cast<JetRes>(resToDraw));
	      ++idraw;
	    }
	}
    }

  if (!idraw)
    {
      std::cout << " Unimplemented Drawing option: " << what << std::endl;
      return -1;
    }

  // should return -1 if error, 0 otherwise
  return iret;
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
}
 
int JetDraw::MakeCanvas(const std::string &name, const int nHist, VCanvas1D& canvas, VPad1D& run)
{
  // instantiate draw client & grab display size
  QADrawClient *cl = QADrawClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  // create canvas
  // n.b. xpos (-1) negative means do not draw menu bar
  canvas.emplace_back( new TCanvas(name.data(), "", -1, 0, (int) (xsize / 1.2), (int) (ysize / 1.2)) );
  canvas.back()->UseCurrentStyle();
  gSystem->ProcessEvents();

  // divide canvas into appropriate no. of pads
  const int   nRow = std::min(nHist, 2);
  const int   nCol = (nHist / 2) + (nHist % 2);
  const float bRow = 0.01;
  const float bCol = 0.01;
  if (nHist > 1)
<<<<<<< HEAD
  {
    canvas.back()->Divide(nRow, nCol, bRow, bCol);
  }
=======
    {
      canvas.back()->Divide(nRow, nCol, bRow, bCol);
    }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
  canvas.back()->cd();

  // create pad for run number
  const std::string runPadName = name + "_run";
  run.emplace_back( new TPad(runPadName.data(), "this does not show", 0, 0, 1, 1) );
  run.back()->SetFillStyle(4000);
  run.back()->Draw();

  // return w/o error
  return 0;
}

int JetDraw::DrawRho(const uint32_t trigToDraw /*const JetRes resToDraw*/)
{

  // for rho histogram names
  const std::string rhoHistName = std::string(m_rho_prefix) + "_" + m_mapTrigToTag[trigToDraw];

  // grab relevant histograms from draw client
  QADrawClient *cl = QADrawClient::instance();

  TH1D *eventwiserho_rhoarea = dynamic_cast<TH1D *>(cl->getHisto(rhoHistName + "_rhoarea"));
  TH1D *eventwiserho_rhomult = dynamic_cast<TH1D *>(cl->getHisto(rhoHistName + "_rhomult"));
  TH1D *eventwiserho_sigmaarea = dynamic_cast<TH1D *>(cl->getHisto(rhoHistName + "_sigmaarea"));
  TH1D *eventwiserho_sigmamult = dynamic_cast<TH1D *>(cl->getHisto(rhoHistName + "_sigmamult"));

  // form canvas name & if it doesn't exist yet, create it
  const std::string rhoCanName = "evtRho_" + m_mapTrigToTag[trigToDraw];
  if (!gROOT->FindObject(rhoCanName.data()))
<<<<<<< HEAD
  {
    MakeCanvas(rhoCanName, 4, m_vecRhoCanvas, m_vecRhoRun);
  }

  m_vecRhoCanvas.back()->cd(1);
  if (eventwiserho_rhoarea)
  {
    eventwiserho_rhoarea->SetTitle("Rho Area");
    eventwiserho_rhoarea->SetXTitle("#rho*A");
    eventwiserho_rhoarea->SetYTitle("Counts");
    // eventwiserho_rhoarea->GetXaxis()->SetNdivisions(505);
    // eventwiserho_rhoarea->GetXaxis()->SetRangeUser(-1, 15);
    eventwiserho_rhoarea->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
    gPad->SetLogy();
    gPad->SetLogz();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecRhoCanvas.back()->cd(2);
  if (eventwiserho_rhomult)
  {
    eventwiserho_rhomult->SetTitle("#rho Multiplicity");
    eventwiserho_rhomult->SetXTitle("#rho_{mult}");
    eventwiserho_rhomult->SetYTitle("Counts");
    eventwiserho_rhomult->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecRhoCanvas.back()->cd(3);
  if (eventwiserho_sigmaarea)
  {
    eventwiserho_sigmaarea->SetTitle("Sigma Area");
    eventwiserho_sigmaarea->SetXTitle("#sigma*A");
    eventwiserho_sigmaarea->SetYTitle("Counts");
    // eventwiserho_sigmaarea->GetXaxis()->SetNdivisions(505);
    eventwiserho_sigmaarea->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecRhoCanvas.back()->cd(4);
  if (eventwiserho_sigmamult)
  {
    eventwiserho_sigmamult->SetTitle("#sigma Multiplicity");
    eventwiserho_sigmamult->SetXTitle("#sigma_{mult}");
    eventwiserho_sigmamult->SetYTitle("Counts");
    eventwiserho_sigmamult->DrawCopy("COLZ");
    gPad->UseCurrentStyle();
  }
  else
  {
    // histogram is missing
    return -1;
  }
=======
    {
      MakeCanvas(rhoCanName, 4, m_vecRhoCanvas, m_vecRhoRun);
    }

  m_vecRhoCanvas.back()->cd(1);
  if (eventwiserho_rhoarea)
    {
      eventwiserho_rhoarea->SetTitle("Rho Area");
      eventwiserho_rhoarea->SetXTitle("#rho*A");
      eventwiserho_rhoarea->SetYTitle("Counts");
      // eventwiserho_rhoarea->GetXaxis()->SetNdivisions(505);
      // eventwiserho_rhoarea->GetXaxis()->SetRangeUser(-1, 15);
      eventwiserho_rhoarea->DrawCopy("HIST");  // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->SetLogy();
      gPad->SetLogz();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecRhoCanvas.back()->cd(2);
  if (eventwiserho_rhomult)
    {
      eventwiserho_rhomult->SetTitle("#rho Multiplicity");
      eventwiserho_rhomult->SetXTitle("#rho_{mult}");
      eventwiserho_rhomult->SetYTitle("Counts");
      eventwiserho_rhomult->DrawCopy("HIST");  // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecRhoCanvas.back()->cd(3);
  if (eventwiserho_sigmaarea)
    {
      eventwiserho_sigmaarea->SetTitle("Sigma Area");
      eventwiserho_sigmaarea->SetXTitle("#sigma*A");
      eventwiserho_sigmaarea->SetYTitle("Counts");
      // eventwiserho_sigmaarea->GetXaxis()->SetNdivisions(505);
      eventwiserho_sigmaarea->DrawCopy("HIST");  // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecRhoCanvas.back()->cd(4);
  if (eventwiserho_sigmamult)
    {
      eventwiserho_sigmamult->SetTitle("#sigma Multiplicity");
      eventwiserho_sigmamult->SetXTitle("#sigma_{mult}");
      eventwiserho_sigmamult->SetYTitle("Counts");
      eventwiserho_sigmamult->DrawCopy("COLZ");
      gPad->UseCurrentStyle();
    }
  else
    {
      // histogram is missing
      return -1;
    }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment

  // Generate run string from client
  std::ostringstream runnostream;
<<<<<<< HEAD
=======
  //  QADrawClient *cl = QADrawClient::instance();
  std::cout << "Build info: " << cl->build() << std::endl;
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
  runnostream << cl->RunNumber() << ", build " << cl->build();

  // prepend module name, component, trigger, and resolution
  std::string runstring = Name();
  runstring += "_EvtRho_";
  runstring += m_mapTrigToName[trigToDraw];
  runstring += " Run ";
  runstring += runnostream.str();

  // add runstring to relevant pad
  m_vecRhoRun.back()->cd();
  PrintRun.DrawText(0.5, 0.95, runstring.data());  // Adjust coordinates as needed
  m_vecRhoCanvas.back()->Update();  // Update the pad

  // return w/o error
  return 0;
}

int JetDraw::DrawConstituents(const uint32_t trigToDraw, const JetRes resToDraw)
{
  // Use resToDraw to select histograms or adjust drawing logic
  switch (resToDraw) {
  case R02:
    if (m_do_debug) std::cout << "Resolution R02" << std::endl;
    break;
  case R03:
    if (m_do_debug) std::cout << "Resolution R03" << std::endl;
    break;
  case R04:
    if (m_do_debug) std::cout << "Resolution R04" << std::endl;
    break;
  case R05:
    if (m_do_debug) std::cout << "Resolution R05" << std::endl;
    break;
  default:
    std::cerr << "Unknown resolution" << std::endl;
    return -1;
  }

  // for constituent hist names
  const std::string cstHistName = std::string(m_constituent_prefix) + "_" + m_mapTrigToTag[trigToDraw] + "_" + std::string(m_jet_type) + "_" + m_mapResToTag[resToDraw];

  // grab relevant histograms from draw client
  QADrawClient *cl = QADrawClient::instance();
<<<<<<< HEAD

=======
  
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
  TH1D *constituents_ncsts_cemc = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_cemc"));
  TH1D *constituents_ncsts_ihcal = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_ihcal"));
  TH1D *constituents_ncsts_ohcal = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_ohcal"));
  TH1D *constituents_ncsts_total = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_total"));
  TH2D *constituents_ncstsvscalolayer = dynamic_cast<TH2D *>(cl->getHisto(cstHistName + "_ncstsvscalolayer"));
  TH1D *constituents_efracjet_cemc = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_efracjet_cemc"));
  TH1D *constituents_efracjet_ihcal = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_efracjet_ihcal"));
  TH1D *constituents_efracjet_ohcal = dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_efracjet_ohcal"));
  TH2D *constituents_efracjetvscalolayer = dynamic_cast<TH2D *>(cl->getHisto(cstHistName + "_efracjetvscalolayer"));

  // form canvas name & if it doesn't exist yet, create it
  const std::string cstCanName = "jetCsts_" + m_mapTrigToTag[trigToDraw] + "_" + m_mapResToTag[resToDraw];
  if (!gROOT->FindObject(cstCanName.data()))
  {
    MakeCanvas(cstCanName, 9, m_vecCstCanvas.back(), m_vecCstRun.back());
  }
<<<<<<< HEAD

  m_vecCstCanvas.back().back()->cd(1);
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

  m_vecCstCanvas.back().back()->cd(2);
  if (constituents_ncsts_ihcal)
  {
    constituents_ncsts_ihcal->SetTitle("Jet N Constituents in IHCal");
    constituents_ncsts_ihcal->SetXTitle("N Constituents");
    constituents_ncsts_ihcal->SetYTitle("Counts");
    constituents_ncsts_ihcal->GetXaxis()->SetNdivisions(505);
    constituents_ncsts_ihcal->DrawCopy("HIST"); // 1D histogram
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(3);
  if (constituents_ncsts_ohcal)
  {
    constituents_ncsts_ohcal->SetTitle("Jet N Constituents in OHCal");
    constituents_ncsts_ohcal->SetXTitle("N Constituents");
    constituents_ncsts_ohcal->SetYTitle("Counts");
    constituents_ncsts_ohcal->GetXaxis()->SetNdivisions(505);
    constituents_ncsts_ohcal->DrawCopy("HIST"); // 1D Histogram                             
    gPad->UseCurrentStyle();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(4);
  if (constituents_ncsts_total)
  {
    constituents_ncsts_total->SetTitle("Jet N Constituents");
    constituents_ncsts_total->SetXTitle("N Constituents");
    constituents_ncsts_total->GetXaxis()->SetNdivisions(505);
    constituents_ncsts_total->SetYTitle("Counts");
    constituents_ncsts_total->DrawCopy("HIST"); // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(5);
  if (constituents_ncstsvscalolayer)
  {
    constituents_ncstsvscalolayer->SetTitle("Jet N Constituents vs Calo Layer");
    constituents_ncstsvscalolayer->SetXTitle("N Constituents");
    constituents_ncstsvscalolayer->SetYTitle("Calo Layer");
    constituents_ncstsvscalolayer->SetZTitle("Counts");
    constituents_ncstsvscalolayer->DrawCopy("COLZ"); // 2D Histogram
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(6);
  if (constituents_efracjetvscalolayer)
  {
    constituents_efracjetvscalolayer->SetTitle("Jet E Fraction vs Calo Layer");
    constituents_efracjetvscalolayer->SetXTitle("Jet E Fraction");
    constituents_efracjetvscalolayer->SetYTitle("Calo Layer");
    constituents_efracjetvscalolayer->SetZTitle("Counts");
    constituents_efracjetvscalolayer->DrawCopy("COLZ"); // 2D Histogram
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(7);
  if (constituents_efracjet_cemc)
  {
    constituents_efracjet_cemc->SetTitle("Jet E Fraction in CEMC");
    constituents_efracjet_cemc->SetXTitle("Jet E fraction");
    constituents_efracjet_cemc->SetYTitle("Counts");
    constituents_efracjet_cemc->DrawCopy("HIST"); // 1D Histogram
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(8);
  if (constituents_efracjet_ihcal)
  {
    constituents_efracjet_ihcal->SetTitle("Jet Fraction in IHCal");
    constituents_efracjet_ihcal->SetXTitle("Jet E Fraction");
    constituents_efracjet_ihcal->SetYTitle("Counts");
    constituents_efracjet_ihcal->DrawCopy("HIST"); // 1D Histogram
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecCstCanvas.back().back()->cd(9);
  if (constituents_efracjet_ohcal)
  {
    constituents_efracjet_ohcal->SetTitle("Jet Fraction in OHCal");
    constituents_efracjet_ohcal->SetXTitle("Jet E Fraction");
    constituents_efracjet_ohcal->SetYTitle("Counts");
    constituents_efracjet_ohcal->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }
=======
  

  // Grab histograms
  //  QADrawClient *cl = QADrawClient::instance();
  std::vector<TH1*> constituents = {
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_cemc")),
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_ihcal")),
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_ohcal")),
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_ncsts_total")),
    dynamic_cast<TH2D *>(cl->getHisto(cstHistName + "_ncstsvscalolayer")),
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_efracjet_cemc")),
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_efracjet_ihcal")),
    dynamic_cast<TH1D *>(cl->getHisto(cstHistName + "_efracjet_ohcal")),
    dynamic_cast<TH2D *>(cl->getHisto(cstHistName + "_efracjetvscalolayer"))
  };

  // Ensure there's a row for the current trigger in canvas and run vectors
  
  m_vecCstCanvas.push_back(std::vector<TCanvas*>());
  //  m_vecCstRun.push_back(std::vector<int>());
  m_vecCstRun.push_back(std::vector<TPad*>());

  m_vecCstCanvas.back().back()->cd(1);
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

  m_vecCstCanvas.back().back()->cd(2);
  if (constituents_ncsts_ihcal)
    {
      constituents_ncsts_ihcal->SetTitle("Jet N Constituents in IHCal");
      constituents_ncsts_ihcal->SetXTitle("N Constituents");
      constituents_ncsts_ihcal->SetYTitle("Counts");
      constituents_ncsts_ihcal->GetXaxis()->SetNdivisions(505);
      constituents_ncsts_ihcal->DrawCopy("HIST"); // 1D histogram
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(3);
  if (constituents_ncsts_ohcal)
    {
      constituents_ncsts_ohcal->SetTitle("Jet N Constituents in OHCal");
      constituents_ncsts_ohcal->SetXTitle("N Constituents");
      constituents_ncsts_ohcal->SetYTitle("Counts");
      constituents_ncsts_ohcal->GetXaxis()->SetNdivisions(505);
      constituents_ncsts_ohcal->DrawCopy("HIST"); // 1D Histogram                             
      gPad->UseCurrentStyle();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(4);
  if (constituents_ncsts_total)
    {
      constituents_ncsts_total->SetTitle("Jet N Constituents");
      constituents_ncsts_total->SetXTitle("N Constituents");
      constituents_ncsts_total->GetXaxis()->SetNdivisions(505);
      constituents_ncsts_total->SetYTitle("Counts");
      constituents_ncsts_total->DrawCopy("HIST"); // 1D Histogram
      gPad->UseCurrentStyle();
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(5);
  if (constituents_ncstsvscalolayer)
    {
      constituents_ncstsvscalolayer->SetTitle("Jet N Constituents vs Calo Layer");
      constituents_ncstsvscalolayer->SetXTitle("N Constituents");
      constituents_ncstsvscalolayer->SetYTitle("Calo Layer");
      constituents_ncstsvscalolayer->SetZTitle("Counts");
      constituents_ncstsvscalolayer->DrawCopy("COLZ"); // 2D Histogram
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(6);
  if (constituents_efracjetvscalolayer)
    {
      constituents_efracjetvscalolayer->SetTitle("Jet E Fraction vs Calo Layer");
      constituents_efracjetvscalolayer->SetXTitle("Jet E Fraction");
      constituents_efracjetvscalolayer->SetYTitle("Calo Layer");
      constituents_efracjetvscalolayer->SetZTitle("Counts");
      constituents_efracjetvscalolayer->DrawCopy("COLZ"); // 2D Histogram
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(7);
  if (constituents_efracjet_cemc)
    {
      constituents_efracjet_cemc->SetTitle("Jet E Fraction in CEMC");
      constituents_efracjet_cemc->SetXTitle("Jet E fraction");
      constituents_efracjet_cemc->SetYTitle("Counts");
      constituents_efracjet_cemc->DrawCopy("HIST"); // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(8);
  if (constituents_efracjet_ihcal)
    {
      constituents_efracjet_ihcal->SetTitle("Jet Fraction in IHCal");
      constituents_efracjet_ihcal->SetXTitle("Jet E Fraction");
      constituents_efracjet_ihcal->SetYTitle("Counts");
      constituents_efracjet_ihcal->DrawCopy("HIST"); // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecCstCanvas.back().back()->cd(9);
  if (constituents_efracjet_ohcal)
    {
      constituents_efracjet_ohcal->SetTitle("Jet Fraction in OHCal");
      constituents_efracjet_ohcal->SetXTitle("Jet E Fraction");
      constituents_efracjet_ohcal->SetYTitle("Counts");
      constituents_efracjet_ohcal->DrawCopy("HIST");  // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment

  // Generate run string from client
  std::ostringstream runnostream;
  runnostream << cl->RunNumber() << ", build " << cl->build();

  // prepend module name, component, trigger, and resolution
  std::string runstring = Name();
  runstring += "_JetCsts_";
  runstring += m_mapTrigToName[trigToDraw];
  runstring += "_";
  runstring += m_mapResToName[resToDraw];
  runstring += " Run ";
  runstring += runnostream.str();
<<<<<<< HEAD

=======
  /*
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
  // add runstring to relevant pad
  m_vecCstRun.back().back()->cd();
  PrintRun.DrawText(0.5, 0.95, runstring.data());  // Adjust coordinates as needed
  m_vecCstCanvas.back().back()->Update();  // Update the pad
<<<<<<< HEAD

=======
  */
  // Group histograms into pages of three
  for (size_t page = 0; page < constituents.size(); page += 3)
    {
      // Create a name for the canvas
      const std::string canvasName = "jetCsts_" + m_mapTrigToTag[trigToDraw] + "_" +
	m_mapResToTag[resToDraw] + "_page" + std::to_string(page / 3 + 1);

      // Extract the histograms for this page
      std::vector<TH1*> pageHists(constituents.begin() + page, 
				  constituents.begin() + std::min(page + 3, constituents.size()));

      // Create canvas and pads using MakeCanvas
      MakeCanvas(canvasName, pageHists.size(), m_vecCstCanvas.back(), m_vecCstRun.back());

      // Draw histograms on this canvas
      for (size_t pad = 0; pad < pageHists.size(); ++pad)
        {
	  m_vecCstCanvas.back().back()->cd(pad + 1); // Go to the specific pad
	  if (pageHists[pad])
            {
	      pageHists[pad]->Draw("HIST");
            }
        }

      // Update the canvas after drawing all histograms
      m_vecCstCanvas.back().back()->Update();
    }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
  // return w/o error
  return 0;
}

int JetDraw::DrawJetKinematics(const uint32_t trigToDraw, const JetRes resToDraw)
{
  // Use resToDraw to select histograms or adjust drawing logic
  switch (resToDraw) {
  case R02:
    if (m_do_debug) std::cout << "Resolution R02" << std::endl;
    break;
  case R03:
    if (m_do_debug) std::cout << "Resolution R03" << std::endl;
    break;
  case R04:
    if (m_do_debug) std::cout << "Resolution R04" << std::endl;
    break;
  case R05:
    if (m_do_debug) std::cout << "Resolution R05" << std::endl;
    break;
  default:
    std::cerr << "Unknown resolution" << std::endl;
    return -1;
  }

  // for kinematic hist names
  const std::string kineHistPrefix = std::string(m_kinematic_prefix) + "_" + m_mapTrigToTag[trigToDraw] + "_" + std::string(m_jet_type);
  const std::string kineProfSuffix = m_mapResToTag[resToDraw] + "_pfx";

  // grab relevant histograms from draw client
  QADrawClient *cl = QADrawClient::instance();

  TH2D *jetkinematiccheck_etavsphi = dynamic_cast<TH2D *>(cl->getHisto(kineHistPrefix + "_etavsphi_" + m_mapResToTag[resToDraw]));
  TH2D *jetkinematiccheck_jetmassvseta = dynamic_cast<TH2D *>(cl->getHisto(kineHistPrefix + "_jetmassvseta_" + m_mapResToTag[resToDraw]));
  TH2D *jetkinematiccheck_jetmassvspt = dynamic_cast<TH2D *>(cl->getHisto(kineHistPrefix + "_jetmassvspt_" + m_mapResToTag[resToDraw]));
  TH2D *jetkinematiccheck_spectra = dynamic_cast<TH2D *>(cl->getHisto(kineHistPrefix + "_spectra_" + m_mapResToTag[resToDraw]));

  TProfile *jetkinematiccheck_jetmassvseta_pfx = dynamic_cast<TProfile *>(cl->getHisto(kineHistPrefix + "_jetmassvseta_" + kineProfSuffix));
  TProfile *jetkinematiccheck_jetmassvspt_pfx = dynamic_cast<TProfile *>(cl->getHisto(kineHistPrefix + "_jetmassvspt_" + kineProfSuffix));

  // form canvas name & if it doesn't exist yet, create it
  const std::string kineCanName = "jetKinematics_" + m_mapTrigToTag[trigToDraw] + "_" + m_mapResToTag[resToDraw];
  if (!gROOT->FindObject(kineCanName.data()))
<<<<<<< HEAD
  {
    MakeCanvas(kineCanName, 4, m_vecKineCanvas.back(), m_vecKineRun.back());
  }

  m_vecKineCanvas.back().back()->cd(1);
  if (jetkinematiccheck_etavsphi)
  {
    jetkinematiccheck_etavsphi->SetTitle("Jet #eta vs #phi");
    jetkinematiccheck_etavsphi->SetXTitle("Jet #eta");
    jetkinematiccheck_etavsphi->SetYTitle("Jet #phi");
    jetkinematiccheck_etavsphi->SetZTitle("Counts");
    // jetkinematiccheck_etavsphi->GetXaxis()->SetNdivisions(505);
    // jetkinematiccheck_etavsphi->GetXaxis()->SetRangeUser(-1, 15);
    jetkinematiccheck_etavsphi->DrawCopy("COLZ");  // 2D Histogram
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecKineCanvas.back().back()->cd(2);
  if (jetkinematiccheck_jetmassvseta && jetkinematiccheck_jetmassvseta_pfx)
  {
    jetkinematiccheck_jetmassvseta->SetTitle("Jet Mass vs #eta");
    jetkinematiccheck_jetmassvseta->SetXTitle("Jet Mass [GeV/c^{2}]");
    jetkinematiccheck_jetmassvseta->SetYTitle("Jet #eta");
    jetkinematiccheck_jetmassvseta->SetZTitle("Counts");
    jetkinematiccheck_jetmassvseta->DrawCopy("COLZ");  // 2D Histogram
    jetkinematiccheck_jetmassvseta_pfx->SetTitle("Jet Mass vs #eta");
    jetkinematiccheck_jetmassvseta_pfx->SetXTitle("Jet Mass [GeV/c^{2}]");
    jetkinematiccheck_jetmassvseta_pfx->SetYTitle("Jet #eta");
    jetkinematiccheck_jetmassvseta_pfx->SetZTitle("Counts");
    jetkinematiccheck_jetmassvseta_pfx->DrawCopy("SAME");  // Profile
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecKineCanvas.back().back()->cd(3);
  if (jetkinematiccheck_jetmassvspt && jetkinematiccheck_jetmassvspt_pfx)
  {
    jetkinematiccheck_jetmassvspt->SetTitle("Jet Mass vs p_{T}");
    jetkinematiccheck_jetmassvspt->SetXTitle("Jet Mass [GeV/c^{2}]");
    jetkinematiccheck_jetmassvspt->SetYTitle("Jet p_{T} [GeV/c]");
    jetkinematiccheck_jetmassvspt->SetZTitle("Counts");
    jetkinematiccheck_jetmassvspt->DrawCopy("COLZ");  // 2D Histogram
    jetkinematiccheck_jetmassvspt_pfx->SetTitle("Jet Mass vs p_{T}");
    jetkinematiccheck_jetmassvspt_pfx->SetXTitle("Jet Mass [GeV/c^{2}]");
    jetkinematiccheck_jetmassvspt_pfx->SetYTitle("Jet p_{T} [GeV/c]");
    jetkinematiccheck_jetmassvspt_pfx->SetZTitle("Counts");
    jetkinematiccheck_jetmassvspt_pfx->DrawCopy("SAME");  // Profile
    gPad->UseCurrentStyle();
    gPad->Update();
    gPad->SetRightMargin(0.15);
  }
  else
  {
    // histogram is missing
    return -1;
  }

  m_vecKineCanvas.back().back()->cd(4);
  if (jetkinematiccheck_spectra)
  {
    jetkinematiccheck_spectra->SetTitle("Jet Spectra");
    jetkinematiccheck_spectra->SetXTitle("p_{T,Jet} [GeV/c]");
    jetkinematiccheck_spectra->SetYTitle("Counts");
    jetkinematiccheck_spectra->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    // histogram is missing
    return -1;
  }
=======
    {
      MakeCanvas(kineCanName, 4, m_vecKineCanvas.back(), m_vecKineRun.back());
    }

  m_vecKineCanvas.back().back()->cd(1);
  if (jetkinematiccheck_etavsphi)
    {
      jetkinematiccheck_etavsphi->SetTitle("Jet #eta vs #phi");
      jetkinematiccheck_etavsphi->SetXTitle("Jet #eta");
      jetkinematiccheck_etavsphi->SetYTitle("Jet #phi");
      jetkinematiccheck_etavsphi->SetZTitle("Counts");
      // jetkinematiccheck_etavsphi->GetXaxis()->SetNdivisions(505);
      // jetkinematiccheck_etavsphi->GetXaxis()->SetRangeUser(-1, 15);
      jetkinematiccheck_etavsphi->DrawCopy("COLZ");  // 2D Histogram
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecKineCanvas.back().back()->cd(2);
  if (jetkinematiccheck_jetmassvseta && jetkinematiccheck_jetmassvseta_pfx)
    {
      jetkinematiccheck_jetmassvseta->SetTitle("Jet Mass vs #eta");
      jetkinematiccheck_jetmassvseta->SetXTitle("Jet #eta");
      jetkinematiccheck_jetmassvseta->SetYTitle("Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvseta->SetZTitle("Counts");
      jetkinematiccheck_jetmassvseta->DrawCopy("COLZ");  // 2D Histogram
      jetkinematiccheck_jetmassvseta_pfx->SetTitle("Jet Mass vs #eta");
      jetkinematiccheck_jetmassvseta_pfx->SetXTitle("Jet #eta");
      jetkinematiccheck_jetmassvseta_pfx->SetYTitle("Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvseta_pfx->SetZTitle("Counts");
      jetkinematiccheck_jetmassvseta_pfx->DrawCopy("SAME");  // Profile
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecKineCanvas.back().back()->cd(3);
  if (jetkinematiccheck_jetmassvspt && jetkinematiccheck_jetmassvspt_pfx)
    {
      jetkinematiccheck_jetmassvspt->SetTitle("Jet Mass vs p_{T}");
      jetkinematiccheck_jetmassvspt->SetXTitle("Jet p_{T} [GeV/c]");
      jetkinematiccheck_jetmassvspt->SetYTitle("Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvspt->SetZTitle("Counts");
      jetkinematiccheck_jetmassvspt->DrawCopy("COLZ");  // 2D Histogram
      jetkinematiccheck_jetmassvspt_pfx->SetTitle("Jet Mass vs p_{T}");
      jetkinematiccheck_jetmassvspt_pfx->SetXTitle("Jet p_{T} [GeV/c]");
      jetkinematiccheck_jetmassvspt_pfx->SetYTitle("Jet Mass [GeV/c^{2}]");
      jetkinematiccheck_jetmassvspt_pfx->SetZTitle("Counts");
      jetkinematiccheck_jetmassvspt_pfx->DrawCopy("SAME");  // Profile
      gPad->UseCurrentStyle();
      gPad->Update();
      gPad->SetRightMargin(0.15);
    }
  else
    {
      // histogram is missing
      return -1;
    }

  m_vecKineCanvas.back().back()->cd(4);
  if (jetkinematiccheck_spectra)
    {
      jetkinematiccheck_spectra->SetTitle("Jet Spectra");
      jetkinematiccheck_spectra->SetXTitle("p_{T,Jet} [GeV/c]");
      jetkinematiccheck_spectra->SetYTitle("Counts");
      jetkinematiccheck_spectra->DrawCopy("HIST");  // 1D Histogram
      gPad->UseCurrentStyle();
      gPad->SetLogy();
    }
  else
    {
      // histogram is missing
      return -1;
    }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized 
  PrintRun.SetTextAlign(23);  // center/top alignment

  // Generate run string from client
  std::ostringstream runnostream;
  runnostream << cl->RunNumber() << ", build " << cl->build();

  // prepend module name, component, trigger, and resolution
  std::string runstring = Name();
  runstring += "_JetKinematics_";
  runstring += m_mapTrigToName[trigToDraw];
  runstring += "_";
  runstring += m_mapResToName[resToDraw];
  runstring += " Run ";
  runstring += runnostream.str();

  // add runstring to relevant pad
  m_vecKineRun.back().back()->cd();
  PrintRun.DrawText(0.5, 0.95, runstring.data());  // Adjust coordinates as needed
  m_vecKineCanvas.back().back()->Update();  // Update the pad

  // return w/o error
  return 0;
}

int JetDraw::DrawJetSeed(const uint32_t trigToDraw, const JetRes resToDraw)
{
  // Use resToDraw to select histograms or adjust drawing logic                                                                           
  switch (resToDraw) {
  case R02:
    if (m_do_debug) std::cout << "Resolution R02" << std::endl;
    break;
  case R03:
    if (m_do_debug) std::cout << "Resolution R03" << std::endl;
    break;
  case R04:
    if (m_do_debug) std::cout << "Resolution R04" << std::endl;
    break;
  case R05:
    if (m_do_debug) std::cout << "Resolution R05" << std::endl;
    break;
  default:
    std::cerr << "Unknown resolution" << std::endl;
    return -1;
  }

  // for seed hist names
  const std::string seedHistName = std::string(m_seed_prefix) + "_" + m_mapTrigToTag[trigToDraw] + "_" + std::string(m_jet_type) + "_" + m_mapResToTag[resToDraw];

  // grab relevant histograms from draw client
  QADrawClient *cl = QADrawClient::instance();

  TH2F *jetseedcount_rawetavsphi = dynamic_cast<TH2F *>(cl->getHisto(seedHistName + "_rawetavsphi"));
  TH1F *jetseedcount_rawpt = dynamic_cast<TH1F *>(cl->getHisto(seedHistName + "_rawpt"));
  TH1F *jetseedcount_rawptall = dynamic_cast<TH1F *>(cl->getHisto(seedHistName + "_rawptall"));
  TH1F *jetseedcount_rawseedcount = dynamic_cast<TH1F *>(cl->getHisto(seedHistName + "_rawseedcount"));
  TH2F *jetseedcount_subetavsphi = dynamic_cast<TH2F *>(cl->getHisto(seedHistName + "_subetavsphi"));
  TH1F *jetseedcount_subpt = dynamic_cast<TH1F *>(cl->getHisto(seedHistName + "_subpt"));
  TH1F *jetseedcount_subptall = dynamic_cast<TH1F *>(cl->getHisto(seedHistName + "_subptall"));
  TH1F *jetseedcount_subseedcount = dynamic_cast<TH1F *>(cl->getHisto(seedHistName + "_subseedcount"));

  // form canvas name & if it doesn't exist yet, create it
  const std::string seedCanName = "jetSeeds_" + m_mapTrigToTag[trigToDraw] + "_" + m_mapResToTag[resToDraw];
  if (!gROOT->FindObject(seedCanName.data()))
<<<<<<< HEAD
  {
    MakeCanvas(seedCanName, 8, m_vecSeedCanvas.back(), m_vecSeedRun.back());
  }

  m_vecSeedCanvas.back().back()->cd(1);
  if (jetseedcount_rawetavsphi)
  {
=======
    {
      MakeCanvas(seedCanName, 8, m_vecSeedCanvas.back(), m_vecSeedRun.back());
    }

  m_vecSeedCanvas.back().back()->cd(1);
  if (jetseedcount_rawetavsphi)
    {
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
      jetseedcount_rawetavsphi->SetLineColor(kBlue);
      jetseedcount_rawetavsphi->GetXaxis()->SetRangeUser(0.0, 12000);
      jetseedcount_rawetavsphi->SetTitle("Raw Seed #eta vs #phi");
      jetseedcount_rawetavsphi->SetXTitle("Jet #eta_{raw} [Rads.]");
      jetseedcount_rawetavsphi->SetYTitle("Jet #phi_{raw} [Rads.]");
      jetseedcount_rawetavsphi->SetZTitle("Counts");
      // jetseedcount_rawetavsphi->GetXaxis()->SetNdivisions(505);
      jetseedcount_rawetavsphi->DrawCopy("COLZ");  // 2D Histogram
      gPad->UseCurrentStyle();
  }
  else
  {
    // histogram missing
    return -1;
  }

  m_vecSeedCanvas.back().back()->cd(2);
  if (jetseedcount_rawpt)
  {
    jetseedcount_rawpt->SetTitle("Raw p_{T}");
    jetseedcount_rawpt->SetXTitle("Jet p_{T,Raw} [GeV]");
    jetseedcount_rawpt->SetYTitle("Counts");
    jetseedcount_rawpt->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    // histogram missing
    return -1;
  }

  m_vecSeedCanvas.back().back()->cd(3);
  if (jetseedcount_rawptall)
  {
    jetseedcount_rawptall->SetTitle("Raw p_{T} (all jet seeds)");
    jetseedcount_rawptall->SetXTitle("Jet p_{T,all seed} [GeV]");
    jetseedcount_rawptall->SetYTitle("Counts");
    jetseedcount_rawptall->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    // histogram missing
    return -1;
  }

  m_vecSeedCanvas.back().back()->cd(4);
  if (jetseedcount_rawseedcount)
  {
    jetseedcount_rawseedcount->SetTitle("Raw Seed Count per Event");
    jetseedcount_rawseedcount->SetXTitle("Raw Seed Count per Event");
    jetseedcount_rawseedcount->SetYTitle("Counts");
    jetseedcount_rawseedcount->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    // histogram missing
    return -1;
  }

  m_vecSeedCanvas.back().back()->cd(5);
  if (jetseedcount_subetavsphi)
  {
    jetseedcount_subetavsphi->SetLineColor(kBlue);
    // jetseedcount_subetavsphi->GetXaxis()->SetRangeUser(0.0, 12000);
    jetseedcount_subetavsphi->SetTitle("Sub-seed #eta vs #phi");
    jetseedcount_subetavsphi->SetXTitle("Jet #eta_{subseed} [Rads.]");
    jetseedcount_subetavsphi->SetYTitle("Jet #phi_{subseed} [Rads.]");
    jetseedcount_subetavsphi->SetZTitle("Counts");
    // jetseedcount_subetavsphi->GetXaxis()->SetNdivisions(505);
    jetseedcount_subetavsphi->DrawCopy("COLZ");  // 2D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    return -1;
  }

  m_vecSeedCanvas.back().back()->cd(6);
  if (jetseedcount_subpt)
  {
    jetseedcount_subpt->SetTitle("Sub. p_{T}");
    jetseedcount_subpt->SetXTitle("Jet p_{T,sub.} [GeV]");
    jetseedcount_subpt->SetYTitle("Counts");
    jetseedcount_subpt->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    return -1;
  }

  m_vecSeedCanvas.back().back()->cd(7);
  if (jetseedcount_subptall)
  {
    jetseedcount_subptall->SetTitle("Sub. p_{T} (all jet seeds)");
    jetseedcount_subptall->SetXTitle("Jet p_{T, all-sub.} [GeV]");
    jetseedcount_subptall->SetYTitle("Counts");
    jetseedcount_subptall->DrawCopy("HIST");  // 1D Histogram
    gPad->UseCurrentStyle();
  }
  else
  {
    return -1;
<<<<<<< HEAD
  }

  m_vecSeedCanvas.back().back()->cd(8);
  if (jetseedcount_subseedcount)
  {
    jetseedcount_subseedcount->SetTitle("Sub Seed Count per Event");
    jetseedcount_subseedcount->SetXTitle("Sub Seed Count per Event");
    jetseedcount_subseedcount->SetYTitle("Counts");
    jetseedcount_subseedcount->DrawCopy("");
    gPad->UseCurrentStyle();
  }
=======
}

  m_vecSeedCanvas.back().back()->cd(8);
  if (jetseedcount_subseedcount)
    {
  jetseedcount_subseedcount->SetTitle("Sub Seed Count per Event");
  jetseedcount_subseedcount->SetXTitle("Sub Seed Count per Event");
  jetseedcount_subseedcount->SetYTitle("Counts");
  jetseedcount_subseedcount->DrawCopy("");
  gPad->UseCurrentStyle();
}
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();  // set to normalized
  PrintRun.SetTextAlign(23);  // center/top alignment

  // Generate run string from client
  std::ostringstream runnostream;
  runnostream << cl->RunNumber() << ", build " << cl->build();

  // prepend module name, component, trigger, and resolution
  std::string runstring = Name();
  runstring += "_JetSeeds_";
  runstring += m_mapTrigToName[trigToDraw];
  runstring += "_";
  runstring += m_mapResToName[resToDraw];
  runstring += " Run ";
  runstring += runnostream.str();

  // add runstring to relevant pad
  m_vecSeedRun.back().back()->cd();
  PrintRun.DrawText(0.5, 0.95, runstring.data());  // Adjust coordinates as needed
  m_vecSeedCanvas.back().back()->Update();  // Update the pad

  // return w/o error
  return 0;
}

<<<<<<< HEAD
int JetDraw::DBVarInit()
{
=======
 int JetDraw::DBVarInit()
 {
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
  /* db = new QADrawDB(this); */
  /* db->DBInit(); */
  return 0;
}

<<<<<<< HEAD
void JetDraw::SetJetSummary(TCanvas* c)
{
   jetSummary = c;
   jetSummary->cd();

   QADrawClient *cl = QADrawClient::instance();
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
   runnostream << Name() << "_jet_summary Run " << cl->RunNumber() << ", build " << cl->build();
   runstring = runnostream.str();
   tr->cd();
   PrintRun.DrawText(0.5, 1., runstring.c_str());
   jetSummary->Update();
}

void JetDraw::SetDoDebug(const bool debug)
{
  m_do_debug = debug;
}

// ----------------------------------------------------------------------------
//! Save canvases to file
// ----------------------------------------------------------------------------
/*! Helper method to save all canvases to
 *  a specified file. This is useful for
 *  debugging and quick testing when
 *  adjusting plotting details/etc.
 */
void JetDraw::SaveCanvasesToFile(TFile* file)
{

  // emit debugging message
  if (m_do_debug)
  {
    std::cout << "Saving plots to file:\n"
              << "  " << file -> GetName()
              << std::endl;
  }

  // check if you can cd into file
  //   - if not, exit
  const bool isGoodCD = file -> cd();
  if (!isGoodCD)
  {
    if (m_do_debug)
    {
      std::cerr << PHWHERE << "WARNING: couldn't cd into output file!" << std::endl;
    }
    return;
  }

  // for tracking how many plots were saved
  std::size_t nWrite = 0;

  // save rho canvases
  for (auto rho : m_vecRhoCanvas)
  {
    rho -> Draw();
    rho -> Write();
    ++nWrite;
  }
  if (m_do_debug) std::cout << "  -- Saved rho plots." << std::endl;

  // save constituent canvases
  for (auto cstRow : m_vecCstCanvas)
  {
    for (auto cst : cstRow)
    {
      cst -> Draw();
      cst -> Write();
      ++nWrite;
    }
  }
  if (m_do_debug) std::cout << "  -- Saved constituent plots." << std::endl;

  // save kinematics canvases
  for (auto kinRow : m_vecKineCanvas)
  {
    for (auto kin : kinRow)
    {
      kin -> Draw();
      kin -> Write();
      ++nWrite;
    }
  }
  if (m_do_debug) std::cout << "  -- Saved kinematic plots." << std::endl;

  // save seed canvases
  for (auto sedRow : m_vecSeedCanvas)
  {
    for (auto sed : sedRow)
    {
      sed -> Draw();
      sed -> Write();
      ++nWrite;
    }
  }
  if (m_do_debug) std::cout << "  -- Saved seed plots." << std::endl;

  // announce how many plots saved & exit
  if (m_do_debug)
  {
    std::cout << "Finished saving plots: " << nWrite << " plots written." << std::endl;
  }
  return;

}  // end 'SaveCanvasesToFile(TFile*)'

void JetDraw::myText(double x, double y, int color, const char *text, double tsize)
{
  TLatex l;
  l.SetTextAlign(22);
  l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextColor(color);
  l.DrawLatex(x, y, text);
}
=======
 void JetDraw::SetJetSummary(TCanvas* c)
 {
  jetSummary = c;
  jetSummary->cd();

  QADrawClient *cl = QADrawClient::instance();
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
  runnostream << Name() << "_jet_summary Run " << cl->RunNumber() << ", build " << cl->build();
  runstring = runnostream.str();
  tr->cd();
  PrintRun.DrawText(0.5, 0.95, runstring.c_str());
  jetSummary->Update();
}

 void JetDraw::SetDoDebug(const bool debug)
 {
  m_do_debug = debug;
}

 // ----------------------------------------------------------------------------
 //! Save canvases to file
 // ----------------------------------------------------------------------------
 /*! Helper method to save all canvases to
 *  a specified file. This is useful for
 *  debugging and quick testing when
  adjusting plotting details/etc.
 */
 void JetDraw::SaveCanvasesToFile(TFile* file)
 {
   
   // emit debugging message
   if (m_do_debug)
     {
       std::cout << "Saving plots to file:\n"
		 << "  " << file -> GetName()
		 << std::endl;
     }
   
   // check if you can cd into file
   //   - if not, exit
   const bool isGoodCD = file -> cd();
   if (!isGoodCD)
     {
       if (m_do_debug)
	 {
	   std::cerr << PHWHERE << "WARNING: couldn't cd into output file!" << std::endl;
	 }
       return;
     }
   
   // for tracking how many plots were saved
   std::size_t nWrite = 0;
   
   // save rho canvases
   for (auto rho : m_vecRhoCanvas)
     {
       rho -> Draw();
       rho -> Write();
       ++nWrite;
     }
   if (m_do_debug) std::cout << "  -- Saved rho plots." << std::endl;
   
   // save constituent canvases
   for (auto cstRow : m_vecCstCanvas)
     {
       for (auto cst : cstRow)
	 {
	   cst -> Draw();
	   cst -> Write();
	   ++nWrite;
	 }
     }
   if (m_do_debug) std::cout << "  -- Saved constituent plots." << std::endl;
   
   // save kinematics canvases
   for (auto kinRow : m_vecKineCanvas)
     {
       for (auto kin : kinRow)
	 {
	   kin -> Draw();
	   kin -> Write();
	   ++nWrite;
	 }
     }
   if (m_do_debug) std::cout << "  -- Saved kinematic plots." << std::endl;
   
   // save seed canvases
   for (auto sedRow : m_vecSeedCanvas)
     {
       for (auto sed : sedRow)
	 {
	   sed -> Draw();
	   sed -> Write();
	   ++nWrite;
	 }
     }
  if (m_do_debug) std::cout << "  -- Saved seed plots." << std::endl;
  
  // announce how many plots saved & exit
  if (m_do_debug)
    {
      std::cout << "Finished saving plots: " << nWrite << " plots written." << std::endl;
    }
  return;
  
 }  // end 'SaveCanvasesToFile(TFile*)'
 
 void JetDraw::myText(double x, double y, int color, const char *text, double tsize)
 {
   TLatex l;
   l.SetTextAlign(22);
   l.SetTextSize(tsize);
   l.SetNDC();
   l.SetTextColor(color);
   l.DrawLatex(x, y, text);
 }
>>>>>>> 28b109c (updates to JetDraw.cc and JetDraw.h)
