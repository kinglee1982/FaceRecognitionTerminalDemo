#ifndef FACE_EXTRACTOR_H
#define FACE_EXTRACTOR_H

#include "lib/handler.h"
#include "person_info.h"
#include "sz_face_module.h"

namespace suanzi {

typedef struct _ExtractionMessage {
  SZ_IMAGE_CTX *imageCtx;
  SZ_FACE_DETECTION face;
} ExtractionMessage;

typedef struct _ExtractionResult {
  SZ_IMAGE_CTX *imageCtx;
  SZ_FACE_DETECTION face;
  PersonInfo person;
} ExtractionResult;

class FaceExtractor : public Handler<ExtractionMessage> {
 public:
  class Callback {
   public:
    typedef Callback *Ptr;
    virtual void on_extracted(ExtractionResult &result) = 0;
  };

  const SZ_INT32 kMinHistorySize = 3;
  const SZ_INT32 kMaxHistorySize = 16;

  const SZ_INT32 kMinRecognizedCount = 3;
  const SZ_FLOAT kMinRecognizedScore = .5f;
  const SZ_FLOAT kMinAccumulateScore = 2.7f; // 0.45 * 6

  const SZ_INT32 kMaxLostAge = 10;

  FaceExtractor(SZ_FACE_CTX* dnn, SZ_DATABASE_CTX* db);
  void set_callback(Callback::Ptr callback);

  SZ_RETCODE extract(SZ_IMAGE_CTX *imageCtx, SZ_FACE_DETECTION *face,
                     SZ_FACE_FEATURE **feature, SZ_INT32 *feature_length);

 private:
  SZ_RETCODE query(SZ_FACE_FEATURE *feature, PersonInfo &person_info);
  PersonInfo sequence_query(const PersonInfo &person_info);

  void handle(ExtractionMessage &message);
  Callback::Ptr callback_;

  SZ_FACE_CTX* dnn_;
  SZ_DATABASE_CTX* db_;

  std::vector<PersonInfo> history_;
  PersonInfo determined_person_ = PERSON_NOT_DETECTED;
  bool person_changed_ = false;

  int lost_age_ = 0;
};

}  // namespace suanzi

#endif