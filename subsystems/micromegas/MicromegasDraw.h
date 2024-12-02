#ifndef MICROMEGAS_MICROMEGASDRAW_H__
#define MICROMEGAS_MICROMEGASDRAW_H__

#include <qahtml/QADraw.h>

#include <vector>

class QADB;
class QADBVar;
class TCanvas;
class TGraphErrors;
class TPad;
class TH1F;
class TH2F;
class TGraph;
class TLegend;

class MicromegasDraw : public QADraw
{
 public: 
  MicromegasDraw(const std::string &name = "MicromegasQA");
  ~MicromegasDraw() override;

  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int DBVarInit();

 private:
  int MakeCanvas(const std::string &name, int num);
  TH1F* ClusterAverage(TH2F*, std::string);
  int BinValues(TH1F*);
  int DrawTileInfo();
  int DrawClusterInfo();
  int DrawBCOInfo();
  TCanvas *TC[4]{};
  TPad *transparent[4]{};
  TPad *Pad[4][4]{};
};

#endif
