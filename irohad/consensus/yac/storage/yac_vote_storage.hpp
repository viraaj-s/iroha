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

#ifndef IROHA_YAC_VOTE_STORAGE_HPP
#define IROHA_YAC_VOTE_STORAGE_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include <boost/optional.hpp>
#include "consensus/yac/messages.hpp"  // because messages passed by value
#include "consensus/yac/storage/storage_result.hpp"  // for Answer
#include "consensus/yac/storage/yac_common.hpp"      // for ProposalHash
#include "consensus/yac/yac_types.hpp"

namespace iroha {
  namespace consensus {
    namespace yac {
      class YacProposalStorage;

      /**
       * Proposal outcome states for multicast propagation strategy
       *
       * Outcome is either CommitMessage, which guarantees that supermajority of
       * votes for the proposal-block hashes is collected, or RejectMessage,
       * which states that supermajority of votes for a block hash cannot be
       * achieved
       *
       * kNotSentNotProcessed - outcome was not propagated in the network
       * AND it was not passed to pipeline. Initial state after receiving an
       * outcome from storage. Outcome with votes is propagated to the network
       * in this state.
       *
       * kSentNotProcessed - outcome was propagated in the network
       * AND it was not passed to pipeline. State can be set in two cases:
       * 1. Outcome is received from the network. Some node has already achieved
       * an outcome and has propagated it to the network, so the first state is
       * skipped.
       * 2. Outcome was propagated to the network
       * Outcome is passed to pipeline in this state.
       *
       * kSentProcessed - outcome was propagated in the network
       * AND it was passed to pipeline. Set after passing proposal to pipeline.
       * This state is final. Receiving a network message in this state results
       * in direct propagation of outcome to message sender.
       */
      enum class ProposalState {
        kNotSentNotProcessed,
        kSentNotProcessed,
        kSentProcessed
      };

      /**
       * Class provide storage for votes and useful methods for it.
       */
      class YacVoteStorage {
       private:
        // --------| private api |--------

        /**
         * Retrieve iterator for storage with specified key
         * @param round - key of that storage
         * @return iterator to proposal storage
         */
        auto getProposalStorage(const Round &round);

        /**
         * Find existed proposal storage or create new if required
         * @param msg - vote for finding
         * @param peers_in_round - number of peer required
         * for verify supermajority;
         * This parameter used on creation of proposal storage
         * @return - iter for required proposal storage
         */
        auto findProposalStorage(const VoteMessage &msg,
                                 PeersNumberType peers_in_round);

       public:
        // --------| public api |--------

        /**
         * Insert votes in storage
         * @param state - current message with votes
         * @param peers_in_round - number of peers participated in round
         * @return structure with result of inserting.
         * boost::none if msg not valid.
         */
        boost::optional<Answer> store(std::vector<VoteMessage> state,
                                      PeersNumberType peers_in_round);

        /**
         * Provide status about closing round of proposal/block
         * @param round, in which proposal/block is supposed to be committed
         * @return true, if round closed
         */
        bool isCommitted(const Round &round);

        /**
         * Method provide state of processing for concrete proposal/block
         * @param round, in which that proposal/block is being voted
         * @return value attached to parameter's round. Default is
         * kNotSentNotProcessed.
         */
        ProposalState getProcessingState(const Round &round);

        /**
         * Mark round with following transition:
         * kNotSentNotProcessed -> kSentNotProcessed
         * kSentNotProcessed -> kSentProcessed
         * kSentProcessed -> kSentProcessed
         * @see ProposalState description for transition cases
         * @param round - target tag
         */
        void nextProcessingState(const Round &round);

       private:
        // --------| fields |--------

        /**
         * Active proposal storages
         */
        std::vector<YacProposalStorage> proposal_storages_;

        /**
         * Processing set provide user flags about processing some
         * proposals/blocks.
         * If such round exists <=> processed
         */
        std::unordered_map<Round, ProposalState, RoundTypeHasher>
            processing_state_;
      };

    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha

#endif  // IROHA_YAC_VOTE_STORAGE_HPP
