#ifndef GLOBALQADRAW_H__
#define GLOBALQADRAW_H__

#include <qahtml/QADraw.h>

#include <vector>

class QADrawDB;
class QADrawDBVar;
class TCanvas;
class TPad;

class GlobalQADraw : public QADraw
{
 public:
  GlobalQADraw(const std::string &name = "GlobalQA");
  ~GlobalQADraw() override;

  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int DBVarInit();

 protected:
  int MakeCanvas(const std::string &name, int num);
  int DrawMBD(const std::string &what = "ALL");
  int DrawZDC(const std::string &what = "ALL");
  int DrawsEPD(const std::string &what = "ALL");

  QADrawDB *db{nullptr};
  TCanvas *TC[3]{};
  TPad *transparent[3]{};
  TPad *Pad[3][7]{};
  int run_type = 0; // AuAu= 0 , pp=1
  
};

#endif
