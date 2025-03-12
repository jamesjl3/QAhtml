#ifndef MICROMEGAS_MICROMEGASDRAW_H__
#define MICROMEGAS_MICROMEGASDRAW_H__

#include <qahtml/QADraw.h>

class TCanvas;
class TPad;
class TH1;
class TH2;

class MicromegasDraw : public QADraw
{
 public:
  MicromegasDraw(const std::string &name = "MicromegasQA");

  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int DBVarInit();

 private:
  int MakeCanvas(const std::string &name, int num);
  TH1* ClusterAverage(TH2*, std::string);
  int DrawClusterInfo();
  int DrawBCOInfo();
  TCanvas *TC[4]{};
  TPad *transparent[4]{};
  TPad *Pad[4][4]{};
};

#endif
