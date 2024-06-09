# Work in Progress 

## Done

#### Init the repo
- build on linux
- build on windows
- set up a lib folder and a test folder


#### 2D geometry
- Point2D, Vector2D, Line2D, Shape2D for return type of geometrical operations, tests
- Line2D::contains(p), intersects(line), intersection(line), tests
- Ray2D, contains(p), intersection(line), intersection(ray), tests
- LineSegment2D, contains(p), intersection(line), intersection(ray), intersection(line_seg), tests
- Line2D,Ray2D,LineSegment2D::distance(p), tests
- LineSegment2D::interpolate(%)->p, location(p)->%, tests
- Polyline2D, contains(p), distance(p), tests
- Polyline2D::Wkt, tests

#### test and build infrastructure
- github actions: run tests on merge 
- ::FromWkt(str)->Shape2D, ::ToWkt()->str parsing and serializing 
- ::FromFile(wkb)->Shape2D, ::ToFile()->wkb parsing and serializing 
- test cases possible in `.wkt` files formats in `geompp_tests/res` folder
- Docker based dev environment: Linux image


#### graphic demos
- Set up a window to display some line segments in OpenGL



## Next

#### test and build infrastructure
- Docker based dev enviroment: Windows image

#### graphic demos
- add keys to move around with the camera
- add an input box to type in a new geometry
- add a key to delete a geometry 




## Backlog to do

#### test and build infrastructure 
- On Visual Studio, use the test runner to run specific tests
- Build on windows via command line (install cmake and g++ on windows, use PowerShell)

#### 2D geometry
- Polyline2D::interpolate(%)->p, location(p)->%, tests
- Polyline2D::intersects(line, ray, line_seg, polyline), tests
- Triangle2D, Triangle2D::contains(p), tests
- Triangle2D::Wkt, tests
- Triangle2D::intersects(line, ray, line_seg), tests
- Polygon2D, contains(p), tests
- Polygon2D::intersects(line, ray, line_seg, triangle, polygon), tests
- List<Point2D>::convex_hull()->polygon, tests

#### more build infrastructure 
- python bindings 
- write interface for python
- write some sample code
- write some python tests

#### 3D geometry 
- Point3D, Vector3D, Line3D, Shape3D for return type of geometrical operations, tests
- Line3D::contains(p), Line3D::intersects(line), Line3D::intersection(line), tests
- Ray3D, contains(p), intersection(line, ray), tests
- LineSegment3D, contains(p), ntersection(line, ray, line_seg), tests
- Line3D, Ray3D, LineSegment3D::distance(p), tests
- LineSegment3D::interpolate(%)->p, LineSegment3D::location(p)->%, tests
- Polyline3D, Polyline3D::contains(p), Polyline3D::distance(p), tests
- Polyline3D::interpolate(%)->p, Polyline3D::locartion(p)->%, tests
- Polyline3D::intersects(line, ray, line_seg, polyline), tests
- Plane, signed_distance, distance, project_onto, project_into, tests
- Triangle3D, Triangle3D::contains(p), tests
- Triangle3D::intersects(line, ray, line_seg, triangle), tests
- Polygon3D, Polygon3D::contains(p), tests
- Polygon3D::intersects(line, ray, line_seg, triangle, polygon), tests
- List<Point3D>::convex_hull()->polygon, tests

#### more build infrastructure 
- C# interface for python
- write some sample code
- write some c# tests

#### more geometrical operations 2D, 3D
- Line2D::overlaps(line), Line2D::overlap(line), tests
- Ray2D::overlaps(line, ray), tests
- LineSegment2D::overlaps(line, ray, line_seg), tests
- Polyline2D::overlaps(line, ray, line_seg, polyline), tests
- Triangle2D::overlaps(line, ray, line_seg, polyline, triangle), tests (overlap for triangle is "adjacency")
- Polygon2D::overlaps(line, ray, line_seg, polyline, triangle, polygon), tests (overlap for polygon is "adjacency")
- Triangle2D::adjacent(line, ray, line_seg, polyline, triangle), tests (same as overlap)
- Polygon2D::adjacent(line, ray, line_seg, polyline, triangle, polygon), tests (same as overlap)

- Line3D::overlaps(line), Line3D::overlap(line), tests
- Ray3D::overlaps(line, ray), tests
- LineSegment3D::overlaps(line, ray, line_seg), tests
- Polyline3D::overlaps(line, ray, line_seg, polyline), tests
- Triangle3D::overlaps(line, ray, line_seg, polyline, triangle), tests (overlap for triangle is "coplanar, intersecting in 2D")
- Polygon3D::overlaps(line, ray, line_seg, polyline, triangle, polygon), tests (overlap for polygon is "coplanar, intersecting in 2D")
- Triangle3D::adjacent(line, ray, line_seg, polyline, triangle), tests (one side in common, no intersection)
- Polygon3D::adjacent(line, ray, line_seg, polyline, triangle, polygon), tests (one or more sides in common, no intersection)

#### add 2D meshing
- Mesh2D class (set of triangles), tests
- Polygon2D::triangulate()->mesh, tests
- Mesh2D::polygonize()->polygon, tests

#### add 3D meshing 
- Mesh3D class (set of triangles), tests
- Polygon3D::triangulate()->mesh, tests
- Mesh3D::polygonize()->polygon, tests

#### add polygon clipping 
- Triangle2D::intersection(triangle), test
- Polygon2D::intersection(triangle, polygon), test
- Triangle3D::intersection(triangle), test
- Polygon3D::intersection(triangle, polygon), test
- Triangle3D::overlap(triangle), test
- Polygon3D::overlap(triangle, polygon), test