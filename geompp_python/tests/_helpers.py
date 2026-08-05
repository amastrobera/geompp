"""
Shared assertion helpers for the geompp Python binding tests, used across multiple test_*.py files.
"""

import geompp


def approx(a, b, eps=1e-9):
    return abs(a - b) < eps


def assert_no_polygon_vertex_hangs_on_triangle_edge(polygon, triangles):
    # A valid triangulation must never let an original polygon vertex land in the MIDDLE of a
    # triangle edge (a "hanging"/T-junction vertex) -- every polygon vertex the triangulation touches
    # must be an actual corner (endpoint) of every triangle edge it lies on.
    polygon_points = polygon.perimeter()
    for ti, t in enumerate(triangles):
        v0, v1, v2 = t.vertices
        edges = [(v0, v1), (v1, v2), (v2, v0)]
        for ei, (a, b) in enumerate(edges):
            seg = geompp.LineSegment2D.make(a, b)
            for p in polygon_points:
                if p == a or p == b:
                    continue  # p IS this edge's endpoint -- not a hanging vertex
                assert not seg.contains(p), (
                    f"polygon vertex {p.to_wkt()} hangs on triangle {ti}'s edge {ei} "
                    f"({a.to_wkt()} -> {b.to_wkt()}) without being one of its endpoints")


def assert_half_edges_are_manifold(triangles):
    # A valid triangulation must be edge-manifold: every oriented triangle edge (a "half-edge")
    # either has no twin at all (a polygon boundary edge, used by exactly one triangle) or has
    # EXACTLY one twin in another triangle traversing the same undirected edge in the OPPOSITE
    # direction (twin.start == edge.end and twin.end == edge.start) -- the standard consequence of
    # every triangle sharing the same CCW winding.
    half_edges = []
    for ti, t in enumerate(triangles):
        v0, v1, v2 = t.vertices
        for ei, (s, e) in enumerate([(v0, v1), (v1, v2), (v2, v0)]):
            half_edges.append((s, e, ti, ei))
    for i, (s, e, ti, ei) in enumerate(half_edges):
        twins = [(s2, e2, tj, ej) for j, (s2, e2, tj, ej) in enumerate(half_edges)
                 if j != i and ((s == s2 and e == e2) or (s == e2 and e == s2))]
        assert len(twins) <= 1, (
            f"triangle {ti}'s edge {ei} ({s.to_wkt()} -> {e.to_wkt()}) has {len(twins)} twins "
            f"-- should have at most 1")
        if twins:
            s2, e2, tj, ej = twins[0]
            assert s2 == e and e2 == s, (
                f"triangle {ti}'s edge {ei} ({s.to_wkt()} -> {e.to_wkt()}) and triangle {tj}'s "
                f"edge {ej} ({s2.to_wkt()} -> {e2.to_wkt()}) share an undirected edge but traverse "
                f"it in the SAME direction")
