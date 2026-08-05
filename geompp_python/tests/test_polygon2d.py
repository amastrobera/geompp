"""
polygon2d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx, assert_no_polygon_vertex_hangs_on_triangle_edge, assert_half_edges_are_manifold


class TestPolygon2D:
    @pytest.fixture
    def square(self):
        pts = [
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ]
        return geompp.Polygon2D.make(pts)

    def test_size(self, square):
        assert len(square) == 4

    def test_getitem(self, square):
        assert approx(square[0].x, 0) and approx(square[0].y, 0)
        assert approx(square[3].x, 0) and approx(square[3].y, 1)

    def test_iteration(self, square):
        pts = list(square)
        assert len(pts) == 4
        assert all(isinstance(p, geompp.Point2D) for p in pts)

    def test_index_error(self, square):
        with pytest.raises(IndexError):
            _ = square[10]

    def test_cw_outer_throws(self):
        with pytest.raises(Exception):
            geompp.Polygon2D.make([
                geompp.Point2D(0, 0), geompp.Point2D(0, 4),
                geompp.Point2D(4, 4), geompp.Point2D(4, 0),
            ])

    def test_with_holes_valid(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        p = geompp.Polygon2D.make(outer, [hole])
        assert len(p) == 4

    def test_with_holes_ccw_hole_throws(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        ccw_hole = [
            geompp.Point2D(1, 1), geompp.Point2D(3, 1),
            geompp.Point2D(3, 3), geompp.Point2D(1, 3),
        ]
        with pytest.raises(Exception):
            geompp.Polygon2D.make(outer, [ccw_hole])

    def test_with_holes_self_intersecting_hole_throws(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(6, 0),
            geompp.Point2D(6, 6), geompp.Point2D(0, 6),
        ]
        # Bowtie hole (self-crossing): (1,1)->(3,1)->(1,3)->(3,3).
        bowtie_hole = [
            geompp.Point2D(1, 1), geompp.Point2D(3, 1),
            geompp.Point2D(1, 3), geompp.Point2D(3, 3),
        ]
        with pytest.raises(Exception):
            geompp.Polygon2D.make(outer, [bowtie_hole])

    def test_with_holes_two_holes_overlap_throws(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(10, 10), geompp.Point2D(0, 10),
        ]
        hole_a = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 5),
            geompp.Point2D(5, 5), geompp.Point2D(5, 1),
        ]
        hole_b = [
            geompp.Point2D(3, 3), geompp.Point2D(3, 7),
            geompp.Point2D(7, 7), geompp.Point2D(7, 3),
        ]  # overlaps hole_a
        with pytest.raises(Exception):
            geompp.Polygon2D.make(outer, [hole_a, hole_b])

    def test_with_holes_two_holes_touch_at_vertex_does_not_throw(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(10, 10), geompp.Point2D(0, 10),
        ]
        # Two squares touching diagonally at the single shared corner (5,5).
        hole_a = [
            geompp.Point2D(3, 3), geompp.Point2D(3, 5),
            geompp.Point2D(5, 5), geompp.Point2D(5, 3),
        ]
        hole_b = [
            geompp.Point2D(5, 5), geompp.Point2D(5, 7),
            geompp.Point2D(7, 7), geompp.Point2D(7, 5),
        ]
        p = geompp.Polygon2D.make(outer, [hole_a, hole_b])
        assert len(p.holes()) == 2

    def test_with_holes_two_holes_disjoint_does_not_throw(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(10, 10), geompp.Point2D(0, 10),
        ]
        hole_a = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        hole_b = [
            geompp.Point2D(6, 6), geompp.Point2D(6, 8),
            geompp.Point2D(8, 8), geompp.Point2D(8, 6),
        ]
        p = geompp.Polygon2D.make(outer, [hole_a, hole_b])
        assert len(p.holes()) == 2

    def test_with_holes_hole_strikes_through_outer_throws(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(10, 10), geompp.Point2D(0, 10),
        ]
        # Hole straddles the outer boundary at x=10: half inside, half poking out.
        hole = [
            geompp.Point2D(8, 4), geompp.Point2D(8, 6),
            geompp.Point2D(12, 6), geompp.Point2D(12, 4),
        ]
        with pytest.raises(Exception):
            geompp.Polygon2D.make(outer, [hole])

    def test_with_holes_hole_flush_against_outer_edge_does_not_throw(self):
        # Hole touches the outer boundary along a full edge, not just a vertex — a normal "notched
        # corner" shape, must not be rejected as "striking through".
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(2, 2), geompp.Point2D(2, 4),
            geompp.Point2D(4, 4), geompp.Point2D(4, 2),
        ]
        p = geompp.Polygon2D.make(outer, [hole])
        assert len(p.holes()) == 1

    def test_with_holes_hole_entirely_outside_outer_throws(self):
        # Hole never crosses or touches the outer boundary, so the strikes-through check alone lets it
        # slip by, but it sits wholly outside the outer square.
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(10, 10), geompp.Point2D(0, 10),
        ]
        hole = [
            geompp.Point2D(20, 20), geompp.Point2D(20, 22),
            geompp.Point2D(22, 22), geompp.Point2D(22, 20),
        ]
        with pytest.raises(Exception):
            geompp.Polygon2D.make(outer, [hole])

    def test_area_self_intersecting_outer_with_hole(self):
        # Bowtie outer (lobes 4.0 + 1.0 = 5.0 total covered area) with a small 1x0.3 hole safely inside
        # the larger lobe, away from the self-crossing — exercises Area()'s slow path together with its
        # direct per-hole subtraction.
        p = geompp.Polygon2D.make(
            [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(1, 3), geompp.Point2D(3, 3)],
            [[geompp.Point2D(1.5, 0.2), geompp.Point2D(1.5, 0.5),
              geompp.Point2D(2.5, 0.5), geompp.Point2D(2.5, 0.2)]],
        )
        assert not p.is_simple()
        assert approx(p.area(), 4.7)

    def test_centroid_self_intersecting_outer_matches_simplify_weighted_average(self):
        # Cross-validated against an independently-computed area-weighted average over simplify()'s
        # pieces, rather than hand-deriving the expected centroid.
        p = geompp.Polygon2D.make(
            [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(1, 3), geompp.Point2D(3, 3)])
        assert not p.is_simple()

        total_area, wx, wy = 0.0, 0.0, 0.0
        for piece in p.simplify():
            a = piece.area()
            c = piece.centroid()
            total_area += a
            wx += a * c.x
            wy += a * c.y

        c = p.centroid()
        assert approx(c.x, wx / total_area) and approx(c.y, wy / total_area)

    def test_centroid_square(self, square):
        c = square.centroid()
        assert approx(c.x, 0.5) and approx(c.y, 0.5)

    def test_centroid_triangle(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3)
        ])
        c = p.centroid()
        assert approx(c.x, 4/3) and approx(c.y, 1.0)

    def test_centroid_with_hole(self):
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        c = geompp.Polygon2D.make(outer, [hole]).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 2.0)

    def test_perimeter_square(self, square):
        # 1×1 square → perimeter = 4
        assert approx(square.perimeter_size(), 4.0)

    def test_perimeter_rectangle(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(3, 0),
            geompp.Point2D(3, 4), geompp.Point2D(0, 4),
        ])
        assert approx(p.perimeter_size(), 14.0)

    def test_perimeter_triangle(self):
        # 3-4-5 right triangle → perimeter = 12
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(0, 3),
        ])
        assert approx(p.perimeter_size(), 12.0)

    def test_to_segments(self, square):
        segs = square.to_segments()
        assert len(segs) == 4  # N vertices → N segments (closed)
        assert all(isinstance(s, geompp.LineSegment2D) for s in segs)
        assert all(approx(s.length(), 1.0) for s in segs)
        first = segs[0]
        assert first.first.almost_equals(geompp.Point2D(0, 0))
        assert first.last.almost_equals(geompp.Point2D(1, 0))
        last = segs[3]
        assert last.first.almost_equals(geompp.Point2D(0, 1))
        assert last.last.almost_equals(geompp.Point2D(0, 0))

    def test_almost_equals(self, square):
        sq2 = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])
        assert square.almost_equals(sq2)
        other = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(2, 0),
            geompp.Point2D(2, 2), geompp.Point2D(0, 2),
        ])
        assert not square.almost_equals(other)

    def test_to_file_from_file(self, square):
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            square.to_file(path)
            assert os.path.exists(path)
            sq2 = geompp.Polygon2D.from_file(path)
            assert square.almost_equals(sq2)
        finally:
            os.unlink(path)

    def test_contains(self, square):
        # fixture: unit square (0,0)-(1,0)-(1,1)-(0,1)
        assert square.contains(geompp.Point2D(0.5, 0.5))    # center
        assert square.contains(geompp.Point2D(0.1, 0.1))    # near corner
        assert square.contains(geompp.Point2D(0.9, 0.9))    # near opposite corner
        assert not square.contains(geompp.Point2D(-0.1, 0.5))
        assert not square.contains(geompp.Point2D(1.1, 0.5))
        assert not square.contains(geompp.Point2D(0.5, -0.1))
        assert not square.contains(geompp.Point2D(0.5, 1.1))
        assert not square.contains(geompp.Point2D(5, 5))

        # polygon with hole: 4×4 outer, 2×2 centred hole
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        poly = geompp.Polygon2D.make(outer, [hole])
        assert poly.contains(geompp.Point2D(0.5, 0.5))   # inside outer, outside hole
        assert poly.contains(geompp.Point2D(3.5, 3.5))   # inside outer, outside hole
        assert not poly.contains(geompp.Point2D(2, 2))   # inside hole
        assert not poly.contains(geompp.Point2D(-1, 2))  # outside outer

        # boundary — vertices and edge midpoints are included
        assert square.contains(geompp.Point2D(0,   0))    # bottom-left vertex
        assert square.contains(geompp.Point2D(1,   0))    # bottom-right vertex
        assert square.contains(geompp.Point2D(1,   1))    # top-right vertex
        assert square.contains(geompp.Point2D(0,   1))    # top-left vertex
        assert square.contains(geompp.Point2D(0.5, 0))    # bottom edge midpoint
        assert square.contains(geompp.Point2D(1,   0.5))  # right edge midpoint
        assert square.contains(geompp.Point2D(0.5, 1))    # top edge midpoint
        assert square.contains(geompp.Point2D(0,   0.5))  # left edge midpoint
        assert poly.contains(geompp.Point2D(2,   0))      # outer bottom edge
        assert poly.contains(geompp.Point2D(4,   2))      # outer right edge
        assert poly.contains(geompp.Point2D(2,   1))      # hole bottom edge
        assert poly.contains(geompp.Point2D(1,   2))      # hole left edge

    def test_is_on_perimeter(self, square):
        # vertices
        assert square.is_on_perimeter(geompp.Point2D(0,   0))
        assert square.is_on_perimeter(geompp.Point2D(1,   0))
        assert square.is_on_perimeter(geompp.Point2D(1,   1))
        assert square.is_on_perimeter(geompp.Point2D(0,   1))
        # edge midpoints
        assert square.is_on_perimeter(geompp.Point2D(0.5, 0))
        assert square.is_on_perimeter(geompp.Point2D(1,   0.5))
        assert square.is_on_perimeter(geompp.Point2D(0.5, 1))
        assert square.is_on_perimeter(geompp.Point2D(0,   0.5))
        # interior and outside must be False
        assert not square.is_on_perimeter(geompp.Point2D(0.5, 0.5))
        assert not square.is_on_perimeter(geompp.Point2D(-0.1, 0.5))
        assert not square.is_on_perimeter(geompp.Point2D(1.1,  0.5))
        # hole boundary
        outer = [
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ]
        hole = [
            geompp.Point2D(1, 1), geompp.Point2D(1, 3),
            geompp.Point2D(3, 3), geompp.Point2D(3, 1),
        ]
        poly = geompp.Polygon2D.make(outer, [hole])
        assert poly.is_on_perimeter(geompp.Point2D(2, 0))  # outer bottom
        assert poly.is_on_perimeter(geompp.Point2D(4, 2))  # outer right
        assert poly.is_on_perimeter(geompp.Point2D(2, 1))  # hole bottom
        assert poly.is_on_perimeter(geompp.Point2D(1, 2))  # hole left
        assert not poly.is_on_perimeter(geompp.Point2D(0.5, 0.5))  # interior
        assert not poly.is_on_perimeter(geompp.Point2D(2,   2))    # inside hole

    # NOTE: is_simple() correctness rides on the (currently provisional) sweep-line comparator; these encode
    # the intended behaviour and should be re-verified once the real sweep-status ordering is in place.
    def test_is_simple_smoke(self, square):
        assert isinstance(square.is_simple(), bool)

    def test_is_simple_convex_square_true(self, square):
        assert square.is_simple()

    def test_is_simple_self_intersecting_false(self):
        # CCW (positive area) but edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(1, 3), geompp.Point2D(3, 3),
        ])
        assert not p.is_simple()

    def test_from_wkt_whitespace(self):
        sq = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])
        assert sq.almost_equals(geompp.Polygon2D.from_wkt("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))"))
        assert sq.almost_equals(geompp.Polygon2D.from_wkt("POLYGON ((  0 0  ,  1 0  ,  1 1  ,  0 1  ,  0 0  ))"))

class TestPolygon2DIntersection:
    @pytest.fixture
    def sq(self):
        return geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(1, 0),
            geompp.Point2D(1, 1), geompp.Point2D(0, 1),
        ])

    # ── Line2D ───────────────────────────────────────────────────────────────

    def test_intersection_line_passes_through(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(0, 0.5), geompp.Point2D(1, 0.5))
        result = sq.intersection(line)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1
        seg = result[0]
        endpoints = {(round(seg.first.x, 3), round(seg.first.y, 3)),
                     (round(seg.last.x,  3), round(seg.last.y,  3))}
        assert (0.0, 0.5) in endpoints
        assert (1.0, 0.5) in endpoints

    def test_intersection_line_misses(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(5, 0), geompp.Point2D(5, 1))
        assert sq.intersection(line) is None

    def test_intersects_line_true(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(0.5, -1), geompp.Point2D(0.5, 2))
        assert sq.intersects(line)

    def test_intersects_line_false(self, sq):
        line = geompp.Line2D.make(geompp.Point2D(5, 0), geompp.Point2D(5, 1))
        assert not sq.intersects(line)

    # ── Ray2D ────────────────────────────────────────────────────────────────

    def test_intersection_ray_hits(self, sq):
        ray = geompp.Ray2D.make(geompp.Point2D(-1, 0.5), geompp.Vector2D(1, 0))
        result = sq.intersection(ray)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1

    def test_intersection_ray_pointing_away(self, sq):
        ray = geompp.Ray2D.make(geompp.Point2D(5, 0.5), geompp.Vector2D(1, 0))
        assert sq.intersection(ray) is None

    def test_intersection_ray_origin_inside(self, sq):
        # Origin inside polygon, ray clips from origin to exit.
        ray = geompp.Ray2D.make(geompp.Point2D(0.5, 0.5), geompp.Vector2D(1, 0))
        result = sq.intersection(ray)
        assert result is not None
        assert isinstance(result, list)
        seg = result[0]
        endpoints = {(round(seg.first.x, 3), round(seg.first.y, 3)),
                     (round(seg.last.x,  3), round(seg.last.y,  3))}
        assert (0.5, 0.5) in endpoints

    def test_intersects_ray(self, sq):
        assert sq.intersects(geompp.Ray2D.make(geompp.Point2D(-1, 0.5), geompp.Vector2D(1, 0)))
        assert not sq.intersects(geompp.Ray2D.make(geompp.Point2D(5, 0.5), geompp.Vector2D(1, 0)))

    # ── LineSegment2D ─────────────────────────────────────────────────────────

    def test_intersection_segment_pierces(self, sq):
        seg = geompp.LineSegment2D.make(geompp.Point2D(-0.5, 0.5), geompp.Point2D(1.5, 0.5))
        result = sq.intersection(seg)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1
        chord = result[0]
        endpoints = {(round(chord.first.x, 3), round(chord.first.y, 3)),
                     (round(chord.last.x,  3), round(chord.last.y,  3))}
        assert (0.0, 0.5) in endpoints
        assert (1.0, 0.5) in endpoints

    def test_intersection_segment_too_short(self, sq):
        seg = geompp.LineSegment2D.make(geompp.Point2D(-2, 0.5), geompp.Point2D(-0.5, 0.5))
        assert sq.intersection(seg) is None

    def test_intersection_segment_entirely_inside(self, sq):
        seg = geompp.LineSegment2D.make(geompp.Point2D(0.2, 0.5), geompp.Point2D(0.8, 0.5))
        result = sq.intersection(seg)
        assert result is not None
        assert isinstance(result, list)
        assert len(result) == 1

    def test_intersects_segment(self, sq):
        assert sq.intersects(geompp.LineSegment2D.make(geompp.Point2D(-0.5, 0.5), geompp.Point2D(1.5, 0.5)))
        assert not sq.intersects(geompp.LineSegment2D.make(geompp.Point2D(-2, 0.5), geompp.Point2D(-0.5, 0.5)))

    # ── Polygon2D boolean ops ───────────────────────────────────────────────

    @pytest.fixture
    def overlapping(self):
        return geompp.Polygon2D.make([
            geompp.Point2D(0.5, 0.5), geompp.Point2D(1.5, 0.5),
            geompp.Point2D(1.5, 1.5), geompp.Point2D(0.5, 1.5),
        ])

    def test_intersects_polygon(self, sq, overlapping):
        assert sq.intersects(overlapping)
        disjoint = geompp.Polygon2D.make([
            geompp.Point2D(5, 5), geompp.Point2D(6, 5), geompp.Point2D(6, 6), geompp.Point2D(5, 6),
        ])
        assert not sq.intersects(disjoint)

    def test_union(self, sq, overlapping):
        result = sq.union(overlapping)
        assert len(result) == 1
        assert abs(result[0].area() - 1.75) < 1e-6

    def test_intersection_polygon(self, sq, overlapping):
        result = sq.intersection(overlapping)
        assert len(result) == 1
        assert abs(result[0].area() - 0.25) < 1e-6

    def test_difference(self, sq, overlapping):
        result = sq.difference(overlapping)
        assert len(result) == 1
        assert abs(result[0].area() - 0.75) < 1e-6

    def test_xor(self, sq, overlapping):
        result = sq.xor(overlapping)
        assert len(result) == 1
        assert result[0].has_holes()

class TestPolygon2DConvexHull:
    def _star(self):
        return geompp.Polygon2D.make([
            geompp.Point2D(-1,  2), geompp.Point2D(-3,  1),
            geompp.Point2D(-1, -1), geompp.Point2D(-2, -4),
            geompp.Point2D( 0, -1), geompp.Point2D( 3, -3),
            geompp.Point2D( 2, -1), geompp.Point2D( 4,  2),
            geompp.Point2D( 2,  1), geompp.Point2D( 0,  5),
        ])

    def test_convex_hull_star_is_pentagon(self):
        hull = self._star().convex_hull()
        assert hull.size() == 5

    def test_convex_hull_convex_polygon_unchanged(self):
        square = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ])
        hull = square.convex_hull()
        assert hull.size() == 4

    def test_perimeter_round_trip(self):
        pts = [geompp.Point2D(0, 0), geompp.Point2D(3, 0),
               geompp.Point2D(3, 3), geompp.Point2D(0, 3)]
        poly = geompp.Polygon2D.make(pts)
        back = poly.perimeter()
        assert len(back) == 4
        for orig, restored in zip(pts, back):
            assert approx(orig.x, restored.x) and approx(orig.y, restored.y)

class TestPolygon2DHoles:
    def test_no_holes(self):
        poly = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 4), geompp.Point2D(0, 4),
        ])
        assert poly.has_holes() is False
        assert poly.holes() == []

    def test_with_hole(self):
        outer = [geompp.Point2D(0, 0), geompp.Point2D(4, 0),
                 geompp.Point2D(4, 4), geompp.Point2D(0, 4)]
        hole = [geompp.Point2D(1, 1), geompp.Point2D(1, 3),
                geompp.Point2D(3, 3), geompp.Point2D(3, 1)]
        poly = geompp.Polygon2D.make(outer, [hole])
        assert poly.has_holes() is True
        holes = poly.holes()
        assert len(holes) == 1
        assert len(holes[0]) == 4
        for orig, restored in zip(hole, holes[0]):
            assert approx(orig.x, restored.x) and approx(orig.y, restored.y)

class TestPolygon2DIsConvex:
    def test_square_is_convex(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0,0), geompp.Point2D(1,0),
            geompp.Point2D(1,1), geompp.Point2D(0,1)])
        assert p.is_convex()

    def test_concave_not_convex(self):
        # Concave polygon (dent)
        p = geompp.Polygon2D.make([
            geompp.Point2D(0,0), geompp.Point2D(4,0),
            geompp.Point2D(4,4), geompp.Point2D(2,2), geompp.Point2D(0,4)])
        assert not p.is_convex()

    def test_with_hole_not_convex(self):
        outer = [geompp.Point2D(0,0), geompp.Point2D(4,0),
                 geompp.Point2D(4,4), geompp.Point2D(0,4)]
        # holes must be CW (reversed winding)
        hole  = [geompp.Point2D(1,1), geompp.Point2D(1,2),
                 geompp.Point2D(2,2), geompp.Point2D(2,1)]
        p = geompp.Polygon2D.make(outer, [hole])
        assert not p.is_convex()

class TestPolygon2DSimplify:
    def _bowtie(self):
        # Self-intersecting: A(0,0), B(4,0), C(1,3), D(3,3) — edges B→C and D→A cross at (2,2).
        return geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(1, 3), geompp.Point2D(3, 3)])

    def test_already_simple_returns_self(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(2, 0),
            geompp.Point2D(2, 2), geompp.Point2D(0, 2)])
        result = p.simplify()
        assert len(result) == 1
        assert result[0].almost_equals(p)

    def test_bowtie_yields_two_polygons(self):
        result = self._bowtie().simplify()
        assert len(result) == 2

    def test_bowtie_results_are_simple(self):
        for poly in self._bowtie().simplify():
            assert poly.is_simple()

    def test_bowtie_areas_sum(self):
        result = self._bowtie().simplify()
        total = sum(p.area() for p in result)
        assert abs(total - 5.0) < 0.01

    def test_wide_bowtie_all_simple(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(10, 0),
            geompp.Point2D(2, 6), geompp.Point2D(8, 6)])
        assert not p.is_simple()
        result = p.simplify()
        assert len(result) == 2
        for poly in result:
            assert poly.is_simple()

class TestPolygon2DTriangulate:
    def test_convex_quad_two_triangles_full_area(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 2), geompp.Point2D(0, 2)])
        triangles = p.triangulate()
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), p.area())
        assert_no_polygon_vertex_hangs_on_triangle_edge(p, triangles)
        assert_half_edges_are_manifold(triangles)

    def test_concave_polygon_correct_area_and_count(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 4),
            geompp.Point2D(2, 1), geompp.Point2D(0, 4)])
        triangles = p.triangulate()
        assert len(triangles) == 3
        assert approx(sum(t.area() for t in triangles), p.area())
        assert_no_polygon_vertex_hangs_on_triangle_edge(p, triangles)
        assert_half_edges_are_manifold(triangles)

    def test_monotone_polygon_strategy_raises(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 2), geompp.Point2D(0, 2)])
        with pytest.raises(RuntimeError):
            p.triangulate(geompp.TriangulationStrategy.MonotonePolygon)

    def test_delaunay_strategy_raises(self):
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0),
            geompp.Point2D(4, 2), geompp.Point2D(0, 2)])
        with pytest.raises(RuntimeError):
            p.triangulate(geompp.TriangulationStrategy.Delaunay)

    def test_reflex_vertex_on_non_adjacent_diagonal_stays_inside_polygon(self):
        # Regression test: this L-shape's reflex vertex (2, 2) sits exactly on the diagonal between
        # the non-adjacent vertices (0, 4) and (4, 0) (all three satisfy x + y == 4). A strict
        # point-in-triangle ear-validity check missed this collinear case and accepted a diagonal
        # that actually exits the polygon through the notch.
        p = geompp.Polygon2D.make([
            geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 2),
            geompp.Point2D(2, 2), geompp.Point2D(2, 4), geompp.Point2D(0, 4)])
        triangles = p.triangulate()
        assert len(triangles) == 4
        total_area = 0.0
        for t in triangles:
            assert p.contains(t.centroid()), f"triangle {t.to_wkt()} strays outside the polygon"
            total_area += t.area()
        assert approx(total_area, p.area())
        assert_no_polygon_vertex_hangs_on_triangle_edge(p, triangles)
        assert_half_edges_are_manifold(triangles)

    def test_five_pointed_star_triangle_areas_sum_to_polygon_area(self):
        # 5-pointed star (same shape used in visual_doc_and_sample_code.md's Triangulation example) --
        # concave, with a reflex vertex at each of its 5 inner corners.
        p = geompp.Polygon2D.make([
            geompp.Point2D(3.0, 6.0), geompp.Point2D(2.29, 3.97), geompp.Point2D(0.15, 3.93),
            geompp.Point2D(1.86, 2.63), geompp.Point2D(1.24, 0.57), geompp.Point2D(3.0, 1.8),
            geompp.Point2D(4.76, 0.57), geompp.Point2D(4.14, 2.63), geompp.Point2D(5.85, 3.93),
            geompp.Point2D(3.71, 3.97)])
        triangles = p.triangulate()
        assert len(triangles) == 8
        total_area = 0.0
        for t in triangles:
            assert p.contains(t.centroid()), f"triangle {t.to_wkt()} strays outside the polygon"
            total_area += t.area()
        assert approx(total_area, p.area())
        assert_no_polygon_vertex_hangs_on_triangle_edge(p, triangles)
        assert_half_edges_are_manifold(triangles)

    def test_five_pointed_star_triangles_exactly_tile_with_no_overlap(self):
        # Stronger companion to the area-sum test above: proves an exact tiling two ways: (1) no pair
        # of triangles shares more than an edge/vertex, and (2) a dense sampling grid over the
        # bounding box confirms "inside the polygon" and "inside some triangle" are the same set of
        # points (skipping a thin margin around every edge, since Polygon2D.contains()/
        # Triangle2D.contains() use different algorithms and can disagree right at a shared boundary
        # -- that's boundary-inclusion noise, not a real gap/overlap).
        p = geompp.Polygon2D.make([
            geompp.Point2D(3.0, 6.0), geompp.Point2D(2.29, 3.97), geompp.Point2D(0.15, 3.93),
            geompp.Point2D(1.86, 2.63), geompp.Point2D(1.24, 0.57), geompp.Point2D(3.0, 1.8),
            geompp.Point2D(4.76, 0.57), geompp.Point2D(4.14, 2.63), geompp.Point2D(5.85, 3.93),
            geompp.Point2D(3.71, 3.97)])
        triangles = p.triangulate()
        assert len(triangles) == 8

        # Triangle2D.intersection(Triangle2D) isn't bound in Python (only the Line2D overload is),
        # so go through to_polygon().intersection() instead -- Polygon2D.intersection() is bound.
        for i in range(len(triangles)):
            for j in range(i + 1, len(triangles)):
                pieces = triangles[i].to_polygon().intersection(triangles[j].to_polygon())
                overlap_area = sum(piece.area() for piece in pieces)
                assert overlap_area < 1e-9, f"triangles {i} and {j} overlap with real area"

        edges = []
        perimeter = p.perimeter()
        for k in range(len(perimeter)):
            edges.append(geompp.LineSegment2D.make(perimeter[k], perimeter[(k + 1) % len(perimeter)]))
        for t in triangles:
            v0, v1, v2 = t.vertices
            edges.append(geompp.LineSegment2D.make(v0, v1))
            edges.append(geompp.LineSegment2D.make(v1, v2))
            edges.append(geompp.LineSegment2D.make(v2, v0))

        margin = 0.02
        steps = 97  # prime step count so the grid never aligns with the star's straight edges
        mismatches = 0
        skipped = 0
        for ix in range(steps):
            for iy in range(steps):
                x = 6.0 * (ix + 0.37) / steps
                y = 6.0 * (iy + 0.61) / steps
                pt = geompp.Point2D(x, y)
                if any(e.distance_to(pt) < margin for e in edges):
                    skipped += 1
                    continue
                in_poly = p.contains(pt)
                in_any_tri = any(t.contains(pt) for t in triangles)
                if in_poly != in_any_tri:
                    mismatches += 1
        assert skipped < steps * steps, "every grid point was too close to an edge -- margin too large?"
        assert mismatches == 0

    def test_comb_polygon_produces_correct_area_and_count(self):
        # A 3-tooth "comb" -- the classic adversarial shape for naive ear-clipping (deep, narrow
        # notches between tall teeth). Unlike the other shapes tested here, this one genuinely needs
        # multiple laps around the ring before it fully triangulates (see the visual doc's
        # Triangulation section for why).
        p = geompp.Polygon2D.make([
            geompp.Point2D(5, 0), geompp.Point2D(5, 10), geompp.Point2D(4, 10), geompp.Point2D(4, 9),
            geompp.Point2D(3, 9), geompp.Point2D(3, 10), geompp.Point2D(2, 10), geompp.Point2D(2, 9),
            geompp.Point2D(1, 9), geompp.Point2D(1, 10), geompp.Point2D(0, 10), geompp.Point2D(0, 0)])
        triangles = p.triangulate()
        assert len(triangles) == 10
        assert approx(sum(t.area() for t in triangles), p.area())
        assert_no_polygon_vertex_hangs_on_triangle_edge(p, triangles)
        assert_half_edges_are_manifold(triangles)
