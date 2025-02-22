#ifndef RAY_CORE_WORKER_COMMON_H
#define RAY_CORE_WORKER_COMMON_H

#include <string>

#include "ray/common/buffer.h"
#include "ray/common/id.h"
#include "ray/raylet/raylet_client.h"
#include "ray/raylet/task_spec.h"

namespace ray {

/// Type of this worker.
enum class WorkerType { WORKER, DRIVER };

/// Language of Ray tasks and workers.
enum class WorkerLanguage { PYTHON, JAVA };

/// Information about a remote function.
struct RayFunction {
  /// Language of the remote function.
  const WorkerLanguage language;
  /// Function descriptor of the remote function.
  const std::vector<std::string> function_descriptor;
};

/// Argument of a task.
class TaskArg {
 public:
  /// Create a pass-by-reference task argument.
  ///
  /// \param[in] object_id Id of the argument.
  /// \return The task argument.
  static TaskArg PassByReference(const ObjectID &object_id) {
    return TaskArg(std::make_shared<ObjectID>(object_id), nullptr);
  }

  /// Create a pass-by-reference task argument.
  ///
  /// \param[in] object_id Id of the argument.
  /// \return The task argument.
  static TaskArg PassByValue(const std::shared_ptr<Buffer> &data) {
    return TaskArg(nullptr, data);
  }

  /// Return true if this argument is passed by reference, false if passed by value.
  bool IsPassedByReference() const { return id_ != nullptr; }

  /// Get the reference object ID.
  const ObjectID &GetReference() const {
    RAY_CHECK(id_ != nullptr) << "This argument isn't passed by reference.";
    return *id_;
  }

  /// Get the value.
  std::shared_ptr<Buffer> GetValue() const {
    RAY_CHECK(data_ != nullptr) << "This argument isn't passed by value.";
    return data_;
  }

 private:
  TaskArg(const std::shared_ptr<ObjectID> id, const std::shared_ptr<Buffer> data)
      : id_(id), data_(data) {}

  /// Id of the argument, if passed by reference, otherwise nullptr.
  const std::shared_ptr<ObjectID> id_;
  /// Data of the argument, if passed by value, otherwise nullptr.
  const std::shared_ptr<Buffer> data_;
};

/// Task specification, which includes the immutable information about the task
/// which are determined at the submission time.
/// TODO(zhijunfu): this can be removed after everything is moved to protobuf.
class TaskSpec {
 public:
  TaskSpec(const raylet::TaskSpecification &task_spec,
           const std::vector<ObjectID> &dependencies)
      : task_spec_(task_spec), dependencies_(dependencies) {}

  TaskSpec(const raylet::TaskSpecification &&task_spec,
           const std::vector<ObjectID> &&dependencies)
      : task_spec_(task_spec), dependencies_(dependencies) {}

  const raylet::TaskSpecification &GetTaskSpecification() const { return task_spec_; }

  const std::vector<ObjectID> &GetDependencies() const { return dependencies_; }

 private:
  /// Raylet task specification.
  raylet::TaskSpecification task_spec_;

  /// Dependencies.
  std::vector<ObjectID> dependencies_;
};

enum class StoreProviderType { PLASMA };

enum class TaskTransportType { RAYLET };

}  // namespace ray

#endif  // RAY_CORE_WORKER_COMMON_H
