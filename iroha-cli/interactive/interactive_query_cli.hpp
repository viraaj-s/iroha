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

#ifndef IROHA_CLI_INTERACTIVE_QUERY_CLI_HPP
#define IROHA_CLI_INTERACTIVE_QUERY_CLI_HPP

#include <unordered_map>
#include "interactive/interactive_common_cli.hpp"
#include "model/generators/query_generator.hpp"
#include "model/query.hpp"

namespace iroha_cli {
  namespace interactive {
    class InteractiveQueryCli {
     public:
      /**
       * @param account_id creator's account identification
       * @param query_counter counter associated with creator's account
       */
      InteractiveQueryCli(std::string account_id, uint64_t query_counter);
      /**
       * Run interactive query command line
       */
      void run();

     private:
      // Creator account id
      std::string creator_;

      // Local query counter of account creator_
      uint64_t counter_;

      //Local time
      uint64_t local_time_;

      // Query menu points
      std::vector<std::string> menu_points_;

      // Query result points
      std::vector<std::string> result_points_;

      // Current context for query forming
      MenuContext current_context_;

      // Processed query
      std::shared_ptr<iroha::model::Query> query_;

      // Query generator for new queries
      iroha::model::generators::QueryGenerator generator_;

      /**
       * Create query menu and assign command handlers for current class
       * object
       */
      void create_queries_menu();
      /**
       * Create result menu and assign result handlers for current class object
       */
      void create_result_menu();

      // ------ Query handlers -----------
      const std::string GET_ACC = "get_acc";
      const std::string GET_ACC_AST = "get_acc_ast";
      const std::string GET_ACC_TX = "get_acc_tx";
      const std::string GET_ACC_SIGN = "get_acc_sign";


      ParamsMap query_params_;
      using QueryName = std::string;
      using QueryParams = std::vector<std::string>;

      // ------  Query parsers ---------
      using QueryHandler = std::shared_ptr<iroha::model::Query> (
      InteractiveQueryCli::*)(QueryParams);
      std::unordered_map<QueryName, QueryHandler> query_handlers_;
      /**
       * Parse line for query
       * @param line - line containing query
       * @return True - if parsing process must be continued. False if parsing
       * context should be changed
       */
      bool parseQuery(std::string line);
      //  --- Specific Query parsers ---
      std::shared_ptr<iroha::model::Query> parseGetAccount(QueryParams params);
      std::shared_ptr<iroha::model::Query> parseGetAccountAssets(
          QueryParams params);
      std::shared_ptr<iroha::model::Query> parseGetAccountTransactions(
          QueryParams params);
      std::shared_ptr<iroha::model::Query> parseGetSignatories(
          QueryParams params);
      // ------ Result parsers --------
      using ResultHandler = bool (InteractiveQueryCli::*)(QueryParams);
      std::unordered_map<QueryName, ResultHandler> result_handlers_;
      /**
       * Parse line for result
       * @param line - cli command
       * @return True - if parsing process must be continued. False if parsing
       * context should be changed
       */
      bool parseResult(std::string line);
      // ---- Specific Result handlers
      bool parseSendToIroha(QueryParams line);
      bool parseSaveFile(QueryParams line);

    };
  }  // namespace interactive
}  // namespace iroha_cli
#endif  // IROHA_CLI_INTERACTIVE_QUERY_CLI_HPP
