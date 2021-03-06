/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/proto_block_factory.hpp"

#include "backend/protobuf/block.hpp"

using namespace shared_model::proto;

ProtoBlockFactory::ProtoBlockFactory(
    std::unique_ptr<shared_model::validation::AbstractValidator<
        shared_model::interface::Block>> interface_validator,
    std::unique_ptr<
        shared_model::validation::AbstractValidator<iroha::protocol::Block>>
        proto_validator)
    : interface_validator_{std::move(interface_validator)},
      proto_validator_{std::move(proto_validator)} {}

std::unique_ptr<shared_model::interface::Block>
ProtoBlockFactory::unsafeCreateBlock(
    interface::types::HeightType height,
    const interface::types::HashType &prev_hash,
    interface::types::TimestampType created_time,
    const interface::types::TransactionsCollectionType &txs,
    const interface::types::HashCollectionType &rejected_hashes) {
  iroha::protocol::Block_v1 block;
  auto *block_payload = block.mutable_payload();
  block_payload->set_height(height);
  block_payload->set_prev_block_hash(crypto::toBinaryString(prev_hash));
  block_payload->set_created_time(created_time);

  // set accepted transactions
  std::for_each(
      std::begin(txs), std::end(txs), [block_payload](const auto &tx) {
        auto *transaction = block_payload->add_transactions();
        (*transaction) = static_cast<const Transaction &>(tx).getTransport();
      });

  // set rejected transactions
  std::for_each(std::begin(rejected_hashes),
                std::end(rejected_hashes),
                [block_payload](const auto &hash) {
                  auto *next_hash =
                      block_payload->add_rejected_transactions_hashes();
                  (*next_hash) = crypto::toBinaryString(hash);
                });

  return std::make_unique<shared_model::proto::Block>(std::move(block));
}

iroha::expected::Result<std::unique_ptr<shared_model::interface::Block>,
                        std::string>
ProtoBlockFactory::createBlock(iroha::protocol::Block block) {
  if (auto errors = proto_validator_->validate(block)) {
    return iroha::expected::makeError(errors.reason());
  }

  std::unique_ptr<shared_model::interface::Block> proto_block =
      std::make_unique<Block>(std::move(block.block_v1()));
  if (auto errors = interface_validator_->validate(*proto_block)) {
    return iroha::expected::makeError(errors.reason());
  }

  return iroha::expected::makeValue(std::move(proto_block));
}
