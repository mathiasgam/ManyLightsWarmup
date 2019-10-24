#include "TriMesh.h"

bool intersect_triangle(const Ray& ray,
	const Vec3f& v0,
	const Vec3f& v1,
	const Vec3f& v2,
	Vec3f& n,
	float& t,
	float& v,
	float& w)
{
	// Find vectors for edges sharing v0
	Vec3f e0 = v1 - v0;
	Vec3f e1 = v0 - v2;
	
	// Find face normal
	n = cross(e0, e1);

	// Compute ray-plane intersection
	float q = dot(ray.direction, n);
	if (fabsf(q) < 1.0e-12f)
		return false;
	q = 1.0f / q;
	Vec3f o_to_v0 = v0 - ray.center;
	t = dot(o_to_v0, n)*q;

	// Check distance to intersection
	if (t < ray.t_min || t > ray.t_max)
		return false;

	// Find barycentric coordinates
	Vec3f n_tmp = cross(o_to_v0, ray.direction);
	v = dot(n_tmp, e1) * q;
	if (v < 0.0f)
		return false;
	w = dot(n_tmp, e0)*q;
	if (w < 0.0f || v + w > 1.0f)
		return false;

	// Let the counterclockwise wound side face forward
	if (q>0)
		n = -n;
	return true;
}

TriMesh::TriMesh()
{
}


TriMesh::~TriMesh()
{
}


bool TriMesh::intersect(Ray & ray, HitInfo & hit, unsigned int i) const
{
	const Vec3ui face = geometry.face(i);
	Vec3f normal;
	float t, v, w;
	if (intersect_triangle(ray, geometry.vertex(face[0]), geometry.vertex(face[1]), geometry.vertex(face[2]), normal, t, v, w)) {
		if (t < hit.t) {
			hit.normal = normal;
			hit.t = t;
			hit.position = ray.center + ray.direction * t;
			return true;
		}
	}
	return false;
}

AABB TriMesh::get_bbox() const
{
	AABB bbox;
	const unsigned int N = geometry.num_vertices();
	for (int i = 0; i < N; i++) {
		bbox.add_point(geometry.vertex(i));
	}
	return bbox;
}

AABB TriMesh::get_bbox(unsigned int i) const
{
	AABB bbox;
	Vec3ui face = geometry.face(i);
	bbox.add_point(geometry.vertex(face[0]));
	bbox.add_point(geometry.vertex(face[1]));
	bbox.add_point(geometry.vertex(face[2]));
	return bbox;
}

void TriMesh::normalize()
{
	AABB bbox = this->get_bbox();
	Vec3f size = bbox.size();
	float max_size = fmaxf(size[0], fmaxf(size[1],size[2]));
	Vec3f translation = bbox.center();
	float inverse = 1.0f / max_size;
	const auto num_vertices = geometry.num_vertices();
	for (int i = 0; i < num_vertices; i++) {
		geometry.vertices[i] *= inverse;
		geometry.vertices[i] -= translation*inverse;
	}
}

void TriMesh::translate(const Vec3f& diff)
{
	unsigned int num_vertices = geometry.num_vertices();
	for (int i = 0; i < num_vertices; i++) {
		geometry.vertices[i] += diff;
	}
}
