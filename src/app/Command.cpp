#include "Command.h"
#include <algorithm>

// CommandManager implementation

void CommandManager::execute(std::unique_ptr<ICommand> command) {
    command->execute();
    undo_stack_.push_back(std::move(command));
    redo_stack_.clear();
}

void CommandManager::undo() {
    if (undo_stack_.empty()) return;
    
    auto command = std::move(undo_stack_.back());
    undo_stack_.pop_back();
    command->undo();
    redo_stack_.push_back(std::move(command));
}

void CommandManager::redo() {
    if (redo_stack_.empty()) return;
    
    auto command = std::move(redo_stack_.back());
    redo_stack_.pop_back();
    command->execute();
    undo_stack_.push_back(std::move(command));
}

std::string CommandManager::get_undo_description() const {
    if (undo_stack_.empty()) return "";
    return undo_stack_.back()->get_description();
}

std::string CommandManager::get_redo_description() const {
    if (redo_stack_.empty()) return "";
    return redo_stack_.back()->get_description();
}

void CommandManager::clear() {
    undo_stack_.clear();
    redo_stack_.clear();
}

// AddPointCommand implementation

AddPointCommand::AddPointCommand(std::vector<Vector> &points, const Vector &point, size_t position)
    : points_(points), point_(point), position_(position) {}

void AddPointCommand::execute() {
    if (position_ <= points_.size()) {
        points_.insert(points_.begin() + position_, point_);
    } else {
        points_.push_back(point_);
    }
}

void AddPointCommand::undo() {
    if (position_ < points_.size()) {
        points_.erase(points_.begin() + position_);
    }
}

std::string AddPointCommand::get_description() const {
    return "Add point";
}

// RemovePointCommand implementation

RemovePointCommand::RemovePointCommand(std::vector<Vector> &points, size_t position)
    : points_(points), position_(position) {}

void RemovePointCommand::execute() {
    if (position_ < points_.size()) {
        removed_point_ = points_[position_];
        points_.erase(points_.begin() + position_);
    }
}

void RemovePointCommand::undo() {
    if (position_ <= points_.size()) {
        points_.insert(points_.begin() + position_, removed_point_);
    }
}

std::string RemovePointCommand::get_description() const {
    return "Remove point";
}

// MovePointCommand implementation

MovePointCommand::MovePointCommand(std::vector<Vector> &points, size_t position, const Vector &new_position)
    : points_(points), position_(position), new_position_(new_position) {}

void MovePointCommand::execute() {
    if (position_ < points_.size()) {
        old_position_ = points_[position_];
        points_[position_] = new_position_;
    }
}

void MovePointCommand::undo() {
    if (position_ < points_.size()) {
        points_[position_] = old_position_;
    }
}

std::string MovePointCommand::get_description() const {
    return "Move point";
}

// BatchCommand implementation

BatchCommand::BatchCommand(std::function<void()> execute_fn, 
                           std::function<void()> undo_fn,
                           const std::string &description)
    : execute_fn_(std::move(execute_fn)),
      undo_fn_(std::move(undo_fn)),
      description_(description) {}

void BatchCommand::execute() {
    execute_fn_();
}

void BatchCommand::undo() {
    undo_fn_();
}

std::string BatchCommand::get_description() const {
    return description_;
}
