#ifndef CPPKIT_TASK_GRAPH_HPP
#define CPPKIT_TASK_GRAPH_HPP

#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <ostream>
#include <queue>
#include <unordered_map>
#include <vector>

/*
 * A general task graph implementation
 *
 */

namespace cppkit {

/*
 * An interface for a task.
 */
struct Task {
  // Create a regular task
  Task() = default;
  // Create a task with specific tag
  Task(int tag) : tag_(tag) {}
  // Destroy the task
  virtual ~Task() = default;

  // Query or set the task properties

  // Query the tag of this task
  int tag() const { return tag_; }
  // Set the tag of this task
  void setTag(int tag) { tag_ = tag; };
  // Query all downstream tasks
  const std::vector<Task*>& downstreamTasks() { return downstreamTasks_; }
  // Add a downstream task
  void addDownstreamTask(Task* t) { downstreamTasks_.push_back(t); }
  // Query the upstream task count
  int upstreamCount() const { return upstreamCount_; }
  // Set the upstream task count
  void setUpstreamCount(int count) { upstreamCount_ = count; }
  // Increase the upstream task count
  void addUpstreamCount(int amount = 1) { upstreamCount_ += amount; }

  // Reset the task to ready-for-schedule state
  void reset() { pendingUpstreamCount_ = upstreamCount_; }
  // Progress the task, return whether the task is finished.
  virtual bool progress() = 0;
  // Check if the task is finished without any progressing.
  virtual bool finished() = 0;
  // Return an unique identity for the task
  virtual std::string id() = 0;

private:
  int tag_ = 0;
  int upstreamCount_ = 0;
  std::atomic<int> pendingUpstreamCount_;
  std::vector<Task*> downstreamTasks_;
};

/*
 * A TaskGraph for a set of tasks. The graph is non-owning. Users shall
 * place tasks in other owning containers and ensures the tasks are alive
 * during scheduling.
 *
 * The TaskGraph is merely used for facilitating task scheduling. The users
 * shall build their own scheduler by making use of the
 */
struct TaskGraph {
  TaskGraph() = default;

  // Add a task into the task graph.
  void addTask(Task* t) { tasksByTag_[t->tag()].push_back(t); }
  // Clear all tasks
  void clear() { tasksByTag_.clear(); }

  // Loop over all tasks
  //
  // Op is a functor like `void(Task* t)`
  template <typename Op>
  void foreach(Op op) const {
    for (auto& kv : tasksByTag_) {
      for (auto& t : kv.second) op(t);
    }
  }
  // Loop over tasks with specific tag
  //
  // Op is a functor like `void(Task* t)`
  template <typename Op>
  void foreachByTag(int tag, Op op) const {
    if (tasksByTag_.find(tag) == tasksByTag_.end()) return;
    for (auto t : tasksByTag_.at(tag)) {
      op(t);
    }
  }
  // Loop over tasks satisfing specific predicates
  //
  // Op is a functor like `void(Task* t)`
  // Pred is a functor like `bool(Task* t)`
  template <typename Op, typename Pred>
  void foreachIf(Op op, Pred pred) const {
    for (auto& kv : tasksByTag_) {
      for (auto t : kv.second) {
        if (pred(t)) op(t);
      }
    }
  }
  // Loop over tasks with specific upstream count
  //
  // Op is a functor like `void(Task* t)`
  template <typename Op>
  void foreachByUpstreamCount(int count, Op op) const {
    for (auto& kv : tasksByTag_) {
      for (auto t : kv.second) {
        if (t->upstreamCount() == count) op(t);
      }
    }
  }

  // Check if the task is valid
  //
  // The graph is valid if and only if the upstream count matches the real
  // upstream count and the graph is a DAG.
  //
  bool validate(std::ostream& os) const {
    std::unordered_map<Task*, int> counts;
    foreach([&counts](Task* t) {
      for (auto d : t->downstreamTasks()) {
        if (counts.find(d) == counts.end()) counts[d] = 0;
        counts[d]++;
      }
    });
    bool isValid = true;
    foreach([&counts, &isValid, &os](Task* t) {
      if (t->upstreamCount() != counts.at(t)) {
        isValid = false;
        os << "Invalid upstream count for '" << t->id() << "@" << t
           << "': claimed " << t->upstreamCount() << ", real " << counts.at(t)
           << std::endl;
      }
    });
    if (!isValid) return false;
    //
    // Check if the graph is a DAG.
    //
    // initialize roots (tasks with no upstreams)
    std::vector<Task*> tasksToVisit;
    foreachByUpstreamCount(
        0, [&tasksToVisit](Task* t) { tasksToVisit.push_back(t); });
    if (tasksToVisit.empty()) {
      os << "The task graph is cyclic: every task has at least one upstream."
         << std::endl;
      return false;
    }
    // initialize counts to 0 for duplicate visit check
    for (auto& kv : counts) kv.second = 0;
    for (size_t i = 0; i < tasksByTag_.size(); i++) {
      assert(!tasksToVisit.empty());
      auto t = tasksToVisit.pop_back();
      tasksToVisit.pop();
      if (++counts[t] > 1) {
        os << "The task graph is cyclic: task '" << t->id() << "@" << t
           << "' is in a cycle." << std::endl;
        return false;
      }
      for (auto d : t->downstreamTasks()) {
        if (std::find(tasksToVisit.begin(), tasksToVisit.end(), d) ==
            tasksToVisit.end())
          tasksToVisit.push(d);
      }
    }
    return isValid;
  }

private:
  std::unordered_map<int, std::vector<Task*>> tasksByTag_;
};

}  // namespace cppkit

#endif
