#ifndef MODEL_H
#define MODEL_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <stack>
#include <utility>
#include <vector>

using std::make_pair;
using std::pair;
using std::set;
using std::stack;
using std::string;
using std::vector;
using walls = pair<bool, bool>;  // first -> right, second -> bottom
using room = pair<int, int>;

namespace s21 {
class Model {
 public:
  Model() {}
  Model(int rows, int columns) { GenerateMaze(rows, columns); }
  ~Model() {
    if (cells_) delete cells_;
  }

  int GetRows() { return rows_; }
  int GetColumns() { return columns_; }

  walls operator()(int row, int column) {
    return cells_->operator[]((row - 1) * columns_ + (column - 1));
  }

  void GenerateMaze(int rows, int columns) {
    SetSize(rows, columns);
    vector<int> sets_in_row(columns_);

    for (int j = 0; j < rows_; ++j) {
      AssignUniqueSet(sets_in_row);
      AddingVerticalWalls(sets_in_row, j);
      AddingHorizontalWalls(sets_in_row, j);
      if (j != rows_ - 1) PreparingNextLine(sets_in_row, j);
    }
    FixEndLine(sets_in_row);
  }

  void FindPath(const room &start, const room &finish, vector<room> &path) {
    bool find_flag = true;
    bool loop_flag = false;
    bool found = false;
    set<room> empty;
    MazeCrawler(empty, start.first, start.second, 0, loop_flag, path, find_flag,
                found, finish.first, finish.second);
  }

  bool Import(string path_to_file) {
    string last_stream_move;
    bool status = true;
    std::ifstream in_file(path_to_file);
    if (in_file.fail()) {
      status = false;
    } else {
      rows_ = columns_ = 0;
      in_file >> rows_;
      in_file >> columns_;
      if (rows_ > 50 || columns_ > 50) status = false;
      if (status) {
        SetSize(rows_, columns_);
        if (!ImportCells(in_file, true)) status = false;
        if (!ImportCells(in_file, false)) status = false;
        in_file >> last_stream_move;
        if (!in_file.eof()) status = false;
      }
      in_file.close();
    };

    return status;
  }

  bool Export(string path_to_file) {
    bool status = true;
    std::ofstream out_file(path_to_file);
    if (out_file.fail()) {
      status = false;
    } else {
      out_file << rows_ << " " << columns_ << "\n";
      ExportCells(out_file, true);
      out_file << "\n";
      ExportCells(out_file, false);
      out_file.close();
    }
    return status;
  }

  bool ContainsErrors() {
    size_t cells_count = rows_ * columns_;
    bool contain_errors = false;
    set<room> cells;
    vector<room> empty;
    bool find_flag = false;
    bool found = false;

    for (int direction = 1; cells.empty() && direction < 5; ++direction)
      MazeCrawler(cells, 0, 0, direction, contain_errors, empty, find_flag,
                  found);
    if (cells_count != cells.size()) contain_errors = true;

    return contain_errors;
  }

 private:
  int rows_ = 0;
  int columns_ = 0;
  vector<walls> *cells_ = nullptr;

  void SetSize(int rows, int columns) {
    rows_ = rows;
    columns_ = columns;
    int elements = rows * columns;
    if (cells_) delete cells_;
    cells_ = new vector<walls>(elements);
    for (auto it = cells_->begin(); it != cells_->end(); ++it) {
      it->first = false;
      it->second = false;
    }
  }

  void ExportCells(std::ostream &output, bool right) {
    for (size_t i = 1; i < cells_->size() + 1; ++i) {
      if (right)
        output << cells_->at(i - 1).first;
      else
        output << cells_->at(i - 1).second;
      if (i % columns_)
        output << " ";
      else
        output << "\n";
    }
  }

  bool ImportCells(std::ifstream &input, bool right) {
    string check_string = "10";
    bool status = true;
    string wall_exists = ".";
    for (int i = 0; i < rows_ * columns_; ++i) {
      if (input.eof()) status = false;
      input >> wall_exists;
      if (wall_exists.find_first_not_of(check_string) != string::npos) {
        status = false;
      } else {
        if (right)
          cells_->at(i).first = std::stod(wall_exists);
        else
          cells_->at(i).second = std::stod(wall_exists);
      }
    }
    return status;
  }

  void AssignUniqueSet(vector<int> &sets) {
    static unsigned int counter = 1;
    for (auto it = sets.begin(); it != sets.end(); ++it) {
      if (*it == 0) {
        *it = counter;
        ++counter;
      }
    }
  }

