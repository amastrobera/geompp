#include "LVSParser.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"
#include "Point3D.hpp"
#include "Line3D.hpp"
#include "Ray3D.hpp"
#include "LineSegment3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

LVSParser::LVSParser(geompp::LVSParser* native)
    : _native(native) {}

LVSParser::~LVSParser() {
    delete _native;
    _native = nullptr;
}

LVSParser::!LVSParser() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

LVSParser^ LVSParser::Open(System::String^ filePath) {
    return gcnew LVSParser(
        new geompp::LVSParser(geompp::LVSParser::Open(marshal_as<std::string>(filePath))));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool LVSParser::HasNext() {
    return _native->HasNext();
}

System::Object^ LVSParser::Next() {
    auto result = _native->Next();
    if (!result.has_value()) return nullptr;

    auto& val = result.value();
    if (std::holds_alternative<geompp::Point2D>(val))
        return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(val)));
    if (std::holds_alternative<geompp::Line2D>(val))
        return gcnew Line2D(new geompp::Line2D(std::get<geompp::Line2D>(val)));
    if (std::holds_alternative<geompp::Ray2D>(val))
        return gcnew Ray2D(new geompp::Ray2D(std::get<geompp::Ray2D>(val)));
    if (std::holds_alternative<geompp::LineSegment2D>(val))
        return gcnew LineSegment2D(new geompp::LineSegment2D(std::get<geompp::LineSegment2D>(val)));
    if (std::holds_alternative<geompp::Point3D>(val))
        return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(val)));
    if (std::holds_alternative<geompp::Line3D>(val))
        return gcnew Line3D(new geompp::Line3D(std::get<geompp::Line3D>(val)));
    if (std::holds_alternative<geompp::Ray3D>(val))
        return gcnew Ray3D(new geompp::Ray3D(std::get<geompp::Ray3D>(val)));
    if (std::holds_alternative<geompp::LineSegment3D>(val))
        return gcnew LineSegment3D(new geompp::LineSegment3D(std::get<geompp::LineSegment3D>(val)));
    return nullptr;
}

System::String^ LVSParser::GetFilePath() {
    return gcnew System::String(_native->GetFilePath().c_str());
}

System::String^ LVSParser::ToWkt(System::Object^ item) {
    if (item == nullptr) return nullptr;

    // Reconstruct a native ReturnSet from the managed wrapper and call native ToWkt
    if (Point2D^ p = dynamic_cast<Point2D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*p->_native)).c_str());
    if (Line2D^ l = dynamic_cast<Line2D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*l->_native)).c_str());
    if (Ray2D^ r = dynamic_cast<Ray2D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*r->_native)).c_str());
    if (LineSegment2D^ s = dynamic_cast<LineSegment2D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*s->_native)).c_str());
    if (Point3D^ p = dynamic_cast<Point3D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*p->_native)).c_str());
    if (Line3D^ l = dynamic_cast<Line3D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*l->_native)).c_str());
    if (Ray3D^ r = dynamic_cast<Ray3D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*r->_native)).c_str());
    if (LineSegment3D^ s = dynamic_cast<LineSegment3D^>(item))
        return gcnew System::String(
            geompp::LVSParser::ToWkt(geompp::LVSParser::ReturnSet(*s->_native)).c_str());
    return nullptr;
}

System::String^ LVSParser::ToString() {
    return gcnew System::String(("LVSParser[" + _native->GetFilePath() + "]").c_str());
}

}  // namespace GeomPP
