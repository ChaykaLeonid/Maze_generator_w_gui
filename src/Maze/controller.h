#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

namespace s21 {
class Controller {
 public:
  Controller();
  ~Controller();
  bool ContainsErrors();
  void GenerateMaze(size_t rows, size_t columns);
  void MakePath(const room &start, const room &end, vector<room> &path);
  size_t GetRows();
  size_t GetColumns();
  walls GetMazeBorders(size_t i, size_t j);
  void Save(std::string filename);
  bool Load(std::string filename);

 private:
  s21::Model *model_;
};
}  // namespace s21

#endif  // CONTROLLER_H