  void AddingVerticalWalls(vector<int> &sets, int row) {
    bool adding_wall;
    for (int i = 0; i < columns_ - 1; ++i) {
      int iter = row * columns_ + i;
      adding_wall = RandomBool();
      if (adding_wall || sets[i] == sets[i + 1]) {
        cells_->at(iter).first = true;
      } else {
        MergeSets(sets, sets[i + 1], sets[i]);
      }
    }
    cells_->at((row + 1) * columns_ - 1).first = true;
  }

  void MergeSets(vector<int> &sets, int from, int to) {
    for (auto it = sets.begin(); it != sets.end(); ++it) {
      if (*it == from) *it = to;
    }
  }

  void AddingHorizontalWalls(vector<int> &sets, int row) {
    bool adding_wall;
    for (int i = 0; i < columns_; ++i) {
      int iter = row * columns_ + i;
      adding_wall = RandomBool();
      if (adding_wall == true && CalculateFreeSet(sets, row, sets[i]) > 1) {
        cells_->at(iter).second = true;
      }
    }
  }

  int CalculateFreeSet(vector<int> &sets, int row, int set_number) {
    int free_sets = 0;
    for (int i = 0; i < columns_; i++) {
      if (sets[i] == set_number &&
          cells_->at(row * columns_ + i).second == false)
        ++free_sets;
    }
    return free_sets;
  }

  void PreparingNextLine(vector<int> &sets, int row) {
    for (int i = 0; i < columns_; i++) {
      if (cells_->at(row * columns_ + i).second == true) {
        sets[i] = 0;
      }
    }
  }

  bool RandomBool() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> random_int(0, 1);
    return (bool)random_int(rng);
  }

  void FixEndLine(vector<int> &sets) {
    for (int i = 0; i < columns_; ++i) {
      cells_->at(columns_ * (rows_ - 1) + i).second = true;
      if (i != columns_ - 1 && sets[i] != sets[i + 1]) {
        cells_->at(columns_ * (rows_ - 1) + i).first = false;
        MergeSets(sets, sets[i + 1], sets[i]);
      }
    }
  }

  void MazeCrawler(set<room> &set, int row, int column, int from_direction,
                   bool &loop_flag, vector<room> &path, bool &find_flag,
                   bool &found, int finish_row = 0, int finish_column = 0) {
    if (!loop_flag && !found) {
      if (find_flag) {
        path.push_back(make_pair(column, row));
        if (row == finish_row && column == finish_column) found = true;
      }
      if (!set.insert(make_pair(row, column)).second) {
        loop_flag = true;
      }
      if (from_direction != 1 && Up(row, column))  // from down
        MazeCrawler(set, row - 1, column, 4, loop_flag, path, find_flag, found,
                    finish_row, finish_column);      // go up
      if (from_direction != 4 && Down(row, column))  // from up
        MazeCrawler(set, row + 1, column, 1, loop_flag, path, find_flag, found,
                    finish_row, finish_column);       // go down
      if (from_direction != 2 && Right(row, column))  // from left
        MazeCrawler(set, row, column + 1, 3, loop_flag, path, find_flag, found,
                    finish_row, finish_column);      // go right
      if (from_direction != 3 && Left(row, column))  // from right
        MazeCrawler(set, row, column - 1, 2, loop_flag, path, find_flag, found,
                    finish_row, finish_column);  // go left
      if (find_flag && !found) path.pop_back();
    }
  }

  bool Down(int row, int column) const {
    bool answer = false;
    if (row >= 0 && column >= 0 && row < rows_ && column < columns_)
      answer = !(cells_->at(row * columns_ + column)).second;
    return answer;
  }

  bool Up(int row, int column) const {
    bool answer = false;
    if (row > 0 && column >= 0 && row < rows_ && column < columns_)
      answer = !(cells_->at((row - 1) * columns_ + column)).second;
    return answer;
  }

  bool Right(int row, int column) const {
    bool answer = false;
    if (row >= 0 && column >= 0 && row < rows_ && column < columns_)
      answer = !(cells_->at(row * columns_ + column)).first;
    return answer;
  }

  bool Left(int row, int column) const {
    bool answer = false;
    if (row >= 0 && column >= 0 && row < rows_ && column < columns_ &&
        (row != 0 || column != 0))
      answer = !(cells_->at(row * columns_ + column - 1)).first;
    return answer;
  }
};
}  // namespace s21

#endif  // MODEL_H
