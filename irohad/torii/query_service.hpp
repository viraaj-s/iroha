/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TORII_QUERY_SERVICE_HPP
#define TORII_QUERY_SERVICE_HPP

#include <unordered_map>
#include "endpoint.grpc.pb.h"
#include "endpoint.pb.h"
#include "qry_responses.pb.h"

#include "backend/protobuf/queries/proto_blocks_query.hpp"
#include "backend/protobuf/queries/proto_query.hpp"
#include "builders/protobuf/transport_builder.hpp"
#include "cache/cache.hpp"
#include "torii/processor/query_processor.hpp"

#include "logger/logger.hpp"

namespace shared_model {
  namespace interface {
    template <typename Interface, typename Transport>
    class AbstractTransportFactory;
  }
}  // namespace shared_model

namespace torii {
  /**
   * Actual implementation of async QueryService.
   * ToriiServiceHandler::(SomeMethod)Handler calls a corresponding method in
   * this class.
   */
  class QueryService : public iroha::protocol::QueryService_v1::Service {
   public:
    using QueryFactoryType =
        shared_model::interface::AbstractTransportFactory<
            shared_model::interface::Query,
            iroha::protocol::Query>;

    QueryService(std::shared_ptr<iroha::torii::QueryProcessor> query_processor,
                 std::shared_ptr<QueryFactoryType> query_factory);

    QueryService(const QueryService &) = delete;
    QueryService &operator=(const QueryService &) = delete;

    /**
     * actual implementation of async Find in QueryService
     * @param request - Query
     * @param response - QueryResponse
     */
    void Find(iroha::protocol::Query const &request,
              iroha::protocol::QueryResponse &response);

    grpc::Status Find(grpc::ServerContext *context,
                      const iroha::protocol::Query *request,
                      iroha::protocol::QueryResponse *response) override;

    grpc::Status FetchCommits(
        grpc::ServerContext *context,
        const iroha::protocol::BlocksQuery *request,
        grpc::ServerWriter<::iroha::protocol::BlockQueryResponse> *writer)
        override;

   private:
    std::shared_ptr<iroha::torii::QueryProcessor> query_processor_;
    std::shared_ptr<QueryFactoryType> query_factory_;

    iroha::cache::Cache<shared_model::crypto::Hash,
                        iroha::protocol::QueryResponse,
                        shared_model::crypto::Hash::Hasher>
        cache_;

    logger::Logger log_;
  };

}  // namespace torii

#endif  // TORII_QUERY_SERVICE_HPP
