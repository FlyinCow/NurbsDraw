#pragma once
#include "math/Vector.h"
#include <functional>
#include <memory>
#include <vector>

// Command interface for undo/redo
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string get_description() const = 0;
};

// Command manager for undo/redo
class CommandManager {
public:
    CommandManager() = default;

    // Execute a command and add it to history
    void execute(std::unique_ptr<ICommand> command);

    // Undo last command
    void undo();

    // Redo previously undone command
    void redo();

    // Check if undo is available
    bool can_undo() const {
        return !undo_stack_.empty();
    }

    // Check if redo is available
    bool can_redo() const {
        return !redo_stack_.empty();
    }

    // Get description of command to be undone
    std::string get_undo_description() const;

    // Get description of command to be redone
    std::string get_redo_description() const;

    // Clear history
    void clear();

private:
    std::vector<std::unique_ptr<ICommand>> undo_stack_;
    std::vector<std::unique_ptr<ICommand>> redo_stack_;
};

// Command for adding a control point
class AddPointCommand : public ICommand {
public:
    AddPointCommand(std::vector<Vec3d> &points, const Vec3d &point, size_t position);

    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::vector<Vec3d> &points_;
    Vec3d point_;
    size_t position_;
};

// Command for removing a control point
class RemovePointCommand : public ICommand {
public:
    RemovePointCommand(std::vector<Vec3d> &points, size_t position);

    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::vector<Vec3d> &points_;
    Vec3d removed_point_;
    size_t position_;
};

// Command for moving a control point
class MovePointCommand : public ICommand {
public:
    MovePointCommand(std::vector<Vec3d> &points, size_t position, const Vec3d &new_position);

    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::vector<Vec3d> &points_;
    size_t position_;
    Vec3d old_position_;
    Vec3d new_position_;
};

// Command for batch editing (e.g., clear all points)
class BatchCommand : public ICommand {
public:
    BatchCommand(std::function<void()> execute_fn,
                 std::function<void()> undo_fn,
                 const std::string &description);

    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::function<void()> execute_fn_;
    std::function<void()> undo_fn_;
    std::string description_;
};
