/* Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "paragraph/translation/allgather/allgather_translator.h"

#include <string>
#include <utility>

#include "absl/memory/memory.h"
#include "factory/ObjectFactory.h"

namespace paragraph {

absl::Status AllGatherTranslator::Translate(Instruction* instruction) const {
  RETURN_IF_FALSE(instruction->InnerSubroutines().empty(),
      absl::InternalError) << "Expected instruction that has no subroutine.";
  ASSIGN_OR_RETURN(auto new_subroutine, GetSubroutine(
      instruction->GetName(),
      instruction,
      instruction->GetGraph()->GetProcessorId(),
      instruction->GetProcessorIndex(instruction->GetGraph()->GetProcessorId()),
      instruction->GetCommunicationGroup(),
      instruction->GetBytesOut()));
  instruction->AppendInnerSubroutine(std::move(new_subroutine));
  RETURN_IF_ERROR(instruction->ValidateIndividualized());
  return absl::OkStatus();
}

shim::StatusOr<std::unique_ptr<AllGatherTranslator>>
    AllGatherTranslator::Create(nlohmann::json config) {
  const std::string& algorithm = config["algorithm"].get<std::string>();
  std::unique_ptr<AllGatherTranslator> translator = absl::WrapUnique(
      factory::ObjectFactory<AllGatherTranslator, nlohmann::json>::create(
          algorithm, config));
  RETURN_IF_FALSE(translator != nullptr, absl::InvalidArgumentError) <<
      "All-Gather algorithm name " << config["algorithm"] <<
      " obtained from Json config is invalid.";
  return translator;
}

}  // namespace paragraph
