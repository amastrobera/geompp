#include "IntersectionEvent2D.hpp"

#include "Point2D.hpp"

namespace GeomPP {

IntersectionEvent2D::IntersectionEvent2D(geompp::detail::IntersectionEvent2D* native) : _native(native) {}

IntersectionEvent2D::~IntersectionEvent2D() { this->!IntersectionEvent2D(); }

IntersectionEvent2D::!IntersectionEvent2D() {
    delete _native;
    _native = nullptr;
}

Point2D^ IntersectionEvent2D::Point::get() {
    return gcnew Point2D(new geompp::Point2D(_native->Point));
}

int IntersectionEvent2D::SegmentId1::get() { return static_cast<int>(_native->SegmentIds[0]); }

int IntersectionEvent2D::SegmentId2::get() { return static_cast<int>(_native->SegmentIds[1]); }

System::Collections::Generic::List<int>^ IntersectionEvent2D::SegmentIds::get() {
    auto list = gcnew System::Collections::Generic::List<int>();
    for (auto id : _native->SegmentIds) { list->Add(static_cast<int>(id)); }
    return list;
}

System::String^ IntersectionEvent2D::ToString() {
    return System::String::Format("IntersectionEvent2D(point=({0}, {1}), seg1={2}, seg2={3})",
                                  _native->Point.x(), _native->Point.y(),
                                  static_cast<int>(_native->SegmentIds[0]), static_cast<int>(_native->SegmentIds[1]));
}

}  // namespace GeomPP
