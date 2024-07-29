#ifndef VIEW_H
#define VIEW_H

#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <QPainter>

#include "controller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

class View : public QMainWindow {
  Q_OBJECT

 public:
  View(QWidget *parent = nullptr);
  ~View();

 private slots:
  void on_generate_button_clicked();
  void on_path_button_clicked();
  void on_import_button_clicked();
  void on_export_button_clicked();

 private:
  QString file_;
  Ui::View *ui;
  QGraphicsScene *scene_;
  QPen *pen_;
  s21::Controller *controller_;
  size_t rows_ = 0;
  size_t columns_ = 0;

  void DrawMaze();
  void DrawPath(const vector<room> &path);
  void Import(std::string filename);
  void Export(std::string filename);
  void GetSize();
  void SetMaximumCells();
};
#endif  // VIEW_H
