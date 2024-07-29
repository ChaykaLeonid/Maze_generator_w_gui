#include "view.h"

#include "ui_view.h"

View::View(QWidget *parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);
  scene_ = new QGraphicsScene();
  ui->graphics_view->setScene(scene_);
  controller_ = new s21::Controller();
  pen_ = new QPen();

  pen_->setColor(Qt::black);
  pen_->setWidth(2);
  ui->graphics_view->centerOn(0, 0);

  ui->graphics_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->graphics_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scene_->setBackgroundBrush(Qt::white);
}

View::~View() {
  delete pen_;
  delete scene_;
  delete ui;
}

void View::on_generate_button_clicked() {
  rows_ = ui->heigth_spinbox->value();
  columns_ = ui->width_spinbox->value();
  controller_->GenerateMaze(rows_, columns_);
  if (!controller_->ContainsErrors()) {
    DrawMaze();
    SetMaximumCells();
  } else {
    ui->statusbar->showMessage("Bad Maze", 2500);
  }
}

void View::on_path_button_clicked() {
  if (!controller_->ContainsErrors()) {
    vector<room> path;
    room start(ui->start_x_spinbox->value() - 1,
               ui->start_y_spinbox->value() - 1);
    room end(ui->finish_x_spinbox->value() - 1,
             ui->finish_y_spinbox->value() - 1);

    GetSize();
    controller_->MakePath(start, end, path);
    DrawPath(path);
  } else {
    ui->statusbar->showMessage("Bad Maze", 2500);
  }
}

void View::on_import_button_clicked() {
  rows_ = columns_ = 0;
  scene_->clear();
  file_ = QFileDialog::getOpenFileName(this, "Select File", "",
                                       tr("Text Files (*.txt)"));
  if (file_ != "") {
    if (controller_->Load(file_.toStdString()) &&
        (!controller_->ContainsErrors())) {
      GetSize();
      SetMaximumCells();
      DrawMaze();

    } else {
      ui->statusbar->showMessage("Bad Maze", 2500);
    }
  }
}

void View::on_export_button_clicked() {
  if (rows_ > 0 && columns_ > 0) {
    file_ = QFileDialog::getSaveFileName(this, "Don't forget extension", "",
                                         tr("Text Files (*.txt)"));
    if (file_ != "") {
      controller_->Save(file_.toStdString());
    }
  } else {
    ui->statusbar->showMessage("Empty Maze", 2500);
  }
}

void View::DrawMaze() {
  scene_->clear();
  int px_x = 0, px_y = 0;
  int step_x = 500 / columns_;
  int step_y = 500 / rows_;

  for (size_t i = 0; i < rows_; i++) {
    for (size_t j = 0; j < columns_; j++) {
      walls wall = controller_->GetMazeBorders(i + 1, j + 1);
      if (wall.first && j != columns_ - 1) {
        if (i == rows_ - 1)
          scene_->addLine(px_x + step_x, px_y, px_x + step_x, 499, *pen_);
        else
          scene_->addLine(px_x + step_x, px_y, px_x + step_x, px_y + step_y,
                          *pen_);
      }
      if (wall.second && i != rows_ - 1) {
        if (j == columns_ - 1)
          scene_->addLine(px_x, px_y + step_y, 499, px_y + step_y, *pen_);
        else
          scene_->addLine(px_x, px_y + step_y, px_x + step_x, px_y + step_y,
                          *pen_);
      }
      px_x += step_x;
    }
    px_x = 0;
    px_y += step_y;
  }

  scene_->addRect(0, 0, 499, 499, *pen_);
}

void View::DrawPath(const vector<room> &path) {
  scene_->clear();
  DrawMaze();
  int step_x = 500 / columns_;
  int step_y = 500 / rows_;
  pen_->setColor(Qt::red);

  for (auto it = path.cbegin(); it != path.cend() - 1; ++it) {
    int x_first = it->first * step_x + step_x / 2;
    int y_first = it->second * step_y + step_y / 2;
    int x_second = (it + 1)->first * step_x + step_x / 2;
    int y_second = (it + 1)->second * step_y + step_y / 2;
    scene_->addLine(x_first, y_first, x_second, y_second, *pen_);
  }
  pen_->setColor(Qt::black);
}

void View::GetSize() {
  rows_ = controller_->GetRows();
  columns_ = controller_->GetColumns();
}

void View::SetMaximumCells() {
  ui->start_x_spinbox->setMaximum(rows_);
  ui->start_x_spinbox->setValue(1);
  ui->start_y_spinbox->setMaximum(columns_);
  ui->start_y_spinbox->setValue(1);
  ui->finish_x_spinbox->setMaximum(rows_);
  ui->finish_x_spinbox->setValue(1);
  ui->finish_y_spinbox->setMaximum(columns_);
  ui->finish_y_spinbox->setValue(1);
}
