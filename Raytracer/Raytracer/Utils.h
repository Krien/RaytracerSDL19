#pragma once
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
