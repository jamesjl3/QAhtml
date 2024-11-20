#ifndef JET_DRAW_H
#define JET_DRAW_H

#include <qahtml/QADrawClient.h>
#include <qahtml/QADraw.h>
#include <jetqa/JetQADefs.h>
#include <map>
#include <utility>
#include <vector>

class QADB;
class QADBVar;
class TCanvas;
class TFile;
class TGraphErrors;
class TH1;
class TH2;
class TPad;

// aliases for convenience
using VPad1D    = std::vector<TPad*>;
using VPad2D    = std::vector<std::vector<TPad*>>;
using VCanvas1D = std::vector<TCanvas*>;
using VCanvas2D = std::vector<std::vector<TCanvas*>>;

class JetDraw : public QADraw
{
 public:

  // tags for jet resolutions
  enum JetRes
  {
    R02,
    R03,
    R04,
    R05
  };

  JetDraw(const std::string &name = "JetQA");
  ~JetDraw() override;
  int MakeHtml(const std::string &what = "ALL") override;
  int Draw(const std::string &what = "ALL") override;
  int DBVarInit();
  void SetJetSummary(TCanvas* c);
  void SetDoDebug(const bool debug);
  void SaveCanvasesToFile(TFile* file);

 private:
  int MakeCanvas(const std::string &name, const int nHist, VCanvas1D& canvas, VPad1D& run);
  int DrawRho(uint32_t trigger);
  int DrawConstituents(uint32_t trigToDraw, JetRes resToDraw);
  int DrawJetKinematics(uint32_t trigger, JetRes reso);
  int DrawJetSeed(uint32_t trigger, JetRes reso);
  void myText(double x, double y, int color, const char *text, double tsize = 0.04);

  TCanvas* jetSummary = nullptr;

  // canvases for drawing
  VCanvas1D m_vecRhoCanvas;
  VCanvas2D m_vecCstCanvas;
  VCanvas2D m_vecKineCanvas;
  VCanvas2D m_vecSeedCanvas;

  // for adding run numbers to canvases
  VPad1D m_vecRhoRun;
  VPad2D m_vecCstRun;
  VPad2D m_vecKineRun;
  VPad2D m_vecSeedRun;

  // turn debugging statements on/off
  bool m_do_debug;

  const char* m_constituent_prefix;
  const char* m_rho_prefix;
  const char* m_kinematic_prefix;
  const char* m_seed_prefix;

  // jet type (TODO will need to expand this to a vector in the future)
  const char* m_jet_type;

  // triggers we want to draw
  std::vector<uint32_t> m_vecTrigToDraw = {
    JetQADefs::GL1::MBDNSJet1,
    JetQADefs::GL1::MBDNSJet2,
    JetQADefs::GL1::MBDNSJet3,
    JetQADefs::GL1::MBDNSJet4
  };

  // resolutions we want to draw
  std::vector<uint32_t> m_vecResToDraw = {
    JetRes::R02,
    JetRes::R03,
    JetRes::R04,
    JetRes::R05
  };

  // map of resolution index to name
  std::map<uint32_t, std::string> m_mapResToName = {
    {JetRes::R02, "Res02"},
    {JetRes::R03, "Res03"},
    {JetRes::R04, "Res04"},
    {JetRes::R05, "Res05"}
  };

  //map res to Tag
  std::map<uint32_t, std::string> m_mapResToTag = {
    {JetRes::R02, "r02"},
    {JetRes::R03, "r03"},
    {JetRes::R04, "r04"},
    {JetRes::R05, "r05"}
  };

  // map of trigger index onto name
  std::map<uint32_t, std::string> m_mapTrigToName = {
    {JetQADefs::GL1::MBDNSJet1, "JetCoin6GeV"},
    {JetQADefs::GL1::MBDNSJet2, "JetCoin8GeV"},
    {JetQADefs::GL1::MBDNSJet3, "JetCoin10GeV"},
    {JetQADefs::GL1::MBDNSJet4, "JetCoin12GeV"}
  };

  // map trig to tag
  std::map<uint32_t, std::string> m_mapTrigToTag = {
    {JetQADefs::GL1::MBDNS1, "mbdns1"},
    {JetQADefs::GL1::MBDNSJet1, "mbdnsjet1"},
    {JetQADefs::GL1::MBDNSJet2, "mbdnsjet2"},
    {JetQADefs::GL1::MBDNSJet3, "mbdnsjet3"},
    {JetQADefs::GL1::MBDNSJet4, "mbdnsjet4"}
  };
};
#endif
