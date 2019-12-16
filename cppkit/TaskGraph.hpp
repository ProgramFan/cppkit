#ifndef CPPKIT_TASK_GRAPH_HPP
#define CPPKIT_TASK_GRAPH_HPP

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

/*
 * A general task graph implementation
 *
 */

namespace cppkit {

namespace taskgraph {
struct WithTag {};
struct WithPriority {};
struct WithTagAndPriority {};
}  // namespace taskgraph

namespace detail {
template <typename Tag>
struct tag_traits {
  static const bool tag = false;
  static const bool priority = false;
};
template <>
struct tag_traits<taskgraph::WithTag> {
  static const bool tag = true;
  static const bool priority = false;
};
template <>
struct tag_traits<taskgraph::WithPriority> {
  static const bool tag = false;
  static const bool priority = true;
};
template <>
struct tag_traits<taskgraph::WithTagAndPriority> {
  static const bool tag = true;
  static const bool priority = true;
};

struct TagAndPriority {
  int tag;
  double priority;
};
struct Tag {
  int tag;
};
struct Priority {
  double priority;
};

template <typename Tag>
struct MetaDataSelector {
  using type = void;
};
template <>
struct MetaDataSelector<taskgraph::WithTag> {
  using type = Tag;
};
template <>
struct MetaDataSelector<taskgraph::WithPriority> {
  using type = Priority;
};
template <>
struct MetaDataSelector<taskgraph::WithTagAndPriority> {
  using type = TagAndPriority;
};
}  // namespace detail

/*
 * An interface for a task.
 */
template <typename Tag = void>
struct Task {
  // Create a regular task
  Task() = default;
  // Create a task with specific tag
  template <typename std::enable_if<detail::tag_traits<Tag>::tag &&
                                        !detail::tag_traits<Tag>::priority,
                                    int>::type = 0>
  Task(int tag) : metaData_{tag} {}
  template <typename std::enable_if<!detail::tag_traits<Tag>::tag &&
                                        detail::tag_traits<Tag>::priority,
                                    int>::type = 0>
  Task(double priority) : metaData_{0, priority} {}
  template <typename std::enable_if<detail::tag_traits<Tag>::tag &&
                                        detail::tag_traits<Tag>::priority,
                                    int>::type = 0>
  Task(int tag, double priority) : metaData_{tag, priority} {}
  // Destroy the task
  virtual ~Task() = default;
  // Forbidden the task to copy
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  // Query or set the task properties

  // Query the tag of this task
  template <
      typename std::enable_if<detail::tag_traits<Tag>::tag, int>::type = 0>
  int tag() const {
    return metaData_.tag;
  }
  // Set the tag of this task
  template <
      typename std::enable_if<detail::tag_traits<Tag>::tag, int>::type = 0>
  void setTag(int tag) {
    metaData_.tag = tag;
  };
  // Query the tag of this task
  template <
      typename std::enable_if<detail::tag_traits<Tag>::priority, int>::type = 0>
  double priority() const {
    return metaData_.priority;
  }
  // Set the tag of this task
  template <
      typename std::enable_if<detail::tag_traits<Tag>::priority, int>::type = 0>
  void setPriority(double priority) {
    metaData_.priority = priority;
  };

  // Query all downstream tasks
  const std::unordered_set<Task*>& downstreamTasks() {
    return downstreamTasks_;
  }
  // Add a downstream task
  void addDownstreamTask(Task* t) {
    // Avoid add twice
    if (downstreamTasks_.find(t) == downstreamTasks_.end()) {
      downstreamTasks_.insert(t);
      t->upstreamCount_++;
    }
  }
  // Query the upstream task count
  int upstreamCount() const { return upstreamCount_; }
  // Add an upstream task. Please either addUpstreamTask or addDownstreamTask
  // and never mix them together in building inter-task dependencies.
  void addUpstreamTask(Task* t) {
    // Avoid add twice
    if (t->downstreamTasks_.find(this) == t->downstreamTasks_.end()) {
      t->downstreamTasks_.insert(this);
      upstreamCount_++;
    }
  }

  // Reset the task to ready-for-schedule state
  void reset() { pendingUpstreamCount_ = upstreamCount_; }

  //
  // Abstract interfaces for task definition
  //

  // Progress the task, return whether the task is finished.
  virtual bool progress() = 0;
  // Check if the task is finished without any progressing.
  virtual bool finished() const = 0;
  // Return an unique identity for the task
  virtual std::string id() const = 0;

private:
  int upstreamCount_ = 0;
  std::atomic<int> pendingUpstreamCount_;
  std::unordered_set<Task*> downstreamTasks_;
  detail::MetaDataSelector<Tag> metaData_;
};

/*
 * A TaskGraph for a set of tasks. The graph is non-owning. Users shall
 * place tasks in other owning containers and ensures the tasks are alive
 * during scheduling.
 *
 * The TaskGraph is merely used for facilitating task scheduling. The users
 * shall build their own scheduler by making use of the
 */
template <typename Tag = void>
struct TaskGraph {
  TaskGraph() = default;

  // Return how many tags are in the task graph
  template <
      typename std::enable_if<detail::tag_traits<Tag>::tag, int>::type = 0>
  int tagCount() const {
    return tasksByTag_.size();
  }
  // Return how many tasks are in the task graph.
  size_t taskCount() const { return taskCount_; }

