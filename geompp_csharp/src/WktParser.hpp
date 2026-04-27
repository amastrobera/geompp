#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "wkt_parser.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;
ref class LineSegment2D;
ref class Point3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;

public ref class WktParser {
public:
    ~WktParser();
    !WktParser();

    // Factory method (private constructor — use Open)
    static WktParser^ Open(System::String^ filePath);

    bool HasNext();

    // Returns one of: Point2D^, Line2D^, Ray2D^, LineSegment2D^,
    //                 Point3D^, Line3D^, Ray3D^, LineSegment3D^, or nullptr
    System::Object^ Next();

    System::String^ GetFilePath();

    static System::String^ ToWkt(System::Object^ item);

    virtual System::String^ ToString() override;

internal:
    WktParser(geompp::WktParser* native);
    geompp::WktParser* _native;
};

}  // namespace GeomPP
