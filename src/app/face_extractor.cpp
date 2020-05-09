#include "face_extractor.h"

#include <ctime>
#include <nlohmann/json.hpp>

#include "lib/logger.h"
#include "sz_database_module.h"

using namespace suanzi;

FaceExtractor::FaceExtractor(SZ_FACE_CTX *dnn, SZ_DATABASE_CTX *db) {
  dnn_ = dnn;
  db_ = db;
}

void FaceExtractor::set_callback(FaceExtractor::Callback::Ptr callback) {
  callback_ = callback;
}

SZ_RETCODE FaceExtractor::extract(SZ_IMAGE_CTX *imageCtx,
                                  SZ_FACE_DETECTION *face,
                                  SZ_FACE_FEATURE **feature,
                                  SZ_INT32 *feature_length) {
  auto start = std::chrono::system_clock::now();
  SZ_RETCODE ret = SZ_FACE_extractFeatureByPosition(dnn_, imageCtx, face,
                                                    feature, feature_length);
  auto end = std::chrono::system_clock::now();
  SZ_LOG_DEBUG("extraction elapsed={:.2f}ms",
               (double)(std::chrono::duration_cast<std::chrono::milliseconds>(
                            end - start)
                            .count()));

  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("SZ_FACE_extractFeatureByPosition failed");
  }
  return ret;
}

void FaceExtractor::handle(ExtractionMessage &message) {
  static SZ_FACE_FEATURE *feature;
  static SZ_INT32 feature_length;
  static PersonInfo person_info;
  static ExtractionResult extraction_result;

  extraction_result.imageCtx = message.imageCtx;
  extraction_result.face = message.face;

  if (message.imageCtx == NULL) {
    if (lost_age_++ >= kMaxLostAge) {
      determined_person_ = PERSON_NOT_DETECTED;
      history_.clear();
    }
    extraction_result.person = determined_person_;
    person_changed_ = false;
  } else if (!person_changed_ &&
             SZ_RETCODE_OK == extract(message.imageCtx, &message.face, &feature,
                                      &feature_length)) {
    lost_age_ = 0;
    if (SZ_RETCODE_OK == query(feature, person_info)) {
      SZ_LOG_DEBUG("person_info: name={}, score={}", person_info.name,
                   person_info.score);

      extraction_result.person = sequence_query(person_info);
    }
  }

  if (callback_) {
    callback_->on_extracted(extraction_result);
  }
}

SZ_RETCODE FaceExtractor::query(SZ_FACE_FEATURE *feature,
                                PersonInfo &person_info) {
  SZ_QUERY_RESULT *query_results;
  SZ_INT32 top_n;

  if (SZ_RETCODE_OK !=
      SZ_DATABASE_query(db_, feature, &query_results, &top_n)) {
    SZ_LOG_ERROR("SZ_DATABASE_query failed");

    return SZ_RETCODE_FAILED;
  } else {
    if (top_n == 0) {
      SZ_LOG_WARN("SZ_DATABASE_query: no record in database");
      person_info = PERSON_UNKNOWN;
      return SZ_RETCODE_OK;
    }

    SZ_INT32 info_length;
    if (SZ_RETCODE_OK != SZ_DATABASE_getInfoLen(db_, &info_length)) {
      SZ_LOG_ERROR("SZ_DATABASE_getInfoLen failed");
      return SZ_RETCODE_FAILED;
    }

    char *info_data = new char[info_length];
    std::string name;
    for (SZ_INT32 i = 0; i < top_n; i++) {
      memcpy(info_data, query_results[i].pInfo, info_length);
      name = nlohmann::json::parse(info_data)["name"];
      SZ_LOG_DEBUG("query top_{}: name={}, score={}", i + 1, name,
                   query_results[i].score);

      if (i == 0) person_info.name = name;
    }

    person_info.id = query_results[0].faceId;
    person_info.score = query_results[0].score;

    delete[] info_data;
    return SZ_RETCODE_OK;
  }
}

PersonInfo FaceExtractor::sequence_query(const PersonInfo &person_info) {
  history_.push_back(person_info);
  if (history_.size() < kMinHistorySize) return PERSON_NOT_DETECTED;
  if (history_.size() > kMaxHistorySize) history_.erase(history_.begin());

  std::map<int, int> person_counts;
  std::map<int, float> person_max_score;
  std::map<int, float> person_accumulate_score;
  std::map<int, std::string> person_names;

  for (auto &person : history_) {
    // initialize map
    if (person_names.find(person.id) == person_names.end())
      person_names[person.id] = person.name;

    if (person_counts.find(person.id) == person_counts.end())
      person_counts[person.id] = 0;

    if (person_max_score.find(person.id) == person_max_score.end())
      person_max_score[person.id] = 0.f;

    if (person_accumulate_score.find(person.id) ==
        person_accumulate_score.end())
      person_accumulate_score[person.id] = 0.f;

    // query count of each person id
    person_counts[person.id] += 1;

    // max score of each person id
    if (person.score > person_max_score[person.id])
      person_max_score[person.id] = person.score;

    // accumulate score of each person id
    person_accumulate_score[person.id] += person.score;
  }

  // person id with max counts
  int max_count = 0, max_person_id = -1;
  for (auto &person_count : person_counts) {
    if (person_count.second > max_count) {
      max_count = person_count.second;
      max_person_id = person_count.first;
    }
  }

  if (max_count >= kMinRecognizedCount &&
      person_accumulate_score[max_person_id] >= kMinAccumulateScore &&
      person_max_score[max_person_id] >= kMinRecognizedScore) {
    SZ_LOG_DEBUG("name={}, count={}, accumulate_score={}",
                 person_names[max_person_id], max_count,
                 person_accumulate_score[max_person_id]);

    person_changed_ = true;
    determined_person_ = {max_person_id, person_names[max_person_id],
                          person_max_score[max_person_id]};

    return determined_person_;
  }

  if (history_.size() == kMaxHistorySize) {
    SZ_LOG_DEBUG("name={}, count={}, accumulate_score={}",
                 person_names[max_person_id], max_count,
                 person_accumulate_score[max_person_id]);

    person_changed_ = true;
    determined_person_ = PERSON_UNKNOWN;
    return determined_person_;
  }

  return determined_person_;
}
