#pragma once


struct Mat8 {
	__m256 ambientX, ambientY, ambientZ;
	__m256 diffuseX, diffuseY, diffuseZ;
	__m256 specularX, specularY, specularZ;
	__m256 absorbX, absorbY, absorbZ;
	__m256 mirror;
	__m256 refracIndex;
};

struct HitInfo8 {
	__m256 nx, ny, nz;
	__m256 px, py, pz;
	__m256 dist;
	__m256 matId; 
};


// READ ONLY struct
struct AvxVector3 {
	const __m256 x;
	const __m256 y;
	const __m256 z;
};



// Helper functions before I found out agner had his own :(.
inline float vec4f_length(Vec4f in)
{
	return sqrtf(horizontal_add(square(in)));
}

inline Vec4f vec4f_normalized(Vec4f in)
{
	return in / vec4f_length(in);
}

inline Vec4i vec4f_toColor(Vec4f in)
{
	assert(in.extract(0) <= 1.0f && in.extract(1) <= 1.0f && in.extract(2) <= 1.0f && in.extract(3) <= 1.0f);
	// Scale to appropriate space
	Vec4i out = roundi(in * Vec4f(255));
	return out;
	// Hope this conversion can be done quickly.
	//return (((int)in.extract(0) << 24) | ((int)in.extract(1) << 16)) | (((int)in.extract(2) << 8) | ((int)in.extract(3)));
}

inline __m256 dot_product(__m256 x1, __m256 y1, __m256 z1, __m256 x2, __m256 y2, __m256 z2) {
	return _mm256_add_ps(_mm256_add_ps(_mm256_mul_ps(x1, x2), _mm256_mul_ps(y1, y2)), _mm256_mul_ps(z1, z2));

}

inline __m256 dot_product(__m256 x, __m256 y, __m256 z) {
	return dot_product(x, y, z, x, y, z);
}

inline __m256 vector_length(__m256 x, __m256 y, __m256 z) {
	return _mm256_sqrt_ps(dot_product(x, y, z));
}

inline AvxVector3 normalize(__m256 x, __m256 y, __m256 z) {
	__m256 invLen = _mm256_invsqrt_ps(dot_product(x, y, z));
	__m256 normX = _mm256_mul_ps(invLen, x);
	__m256 normY = _mm256_mul_ps(invLen, y);
	__m256 normZ = _mm256_mul_ps(invLen, z);
	return AvxVector3{ normX, normY, normZ };
}


inline AvxVector3 normalize(AvxVector3 v) {
	return normalize(v.x, v.y, v.z);
}

inline __m256 dot_product(AvxVector3 v1, AvxVector3 v2) {
	return dot_product(v1.x, v1.y, v1.z, v2.y, v2.y, v2.z);
}

inline __m256 vector_length(AvxVector3 v) {
	return vector_length(v.x, v.y, v.z);
} 