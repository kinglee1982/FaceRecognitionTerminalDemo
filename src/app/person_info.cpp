#include "person_info.h"

using namespace suanzi;

bool PersonInfo::operator==(const PersonInfo &p) { return p.id == this->id; }

bool PersonInfo::operator!=(const PersonInfo &p) { return p.id != this->id; }