/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_CONSENSUS_STATUS_PROCESSOR_HPP
#define IROHA_CONSENSUS_STATUS_PROCESSOR_HPP

namespace shared_model {
  namespace interface {
    class TransactionBatch;
  }  // namespace interface
}  // namespace shared_model

namespace iroha {
  namespace torii {
    /**
     * ConsensusStatusProcessor is interface which reflects statuses consensus
     * statuses and initiate processing of transactions in it
     */
    class ConsensusStatusProcessor {
     public:
      /**
       * Process batch and propagate it to the MST or PCS
       * @param transaction_batch - transaction batch for processing
       */
      virtual void batchHandle(const shared_model::interface::TransactionBatch
                                   &transaction_batch) const = 0;

      virtual ~ConsensusStatusProcessor() = default;
    };
  }  // namespace torii
}  // namespace iroha
#endif  // IROHA_CONSENSUS_STATUS_PROCESSOR_HPP