  // Add a task into the task graph.
  template <
      typename std::enable_if<detail::tag_traits<Tag>::tag, int>::type = 0>
  void addTask(Task<Tag>* t) {
    tasksByTag_[t->tag()].push_back(t);
    taskCount_++;
  }
  template <
      typename std::enable_if<!detail::tag_traits<Tag>::tag, int>::type = 0>
  void addTask(Task<Tag>* t) {
    tasksByTag_[0].push_back(t);
    taskCount_++;
  }
  // Clear all tasks
  void clear() {
    tasksByTag_.clear();
    taskCount_ = 0;
  }

  // Loop over all tasks
  //
  // Op is a functor like `void(Task* t)`
  template <typename Op>
  void foreach (Op op) const {
    for (auto& kv : tasksByTag_) {
      for (auto& t : kv.second) op(t);
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
  // Loop over tasks with specific tag
  //
  // Op is a functor like `void(Task* t)`
  template <typename Op, typename std::enable_if<detail::tag_traits<Tag>::tag,
                                                 int>::type = 0>
  void foreachByTag(int tag, Op op) const {
    if (tasksByTag_.find(tag) == tasksByTag_.end()) return;
    for (auto t : tasksByTag_.at(tag)) {
      op(t);
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
  std::pair<bool, std::string> validate(bool diagnostics = false) const {
    std::unordered_map<Task<Tag>*, int> counts;
    foreach ([&counts](Task<Tag>* t) {
      if (counts.find(t) == counts.end()) counts[t] = 0;
      for (auto d : t->downstreamTasks()) {
        if (counts.find(d) == counts.end()) counts[d] = 0;
        counts[d]++;
      }
    })
      ;
    bool isValid = true;
    std::ostringstream os;
    foreach ([&counts, &isValid, &os, &diagnostics](Task<Tag>* t) {
      if (t->upstreamCount() != counts.at(t)) {
        isValid = false;
        if (diagnostics) {
          os << "Invalid upstream count for '" << t->id() << "@" << t
             << "': claimed " << t->upstreamCount() << ", real "
             << counts.at(t);
        }
      }
    })
      ;
    if (!isValid) return {false, os.str()};
    //
    // Check if the graph is a DAG.
    //
    // initialize roots (tasks with no upstreams)
    std::queue<Task<Tag>*> ready;
    foreachByUpstreamCount(0, [&ready](Task<Tag>* t) { ready.push(t); });
    if (ready.empty()) {
      if (diagnostics) {
        os << "The task graph is cyclic: there exist no source tasks.";
      }
      return {false, os.str()};
    }
    // Count tasks without dependent tasks. There shall be at least one such
    // task in a DAG.
    int sinkCount = 0;
    foreachIf(
        [&sinkCount](Task<Tag>* t) { sinkCount++; },
        [](Task<Tag>* t) -> bool { return t->downstreamTasks().empty(); });
    if (sinkCount == 0) {
      if (diagnostics) {
        os << "The task graph is cyclic: there exist no sink tasks.";
      }
      return {false, os.str()};
    }
    // Pseudo scheduling with count limit to check for cycle. A digraph is
    // acyclic if and only if it can be scheduled with digraph.size() turns.
    for (auto& kv : counts) kv.second = kv.first->upstreamCount();
    size_t tasksTodo = this->taskCount();
    for (size_t i = 0; i < taskCount(); i++) {
      if (ready.empty()) {
        if (diagnostics) {
          os << "The task graph is cyclic: at least one cycle exists in [";
          bool first = true;
          for (auto& kv : counts) {
            if (kv.second > 0)
              os << (first ? (first = false, "'") : ", '") << kv.first->id()
                 << "@" << kv.first << "'";
          }
          os << "]";
        }
        return {false, os.str()};
      }
      auto t = ready.front();
      ready.pop();
      tasksTodo--;
      for (auto d : t->downstreamTasks()) {
        int c = --counts[d];
        if (c < 0) {
          if (diagnostics) {
            os << "The task graph is cyclic: task '" << d->id() << "@" << d
               << "' is in a cycle.";
          }
          return {false, os.str()};
        } else if (c == 0) {
          ready.push(d);
        }
      }
    }
    if (tasksTodo > 0) {
      if (diagnostics) {
        os << "The task graph is cyclic: still tasks after taskCount "
              "schedules.";
      }
      return {false, os.str()};
    }
    return {true, ""};
  }

  // Reset tasks for scheduling
  void reset() {
    foreach ([](Task<Tag>* t) { t->reset(); })
      ;
  }

  // Construct an graphviz representation of the graph. Can be feed into `dot`
  // to visualize the graph.
  std::string toString() const {
    if (tasksByTag_.empty()) return "digraph {}";
    std::ostringstream os;
    os << "digraph {" << std::endl;
    foreach ([&os](Task<Tag>* t) {
      if (t->upstreamCount() == 0 && t->downstreamTasks().empty()) {
        os << "  \"" << t->id() << "\";" << std::endl;
      } else {
        for (auto d : t->downstreamTasks()) {
          os << "  \"" << t->id() << "\" -> \"" << d->id() << "\";"
             << std::endl;
        }
      }
    })
      ;
    os << "}";
    return os.str();
  }

private:
  size_t taskCount_ = 0;
  std::unordered_map<int, std::vector<Task<Tag>*>> tasksByTag_;
};

}  // namespace cppkit

#endif
