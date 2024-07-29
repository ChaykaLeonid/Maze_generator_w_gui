#include "controller.h"

s21::Controller::Controller() { model_ = new s21::Model; }
s21::Controller::~Controller() { delete model_; }

bool s21::Controller::ContainsErrors() { return model_->ContainsErrors(); }

void s21::Controller::GenerateMaze(size_t rows, size_t columns) {
  model_->GenerateMaze(rows, columns);
}

void s21::Controller::MakePath(const room &start, const room &end,
                               vector<room> &path) {
  model_->FindPath(start, end, path);
}

size_t s21::Controller::GetRows() { return model_->GetRows(); }

size_t s21::Controller::GetColumns() { return model_->GetColumns(); }

walls s21::Controller::GetMazeBorders(size_t i, size_t j) {
  return model_->operator()(i, j);
}

void s21::Controller::Save(string filename) { model_->Export(filename); }

bool s21::Controller::Load(string filename) { return model_->Import(filename); }
