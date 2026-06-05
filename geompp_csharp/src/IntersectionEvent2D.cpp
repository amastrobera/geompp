#include "IntersectionEvent2D.hpp"

#include "Point2D.hpp"

namespace GeomPP {

IntersectionEvent2D::IntersectionEvent2D(geompp::IntersectionEvent2D* native) : _native(native) {}

IntersectionEvent2D::~IntersectionEvent2D() { this->!IntersectionEvent2D(); }

IntersectionEvent2D::!IntersectionEvent2D() {
    delete _native;
    _native = nullptr;
}

Point2D^ IntersectionEvent2D::Point::get() {
    return gcnew Point2D(new geompp::Point2D(_native->Point));
}

int IntersectionEvent2D::SegmentId1::get() { return static_cast<int>(_native->SegmentId1); }

int IntersectionEvent2D::SegmentId2::get() { return static_cast<int>(_native->SegmentId2); }

System::String^ IntersectionEvent2D::ToString() {
    return System::String::Format("IntersectionEvent2D(point=({0}, {1}), seg1={2}, seg2={3})",
                                  _native->Point.x(), _native->Point.y(),
                                  static_cast<int>(_native->SegmentId1), static_cast<int>(_native->SegmentId2));
}

}  // namespace GeomPP
