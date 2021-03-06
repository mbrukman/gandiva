// Copyright (C) 2017-2018 Dremio Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef GANDIVA_PROJECTOR_CACHE_KEY_H
#define GANDIVA_PROJECTOR_CACHE_KEY_H

#include "gandiva/arrow.h"
#include "gandiva/projector.h"

namespace gandiva {
class ProjectorCacheKey {
 public:
  ProjectorCacheKey(SchemaPtr schema, std::shared_ptr<Configuration> configuration,
                    ExpressionVector expression_vector)
      : schema_(schema), configuration_(configuration) {
    static const int kSeedValue = 4;
    size_t result = kSeedValue;
    for (auto &expr : expression_vector) {
      std::string expr_as_string = expr->ToString();
      expressions_as_strings_.push_back(expr_as_string);
      boost::hash_combine(result, expr_as_string);
    }
    boost::hash_combine(result, configuration);
    boost::hash_combine(result, schema_->ToString());
    hash_code_ = result;
  }

  std::size_t Hash() const { return hash_code_; }

  bool operator==(const ProjectorCacheKey &other) const {
    // arrow schema does not overload equality operators.
    if (!(schema_->Equals(*other.schema().get(), true))) {
      return false;
    }

    if (configuration_ != other.configuration_) {
      return false;
    }

    if (expressions_as_strings_ != other.expressions_as_strings_) {
      return false;
    }
    return true;
  }

  bool operator!=(const ProjectorCacheKey &other) const { return !(*this == other); }

  SchemaPtr schema() const { return schema_; }

 private:
  const SchemaPtr schema_;
  const std::shared_ptr<Configuration> configuration_;
  std::vector<std::string> expressions_as_strings_;
  size_t hash_code_;
};
}  // namespace gandiva
#endif  // GANDIVA_PROJECTOR_CACHE_KEY_H
