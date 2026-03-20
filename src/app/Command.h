#pragma once
#include "math/Vector.h"
#include <vector>
#include <memory>
#include <functional>

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
    bool can_undo() const { return !undo_stack_.empty(); }
    
    // Check if redo is available
    bool can_redo() const { return !redo_stack_.empty(); }
    
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
    AddPointCommand(std::vector<Vector> &points, const Vector &point, size_t position);
    
    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::vector<Vector> &points_;
    Vector point_;
    size_t position_;
};

// Command for removing a control point
class RemovePointCommand : public ICommand {
public:
    RemovePointCommand(std::vector<Vector> &points, size_t position);
    
    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::vector<Vector> &points_;
    Vector removed_point_;
    size_t position_;
};

// Command for moving a control point
class MovePointCommand : public ICommand {
public:
    MovePointCommand(std::vector<Vector> &points, size_t position, const Vector &new_position);
    
    void execute() override;
    void undo() override;
    std::string get_description() const override;

private:
    std::vector<Vector> &points_;
    size_t position_;
    Vector old_position_;
    Vector new_position_;
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
