#include <iostream>

#include "TaskGraph.hpp"

class SimpleTask : public cppkit::Task<> {
public:
  SimpleTask(int id) : cppkit::Task<>(), id_(id) {}

  bool progress() override { return true; }
  bool finished() const override { return true; }
  std::string id() const override { return std::to_string(id_); }

private:
  int id_;
};

void doTest() {
  // Contruct a task graph.
  std::vector<std::unique_ptr<SimpleTask>> tasks;
  for (int i = 0; i < 6; i++) {
    tasks.emplace_back(new SimpleTask(i));
  }
  tasks[0]->addDownstreamTask(tasks[1].get());
  tasks[1]->addDownstreamTask(tasks[2].get());
  tasks[2]->addDownstreamTask(tasks[3].get());
  tasks[3]->addDownstreamTask(tasks[4].get());
  tasks[4]->addDownstreamTask(tasks[2].get());
  cppkit::TaskGraph<> tg;
  for (auto& t : tasks) tg.addTask(t.get());
  auto result = tg.validate(true);
  if (!result.first) std::cerr << result.second << std::endl;
  std::cout << tg.toString() << std::endl;
}

int main(int argc, char* argv[]) {
  doTest();
  return 0;
}
